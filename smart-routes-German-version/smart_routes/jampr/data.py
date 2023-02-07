import torch
import numpy as np
import os
from tqdm import tqdm
from torch.utils.data.dataset import Dataset
from torch.utils.data import DataLoader
from collections import namedtuple

CAPACITIES = {
    10: 20.,
    20: 30.,
    50: 40.,
    100: 50.
}
TW_CAPACITIES = {
    10: 250.,
    20: 500.,
    50: 750.,
    100: 1000.,
    200: 3500.,
    400: 6000.,
    1000: 12000.
}

CVRPTW_SET = namedtuple("CVRPTW_SET",
                        ["depot_loc",  # Depot location
                         "node_loc",  # Node locations
                         "demand",  # demand per node
                         "capacity",  # vehicle capacity (homogeneous)
                         "depot_tw",  # depot time window (full horizon)
                         "node_tw",  # node time windows
                         "durations",  # service duration per node
                         "service_window",  # maximum of time units
                         "time_factor"])  # value to map from distances in [0, 1] to time units (transit times)


def generate_data(device, n_samples=10, n_customer=20,
                  service_window=1000,
                  service_duration=10,
                  time_factor=100.0,
                  tw_expansion=3.0,
                  seed=None,
                  task='cvrp',
                  depot_num=1):
    """
            Args:
            n_samples (int): size of dataset
            n_customer (int): size of problem instance graph (number of customers without depot)
            service_window (int): maximum of time units
            service_duration (int): duration of service
            time_factor (float): value to map from distances in [0, 1] to time units (transit times)
            tw_expansion (float): expansion factor of TW w.r.t. service duration
    Returns:
            List of CVRP-TW instances
    """
    if seed is not None:
        torch.manual_seed(seed)

    rnds = np.random

    # sample locations
    dloc = rnds.uniform(size=(n_samples, 1 if task == 'cvrp' else depot_num, 2))  # depot location
    nloc = rnds.uniform(size=(n_samples, n_customer, 2))  # node locations

    # TW start needs to be feasibly reachable directly from depot
    min_t = np.ceil(np.linalg.norm(
        dloc[:, :1] * time_factor - nloc * time_factor, axis=-1)) + 1
    # TW end needs to be early enough to perform service and return to depot until end of service window
    max_t = np.ceil(np.linalg.norm(
        dloc[:, :1] * time_factor - nloc * time_factor, axis=-1) + service_duration) + 1

    # horizon allows for the feasibility of reaching nodes / returning from nodes within the global tw (service window)
    horizon = list(zip(min_t, service_window - max_t))
    epsilon = np.maximum(np.abs(rnds.standard_normal(
        [n_samples, n_customer])), 1 / time_factor)

    # sample earliest start times a
    a = [rnds.randint(*h) for h in horizon]
    # calculate latest start times b, which is
    # a + service_time_expansion x normal random noise, all limited by the horizon
    # and combine it with a to create the time windows
    tw = [np.transpose(np.vstack((rt,  # a
                                  np.minimum(
                                      rt + tw_expansion * time_factor * sd, h[-1]).astype(int)  # b
                                  ))).tolist()
          for rt, sd, h in zip(a, epsilon, horizon)]

    return (
        torch.tensor(dloc, device=device, dtype=torch.float32),
        torch.tensor(nloc, device=device, dtype=torch.float32),
        torch.minimum(torch.maximum(torch.abs(torch.normal(15, 10, size=[
            n_samples, n_customer])).type(torch.int), torch.tensor(1)), torch.tensor(42)).to(device),
        torch.tensor(np.full(n_samples, TW_CAPACITIES[n_customer]), device=device, dtype=torch.float32),
        torch.tensor([[0, service_window]] * n_samples, device=device),
        torch.tensor(tw, device=device),
        torch.tensor(np.full([n_samples, n_customer], service_duration), device=device),
        torch.tensor([service_window] * n_samples, device=device),
        torch.tensor([time_factor] * n_samples, device=device)
    )

    # return (torch.rand((n_samples, 2), device = device),
    # 		torch.rand((n_samples, n_customer, 2), device = device),
    # 		torch.randint(size = (n_samples, n_customer), low = 1, high = 10, device = device) / CAPACITIES[n_customer])


class Generator(Dataset):
    """ https://github.com/utkuozbulak/pytorch-custom-dataset-examples
            https://github.com/wouterkool/attention-learn-to-route/blob/master/problems/vrp/problem_vrp.py
            https://github.com/nperlmut31/Vehicle-Routing-Problem/blob/master/dataloader.py
    """

    def __init__(self, device, n_samples=5120, n_customer=20, seed=None):
        self.tuple = generate_data(device, n_samples, n_customer, task='pdp', depot_num=3)

    def __getitem__(self, idx):
        return (self.tuple[0][idx], self.tuple[1][idx], self.tuple[2][idx],
            self.tuple[3][idx], self.tuple[4][idx], self.tuple[5][idx],
            self.tuple[6][idx], self.tuple[7][idx], self.tuple[8][idx])

    def __len__(self):
        return self.tuple[0].size(0)


def data_from_txt(path):
    if not os.path.isfile(path):
        raise FileNotFoundError
    with open(path, 'r') as f:
        lines = list(map(lambda s: s.strip(), f.readlines()))
        customer_xy, demand = [], []
        ZERO, DEPOT, CUSTO, DEMAND = [False for i in range(4)]
        ZERO = True
        for line in lines:
            if (ZERO):
                if (line == 'NODE_COORD_SECTION'):
                    ZERO = False
                    DEPOT = True

            elif (DEPOT):
                # depot_xy.append(list(map(int, line.split()))[1:])
                depot_xy = list(map(lambda k: float(k) / 100., line.split()))[1:]
                DEPOT = False
                CUSTO = True

            elif (CUSTO):
                if (line == 'DEMAND_SECTION'):
                    DEMAND = True
                    CUSTO = False
                    continue
                customer_xy.append(
                    list(map(lambda k: float(k) / 100., line.split()))[1:])
            elif (DEMAND):
                if (line == '1 0'):
                    continue
                elif (line == 'DEPOT_SECTION'):
                    break
                else:
                    # demand.append(list(map(int, line.split()))[1])
                    demand.append(
                        list(map(lambda k: float(k) / 100., line.split()))[1])

    # print(np.array(depot_xy).shape)
    # print(np.array(customer_xy).shape)
    # print(np.array(demand).shape)

    return (torch.tensor(np.expand_dims(np.array(depot_xy), axis=0), dtype=torch.float),
            torch.tensor(np.expand_dims(
                np.array(customer_xy), axis=0), dtype=torch.float),
            torch.tensor(np.expand_dims(np.array(demand), axis=0), dtype=torch.float))


if __name__ == '__main__':
    device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
    print('device-->', device)

    data = generate_data(device, n_samples=128, n_customer=20, seed=123)
    for i in range(3):
        print(data[i].dtype)  # torch.float32
        print(data[i].size())

    batch, batch_steps, n_customer = 128, 100000, 20
    dataset = Generator(device, n_samples=batch *
                                          batch_steps, n_customer=n_customer)
    data = next(iter(dataset))

    dataloader = DataLoader(dataset, batch_size=128, shuffle=True)
    print('use datalodaer ...')
    for i, data in enumerate(dataloader):
        for j in range(len(data)):
            print(data[j].dtype)  # torch.float32
            print(data[j].size())
        if i == 0:
            break

    path = '../OpenData/A-n53-k7.txt'
    data = data_from_txt(path)
    print('read file ...')
    data = list(map(lambda x: x.to(device), data))
    for da in data:
        print(data[j].dtype)  # torch.float32
        print(da.size())
