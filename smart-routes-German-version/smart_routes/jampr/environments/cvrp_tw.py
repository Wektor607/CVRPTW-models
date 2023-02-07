from collections import namedtuple
from typing import List, Tuple
import matplotlib.pyplot as plt
import numpy as np
import torch
from environments.distance_measures import EuclidianDistancer


class Environment:
    def __init__(self, depot_location, node_locations, demands,
                 capacities, depot_tws, node_tws, durations,
                 service_windows, time_factors,
                 n_vehicle: int = 10, max_tour_len: int = 20,
                 active_vehicles_num=6, env_type: str = 'synthetic', problem_type='cvrp'):
        """
        get data from dataloader
        """
        assert env_type in ['synthetic', 'real'], 'env type not supported'
        assert problem_type in ['cvrp', 'pdp'], 'problem type not supported'

        self.depot_location = depot_location  # batch_size x 2
        self.node_locations = node_locations  # batch_size x n_nodes x 2
        self.node_locations = np.concatenate(
            (depot_location, node_locations), axis=1)
        self.demands = demands  # batch_size x n_nodes
        self.capacities = capacities  # batch_size
        self.depot_tws = depot_tws  # batch_size x 1 x 2
        self.node_tws = node_tws  # batch_size x n_nodes x 2
        self.durations = durations  # batch_size x 1
        self.service_windows = service_windows  # batch_size x 1
        self.time_factors = time_factors  # batch_size x 1

        self.n_vehicles = n_vehicle  # K
        self.act_vehicles = active_vehicles_num  # number of active vehicles

        self.max_tour_len = max_tour_len
        self.env_type = env_type

        self.n_instances = self.node_locations.shape[0]
        self.n_nodes = self.node_locations.shape[1]
        
        self.depo_collocations = []
        self.depo_num = self.depot_location.shape[1]
        
        for i in range(self.depot_location.shape[0]):
            cur_batch = []
            indxs = list(range(self.depo_num, self.n_nodes))
            prev_bord = 0
            cur_bord = len(indxs) // self.depo_num + len(indxs) % self.depo_num
            for j in range(self.depo_num):
                cur_batch.append(indxs[prev_bord : cur_bord])
                prev_bord = cur_bord
                cur_bord += len(indxs) // self.depo_num
            self.depo_collocations.append(cur_batch)
        
        self.distancer = EuclidianDistancer()

        self.distance_matrix = self.distancer.batched_distance_matrix(
            self.node_locations)

        self.mask = np.zeros(
            shape=(self.n_instances, self.n_vehicles, self.n_nodes), dtype=bool)
        # self.mask[:, :, 0] = 1  # init depot
        self.tour_plan = np.zeros(shape=(self.n_instances,
                                         self.n_vehicles,
                                         self.max_tour_len), dtype=int)

        self.vehicle_indices = np.arange(0, self.n_vehicles)
        # set to zeros on init (all vehicle in one place)
        self.vehicle_return_to_dep_cost = np.zeros(shape=(self.n_instances,
                                                          self.n_vehicles,
                                                          1))
        # generate initial positions [n_instances x n_vehicle x 2
        # if vehicle positions are not the same it doesn't work
        self.vehicle_positions = np.repeat(self.depot_location[:, :1],
                                           self.n_vehicles,
                                           axis=1).reshape(self.n_instances,
                                                           self.n_vehicles, 2)
        # self.vehicle_positions = np.stack([np.stack([self.depot_location] * self.n_vehicles)
        #                                    for _ in range(self.n_instances)])
        self.vehicle_current_time = np.zeros(
            shape=(self.n_instances, self.n_vehicles, 1), dtype=float)

        self.vehicle_to_node_id = np.zeros(
            shape=(self.n_instances, self.n_vehicles, 1))

        self.vehicle_remain_capacities = np.repeat(self.capacities.reshape(-1, 1),
                                                   self.n_vehicles, axis=1)

        self.vehicle_depot_deadlines = np.tile(self.depot_tws, self.n_vehicles).reshape(self.n_instances,
                                                                                        self.n_vehicles,
                                                                                        2)

        self.vehicle_node_tws = np.repeat(
            self.node_tws[:, np.newaxis, ...], self.n_vehicles, axis=1)

        # self.vehicle_remain_time = np.repeat(self.depot_location, self.n_instances)

        self.n_step = 0
        self.done_games = np.zeros(shape=(self.n_instances,))
        self.done_nodes = np.zeros(
            shape=(self.n_instances, self.n_nodes), dtype=bool)
        self.done_nodes[:, 0] = True

        self.missed_nodes_multiplier = 1
        self.num_act_veh_multiplier = 1

    def reset(self):
        """

        :return: state (
            s0 <
                mask[active_vehicles x n_nodes], <- visited states = [True, False, ..., False]
                S_[K x L], <- tour plan for vehicles
                v_[K x 5], <- vehicle features
                idx_[active_vehicles], <- indices of current active vehicles
            >
        )
        """
        self.act_veh_idx = np.arange(self.act_vehicles)[None, :]
        self.act_veh_idx = np.repeat(self.act_veh_idx, self.n_instances, 0)

        self.acted_veh = np.zeros(
            (self.n_instances, self.n_vehicles)).astype(bool)
        self.acted_veh[:, :self.act_vehicles] = True

        self.mask = np.ones(shape=(self.n_instances, self.n_vehicles, self.n_nodes),
                             dtype=bool)
        
        self.mask[:, :, :self.depo_num] = 0 
        # self.mask[:, :, 0] = 1  # init depot
        self.tour_plan = np.zeros(shape=(self.n_instances,
                                         self.n_vehicles,
                                         self.max_tour_len),
                                  dtype=int)
        self.n_step = np.zeros((self.n_instances, self.n_vehicles), dtype=int)
        self.done_nodes = np.zeros(
            shape=(self.n_instances, self.n_nodes), dtype=bool)
        self.done_nodes[:, 0] = True
        return [
            np.take_along_axis(self.mask, self.act_veh_idx[..., None], 1),
            self.tour_plan,
            self.get_vehicle_features(),
            self.act_veh_idx
        ]

    def step(self, actions: np.ndarray):
        """

        :param actions: matrix_[KxN], N=1 n_samples x K x 1
        :return: state(
            mask[active_vehicles x n_nodes],
            S_[KxL],
            v_[K x 5]
        ),
        reward (

        )
        done True | False

        1. for each vehicle in actions

        """
        for i, sample in enumerate(actions):
            # update return cost
            self._update_vehicle_return_cost_and_time(i, sample)
            self._update_vehicle_position(i, sample)
            self._update_plan(i, sample)
            self._update_mask(i, sample)
            self._update_active_vehicles(i)
            self._update_done_nodes(i, sample)

        # self.n_step += 1
        np.put_along_axis(arr=self.n_step, indices=self.act_veh_idx, values=np.take_along_axis(
            self.n_step, self.act_veh_idx, 1) + 1, axis=1)

        # check done game

        return [
            np.take_along_axis(self.mask, self.act_veh_idx[..., None], 1),
            self.tour_plan,
            self.get_vehicle_features(),
            self.act_veh_idx
        ]

    def _update_done_nodes(self, n_sample, actions):
        self.done_nodes[n_sample, actions] = True

    def _update_active_vehicles(self, n_sample):
        for i in range(self.act_vehicles):
            if (self.mask[n_sample, self.act_veh_idx[n_sample][i], 1:]).all():
                # skip case of fully ended route
                if self.acted_veh[n_sample].sum() == self.n_vehicles:
                    continue
                self.act_veh_idx[n_sample, i] = self.acted_veh[n_sample].sum()
                self.acted_veh[n_sample, self.acted_veh[n_sample].sum()] = True

    def _update_vehicle_to_node_id(self, n_sample, actions):
        self.vehicle_to_node_id[n_sample] = actions

    def _update_current_time(self, n_sample, actions):
        """
        (distance * time_factors) + durations
        Args:
            n_sample:
            actions:

        Returns:

        """

        self.vehicle_current_time[n_sample] += self.distance_matrix[n_sample][actions]

    def _update_vehicle_return_cost_and_time(self, n_sample, actions):
        """
        distance to depot
        Args:
            n_sample:
            actions:

        Returns:

        """

        # previous_p = np.take_along_axis(
        #     arr=np.take_along_axis(
        #         self.tour_plan,
        #         self.act_veh_idx[..., None],
        #         axis=1
        #     ),
        #     indices=np.take_along_axis(
        #         self.n_step,
        #         self.act_veh_idx,
        #         axis=1
        #     )[..., None],
        #     axis=2
        # )

        # next_p = actions

        # prev = np.take_along_axis(self.distance_matrix, previous_p, 1)
        # np.put_along_axis(self.vehicle_return_to_dep_cost, self.act_veh_idx[..., None], axis=1, values=prev[..., 0])

        for i, (previous_p, next_p) in enumerate(zip(self.tour_plan[n_sample][self.act_veh_idx[n_sample],
                                                                              self.n_step[n_sample][self.act_veh_idx[n_sample]]],
                                                     actions.ravel())):
            self.vehicle_return_to_dep_cost[n_sample, self.act_veh_idx[n_sample, i],
                                            0] = self.distance_matrix[n_sample, previous_p, 0]
            # TODO: always false??
            if next_p == 0:
                self.vehicle_current_time[n_sample, self.act_veh_idx[n_sample, i], 0] += self.distance_matrix[n_sample, previous_p, next_p] * \
                    self.time_factors[n_sample]
                self.vehicle_depot_deadlines[n_sample, self.act_veh_idx[n_sample, i], 1] -= self.distance_matrix[n_sample, previous_p, next_p] * \
                    self.time_factors[n_sample]
            else:
                self.vehicle_current_time[n_sample, self.act_veh_idx[n_sample, i], 0] += self.distance_matrix[n_sample, previous_p, next_p] * \
                    self.time_factors[n_sample] + \
                    self.durations[n_sample, next_p - self.depo_num]
                self.vehicle_depot_deadlines[n_sample, self.act_veh_idx[n_sample, i], 1] -= self.distance_matrix[n_sample, previous_p, next_p] * \
                    self.time_factors[n_sample] + \
                    self.durations[n_sample, next_p - self.depo_num]

    def _update_vehicle_position(self, n_sample, actions):
        self.vehicle_positions[n_sample, self.act_veh_idx[n_sample]] = self.node_locations[n_sample][actions.ravel(
        )]

    def _update_plan(self, n_sample, actions):
        self.tour_plan[n_sample, self.act_veh_idx[n_sample],
                       self.n_step[n_sample, self.act_veh_idx[n_sample]]] = actions.ravel()

    def _update_mask(self, n_sample, actions):
        """
        1. visited
        2. capacities
        3. time windows
        4. service time
        5. pdp constraint

        compare with service_windows and mask unreachable
        Returns:

        """
        # mark visited
        for i, action in enumerate(actions):
            if (self.n_step[n_sample, self.act_veh_idx[n_sample, i]] == 0) and (action < self.depo_num):
                # first step, we are going to the different depots
                self.mask[n_sample, self.act_veh_idx[n_sample, i], self.depo_collocations[n_sample][action]] = 0
            elif (action < self.depo_num):
                # if next depot => end of tour for this vehicle
                self.mask[n_sample, self.act_veh_idx[n_sample, i], :] = 1
            else:
                self.mask[n_sample, :, action] = 1

        # update capacities
        demand_indices = actions.ravel() - self.depo_num
        demands_ = self.demands[n_sample][demand_indices]
        demands_[demand_indices < 0] = 0

        mask_same_nodes = actions != actions[0]
        mask_same_nodes[0] = True

        self.vehicle_remain_capacities[n_sample, self.act_veh_idx[n_sample][mask_same_nodes]
                                       ] -= self.demands[n_sample][demand_indices[mask_same_nodes]].ravel()

        for i in range(len(self.vehicle_remain_capacities[n_sample][self.act_veh_idx[n_sample]])):
            capacities_mask = np.where(self.demands[n_sample] > self.vehicle_remain_capacities[n_sample][self.act_veh_idx[n_sample][i]],
                                       True,
                                       False)
            self.mask[n_sample, self.act_veh_idx[n_sample]
                      [i], self.depo_num:][capacities_mask] = 1

        # update node tws
        for i in range(self.act_vehicles):
            current_node = self.tour_plan[n_sample, self.act_veh_idx[n_sample]
                                          [i], self.n_step[n_sample][self.act_veh_idx[n_sample][i]]]
            available_nodes_indices = np.where(
                ~self.mask[n_sample, self.act_veh_idx[n_sample][i], self.depo_num:])
            left_bounds = self.distance_matrix[n_sample][current_node][available_nodes_indices]
            left_bounds = left_bounds * \
                self.time_factors[n_sample] + \
                self.vehicle_current_time[n_sample,
                                          self.act_veh_idx[n_sample][i]]
            left_bounds += self.durations[n_sample][available_nodes_indices]

            new_left_bounds = np.maximum(
                self.vehicle_node_tws[n_sample, self.act_veh_idx[n_sample][i]][available_nodes_indices][:, 0], left_bounds)
            self.vehicle_node_tws[n_sample,
                                  self.act_veh_idx[n_sample][i]][available_nodes_indices][:, 0] = new_left_bounds
            unreachable_by_tws = np.where(
                self.vehicle_node_tws[n_sample, self.act_veh_idx[n_sample][i], :, 0] > self.vehicle_node_tws[n_sample, self.act_veh_idx[n_sample][i], :, 1], True, False)
            self.mask[n_sample, self.act_veh_idx[n_sample][i]][np.concatenate(
                (np.array([False for _ in range(self.depo_num)]), unreachable_by_tws))] = 1

            # update by service time (right bound of depot time)
            if self.vehicle_depot_deadlines[n_sample, self.act_veh_idx[n_sample][i], 1] <= 0:
                self.mask[n_sample, self.act_veh_idx[n_sample][i]][self.depo_num:] = 1
                
            for ind, routes in enumerate(self.depo_collocations[n_sample]):
                cur_node = self.tour_plan[n_sample, self.act_veh_idx[n_sample, i], self.n_step[n_sample, self.act_veh_idx[n_sample, i]]]
                if (cur_node != ind) and (cur_node not in routes):
                    self.mask[n_sample, self.act_veh_idx[n_sample][i], routes] = 1
        return self.mask, self.tour_plan, self.get_vehicle_features(), 1.0

    def render(self, n_sample, path=None):
        plt.figure(figsize=(10, 10))
        plt.scatter(self.node_locations[n_sample]
                    [:, 0], self.node_locations[n_sample][:, 1])
        plt.scatter(self.depot_location[n_sample, :, 0],
                    self.depot_location[n_sample, :, 1])

        colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']

        for i in range(self.n_vehicles):
            for j in range(self.depo_num):
                steps = self.tour_plan[n_sample, i, :self.n_step[n_sample, i] + 1]
                if len(steps) == 0:
                    continue

                c = colors[i % len(colors)]
                x, y = self.depot_location[n_sample, j]
                dx, dy = self.node_locations[n_sample, steps[0]]
                plt.arrow(x, y, dx - x, dy - y, shape='full', lw=0.2,
                        length_includes_head=True, head_width=.01, color=c)
                for j in range(1, len(steps)):
                    x, y = self.node_locations[n_sample, steps[j - 1]]
                    dx, dy = self.node_locations[n_sample, steps[j]]
                    plt.arrow(x, y, dx - x, dy - y, shape='full', lw=0.2,
                            length_includes_head=True, head_width=.01, color=c)
        plt.xticks(fontsize=24)
        plt.yticks(fontsize=24)
        plt.show()
        if path:
            plt.savefig(path)

    def get_vehicle_features(self):
        """
        1. vehicle index
        2. current return cost to depot
        3-4. current position of vehicle
        5. current time
        """
        return np.concatenate(
            [
                np.repeat(np.arange(self.n_vehicles)[
                          None, :, None], self.n_instances, 0),
                self.vehicle_return_to_dep_cost,
                self.vehicle_positions,
                self.vehicle_current_time
            ],
            axis=2)

    def get_log_likelihood(self, _log_p, pi):
        """ _log_p: (batch, decode_step, num_vehicles, n_nodes)
                pi: (batch, decode_step, num_vehicles, 1), predicted tour
        """
        log_p = torch.gather(input=_log_p, dim=-1, index=pi)
        return torch.sum(log_p.squeeze(-1), 1).sum(dim=-1)

    def get_costs(self, pi, pi_act_ids):
        """ self.node_locations: (batch, n_nodes, 2), Coordinates of depot + customer nodes
                pi: (batch, decode_step, num_vehicles, 1), predicted tour
                d: (batch, decode_step, num_vehicles, 2)
                Note: first element of pi is not depot, the first selected node in the path
        """
        ret_cost = torch.tensor(
            [0 for _ in range(pi.shape[0])], dtype=torch.float, device=pi.device)
        for batch in range(pi.shape[0]):
            for ind, step in enumerate(self.n_step[batch]):
                ret_cost[batch] += self.distance_matrix[batch,
                                                        0, self.tour_plan[batch, ind, 0]]
                ret_cost[batch] += self.distance_matrix[batch][self.tour_plan[batch,
                                                                              ind, :step][:-1], self.tour_plan[batch, ind, :step][1:]].sum()
                ret_cost[batch] += self.distance_matrix[batch,
                                                        self.tour_plan[batch, ind, step], 0]

        # missed nodes
        ret_cost += torch.tensor((~self.done_nodes).sum(-1),
                                 device=pi.device) * self.missed_nodes_multiplier

        # num_act_vehicles
        ret_cost += torch.tensor(self.acted_veh.sum(-1),
                                 device=pi.device) * self.num_act_veh_multiplier

        return ret_cost

        d = torch.gather(input=torch.tensor(self.node_locations[:, :, None, :], device=pi.device).repeat(1, 1, self.n_vehicles, 1), dim=1,
                         index=pi.repeat(1, 1, 1, 2))
        # d = torch.gather(input = self.xy, dim = 1, index = pi[:,:,None].expand(self.batch,pi.size(1),2))
        return (torch.sum((d[:, 1:] - d[:, :-1]).norm(p=2, dim=-1), dim=1)
                # distance from depot to first selected node
                + (d[:, 0] - torch.tensor(self.depot_location[:, None, :],
                   device=pi.device)).norm(p=2, dim=-1)
                # distance from last selected node (!=0 for graph with longest path) to depot
                + (d[:, -1] - torch.tensor(self.depot_location[:,
                   None, :], device=pi.device)).norm(p=2, dim=-1)
                ).sum(dim=-1)
