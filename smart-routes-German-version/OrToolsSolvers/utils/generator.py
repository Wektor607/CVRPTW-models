import numpy as np

from OrToolsSolvers.parameters import GenerationParameters
from OrToolsSolvers.problem import Problem
from smart_routes.utils.distance import Distancer, EuclidianDistancer

CAPACITIES = {
    10: 20.,
    20: 30.,
    50: 40.,
    100: 50.,
    200: 60.,
    400: 70.,
    1000: 80.
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


def __generate_dataset(problem: Problem, gp: GenerationParameters, distancer: Distancer = EuclidianDistancer()):
    if problem.time_windows:
        cap_set = TW_CAPACITIES
    else:
        cap_set = CAPACITIES
    if gp.rnds == None:
        rnds = np.random
    # sample locations
    dloc = rnds.uniform(size=(gp.n_samples, gp.depot_num, 2))  # depot location
    nloc = rnds.uniform(size=(gp.n_samples, gp.n_customer, 2))  # node locations

    coordinates = np.concatenate([dloc, nloc], axis=1)
    result = {
        'problem': [problem for _ in range(gp.n_samples)],
        'coordinates': coordinates,
        'distance_matrix': distancer.distance_matrix(coordinates),
    }
    if problem.capacity:
        result['vehicle_capacities'] = np.full((gp.n_samples, 1), cap_set[gp.n_customer])
        result['nodes_demands'] = np.minimum(np.maximum(
            np.abs(rnds.normal(loc=gp.mu, scale=gp.std, size=[gp.n_samples, gp.n_customer])).astype(
                int),
            gp.capacity_min), gp.capacity_max)
    if problem.time_windows:
        # TW start needs to be feasibly reachable directly from depot
        min_t = np.ceil(np.linalg.norm(dloc[:, None, :] * gp.time_factor - nloc * gp.time_factor, axis=-1)) + 1
        # TW end needs to be early enough to perform service and return to depot until end of service window
        max_t = np.ceil(
            np.linalg.norm(dloc[:, None, :] * gp.time_factor - nloc * gp.time_factor,
                           axis=-1) + gp.service_duration) + 1

        # horizon allows for the feasibility of reaching nodes /
        # returning from nodes within the global tw (service window)
        horizon = list(zip(min_t, gp.service_window - max_t))
        epsilon = np.maximum(np.abs(rnds.standard_normal([gp.n_samples, gp.n_customer])), 1 / gp.time_factor)

        # sample earliest start times a
        a = [rnds.randint(*h) for h in horizon]
        # calculate latest start times b, which is
        # a + service_time_expansion x normal random noise, all limited by the horizon
        # and combine it with a to create the time windows
        tw = [np.transpose(np.vstack((rt,  # a
                                      np.minimum(rt + gp.tw_expansion * gp.time_factor * sd, h[-1]).astype(int)  # b
                                      )))
              for rt, sd, h in zip(a, epsilon, horizon)]

        result['full_time_windows'] = np.array([[0, gp.service_window]] * gp.n_samples)
        result['time_windows'] = np.array(tw)
        result['service_durations'] = np.full([gp.n_samples, gp.n_customer], gp.service_duration)
        result['time_factors'] = np.array([[gp.time_factor]] * gp.n_samples)

    return result


def generate_instance(problem: Problem, gp: GenerationParameters = None, distancer: Distancer = EuclidianDistancer()):
    return generate_batch(problem, 1, gp, distancer)


def generate_batch(problem: Problem, batch_size: int = 1, gp: GenerationParameters = None,
                   distancer: Distancer = EuclidianDistancer()):
    if gp is not None:
        gp.n_samples = batch_size
    else:
        gp = GenerationParameters(n_samples=batch_size)
    return __generate_dataset(problem, gp, distancer)
