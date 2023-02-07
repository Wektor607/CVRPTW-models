import torch
import torch.nn as nn

from layers import MultiHeadAttention, DotProductAttention
from data import generate_data
from decoder_utils import TopKSampler, CategoricalSampler
from environments.cvrp_tw import Environment as Env

num_vehicles = 20
active_veh_num = 6


class DecoderCell(nn.Module):
    def __init__(self, embed_dim=128, embed_dim_dec=256, veh_dim=64, n_heads=8, clip=10., **kwargs):
        super().__init__(**kwargs)

        self.embed_dim = embed_dim
        self.embed_dim_dec = embed_dim_dec
        self.veh_dim = veh_dim

        self.Wk1 = nn.Linear(embed_dim_dec, embed_dim_dec, bias=False)
        self.Wv = nn.Linear(embed_dim_dec, embed_dim_dec, bias=False)
        self.Wk2 = nn.Linear(embed_dim_dec, embed_dim_dec, bias=False)
        self.Wout = nn.Linear(embed_dim_dec, embed_dim_dec, bias=False)
        self.Wq = nn.Linear(embed_dim * 5, embed_dim_dec, bias=False)

        self.MHA = MultiHeadAttention(
            n_heads=n_heads, embed_dim=embed_dim_dec, need_W=False)
        self.SHA = DotProductAttention(
            clip=clip, return_logits=True, head_depth=embed_dim_dec)
        # SHA ==> Single Head Attention, because this layer n_heads = 1 which means no need to spilt heads

        # TODO: better place for encoders ?
        # g_s
        self.TourEncoder = nn.Sequential(
            nn.Linear(embed_dim, veh_dim),
            nn.Sigmoid(),
            nn.Linear(veh_dim, veh_dim)
        )
        # g_v
        self.VehicleEncoder = nn.Sequential(
            nn.Linear(5, embed_dim),
            nn.Sigmoid(),
            nn.Linear(embed_dim, veh_dim),
            nn.Sigmoid(),
            nn.Linear(veh_dim, veh_dim),
            nn.Sigmoid(),
            nn.Linear(veh_dim, veh_dim)
        )

        self.lin_veh = nn.Linear(embed_dim, embed_dim_dec)
        self.lin_node = nn.Linear(embed_dim, embed_dim_dec, bias=False)
        self.lin_nodes_veh = nn.Linear(embed_dim + 1, embed_dim_dec)

    def compute_static(self, node_embeddings, graph_embedding):
        self.node_proj = self.lin_node(graph_embedding[:, None, :])

    def compute_dynamic(self, C, M, mask):

        Q1 = self.Wq(C)
        V = self.Wv(M)
        K1 = self.Wk1(M)
        Q2 = self.MHA([Q1.unsqueeze(1), K1, V], mask=mask)
        Q2 = self.Wout(Q2)

        K2 = self.Wk2(M)

        logits = self.SHA([Q2, K2, None], mask=mask)
        return logits.squeeze(dim=1)

    def forward(self, x, encoder_output, return_pi=False, decode_type='sampling'):
        node_embeddings, graph_embedding = encoder_output
        self.compute_static(node_embeddings, graph_embedding)
        env = Env(*[e.cpu().detach().numpy()
                  for e in x], n_vehicle=num_vehicles, max_tour_len=200, problem_type='pdp')
        step_context = env.reset()
        batch = node_embeddings.shape[0]
        graph_size = node_embeddings.shape[1]

        next_node = torch.zeros(
            (batch, num_vehicles, 1), dtype=torch.long, device=node_embeddings.device)
        last_node = torch.zeros(
            (batch, num_vehicles, 1), dtype=torch.long, device=node_embeddings.device)

        step_context = [torch.tensor(
            s, device=node_embeddings.device) for s in step_context]

        K = num_vehicles
        L = step_context[1].shape[2]

        # this variable will save all nodes embeddings after tour encoder
        gs_embeddings = torch.zeros(
            (batch, K, self.veh_dim, L), device=node_embeddings.device)

        selecter = {'greedy': TopKSampler(), 'sampling': CategoricalSampler()}.get(
            decode_type, None)
        log_ps, tours, act_ids = [], [], []
        for i in range(env.n_nodes*2):

            # vehicle features
            g_v_k = self.VehicleEncoder(step_context[2].type(torch.float32))

            # TODO: check if there is better way for indexing embeddings
            idx = torch.repeat_interleave(
                next_node, node_embeddings.shape[-1], dim=2)
            required_embs = node_embeddings.gather(index=idx, dim=1)
            g_s_k = self.TourEncoder(required_embs)
            # gs_embeddings[..., i].scatter_(dim=1, index=next_node.expand(
            #     batch, active_veh_num, self.veh_dim), src=g_s_k)
            gs_embeddings[..., i] = g_s_k

            # route_features = gs_embeddings.gather(index=next_node[..., None].expand(
            #     batch, active_veh_num, self.veh_dim, L), dim=1).sum(dim=3) / L
            route_features = gs_embeddings.sum(dim=3) / L
            vehicle = torch.cat((g_v_k, route_features), dim=2)
            fleet = vehicle.mean(dim=1)

            last_idx = torch.repeat_interleave(
                last_node, node_embeddings.shape[-1], dim=2)
            last = node_embeddings.gather(index=last_idx, dim=1).mean(dim=1)

            # only active vehicles further
            vehicle = vehicle.gather(index=step_context[3][..., None].expand(
                batch, active_veh_num, vehicle.shape[-1]), dim=1)
            act = vehicle.mean(dim=1)

            C = torch.cat((graph_embedding, fleet, act,
                          node_embeddings[:, 0, :], last), dim=1)

            veh_proj = self.lin_veh(vehicle)

            mm = torch.matmul(vehicle, node_embeddings.permute(0, 2, 1)
                              ).reshape(batch, graph_size * active_veh_num, -1)
            pwm = (vehicle[:, :, :, None] * (node_embeddings.permute(0, 2, 1).unsqueeze(1))).permute(0, 1, 3, 2).reshape(batch,
                                                                                                                         graph_size * active_veh_num,
                                                                                                                         -1)
            g_a = self.lin_nodes_veh(torch.cat((mm, pwm), dim=2)).reshape(
                batch, active_veh_num, graph_size, -1)
            g_a = g_a + self.node_proj[:, None, :, :] + veh_proj[:, :, None, :]
            M = g_a.reshape(batch, graph_size * active_veh_num, -1)

            logits = self.compute_dynamic(C, M, step_context[0].view(
                batch, -1, 1)).view(batch, active_veh_num, graph_size)
            log_p = torch.log_softmax(logits, dim=-1)
            last_node = next_node.clone()
            result = selecter(log_p)
            next_node.scatter_(
                dim=1, index=step_context[3][..., None], src=result)
            step_context = env.step(
                next_node.gather(dim=1, index=step_context[3][..., None]).squeeze(-1).cpu().detach().numpy())
            step_context = [torch.tensor(
                s, device=node_embeddings.device) for s in step_context]
            tours.append(result)
            log_ps.append(log_p.gather(index=result, dim=-1))
            act_ids.append(torch.tensor(
                env.act_veh_idx[..., None], device=x[0].device))
            if env.mask[..., env.depo_num:].all():
                break
        env.render(0, "??.png")
        pi = torch.stack(tours, 1)
        pi_act_ids = torch.stack(act_ids, 1)
        cost = env.get_costs(pi, pi_act_ids)
        # env.get_log_likelihood(torch.stack(log_ps, 1), pi)
        ll = torch.stack(log_ps, 1).sum(-1).sum(-1).sum(-1)

        if return_pi:
            return cost, ll, pi
        return cost, ll


if __name__ == '__main__':
    batch, n_nodes, embed_dim = 5, 21, 128
    data = generate_data(device=torch.device(
        'cpu'), n_samples=batch, n_customer=n_nodes-1)
    decoder = DecoderCell(embed_dim, n_heads=8, clip=10.)
    node_embeddings = torch.rand(
        (batch, n_nodes, embed_dim), dtype=torch.float)
    graph_embedding = torch.rand((batch, embed_dim), dtype=torch.float)
    encoder_output = (node_embeddings, graph_embedding)
    # a = graph_embedding[:,None,:].expand(batch, 7, embed_dim)
    # a = graph_embedding[:,None,:].repeat(1, 7, 1)
    # print(a.size())

    decoder.train()
    cost, ll, pi = decoder(data, encoder_output,
                           return_pi=True, decode_type='sampling')
    print('\ncost: ', cost.size(), cost)
    print('\nll: ', ll.size(), ll)
    print('\npi: ', pi.size(), pi)

    # cnt = 0
    # for i, k in decoder.state_dict().items():
    # 	print(i, k.size(), torch.numel(k))
    # 	cnt += torch.numel(k)
    # print(cnt)

    # ll.mean().backward()
    # print(decoder.Wk1.weight.grad)
    # https://discuss.pytorch.org/t/model-param-grad-is-none-how-to-debug/52634
