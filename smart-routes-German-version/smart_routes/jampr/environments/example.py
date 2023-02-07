import numpy as np

from cvrp_tw import Environment
import torch

from JAMPR.data import generate_data

if __name__ == '__main__':
    device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
    data = generate_data(device, n_samples=2, n_customer=10, seed=123, time_factor=10.)
    data = [item.cpu().numpy() for item in data]
    env = Environment(*data, n_vehicle=3)

    action = np.array([[[1], [2], [3]],
                       [[1], [2], [3]]
                       ])
    res = env.step(action)
    action = np.array([[[4], [5], [6]],
                       [[4], [5], [6]]
                       ])
    env.step(action)
    action = np.array([[[7], [8], [9]],
                       [[7], [8], [9]]
                       ])
    env.step(action)
    action = np.array([[[10], [0], [0]],
                       [[10], [0], [0]]
                       ])
    env.step(action)
    env.render(0)
    print(111)