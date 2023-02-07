import numpy as np
import os
import math
import matplotlib.pyplot as plt

def compute_metrics(routes: np.ndarray, *args, **kwargs) -> np.ndarray:
    pass

def visualize_solution(points: np.ndarray, routes: np.ndarray, graph_path: str) -> None:
    pass

"""
Функция, строящая графики по всем алгоритмам, которые решали конкретные экземпляры задач
"""
def visualize_metrics(lst_results: list, count_towns: int) -> None:
    meth_alg = ['SA', '2Opt', '3Opt', 'LKH']

    if(os.path.exists(f"graphs") == False):
        os.mkdir(f"graphs")

    for name in lst_results:
        with open(name, 'r') as f:
            file = f.readlines()
        costs = []
        times = []

        cur_cost = []
        cur_time = []

        m = float('-inf')
        for line in file:
            if line == '\n':
                costs.append(cur_cost)
                times.append(cur_time)

                cur_cost = []
                cur_time = []

                continue

            line = line.split('\t')
            cur_cost.append(float(line[0]))
            cur_time.append(float(line[1][:-1]))

            if cur_time[-1] > m:
                m = cur_time[-1]
        step = 1
        m = math.ceil(m)

        time_batched = [[0] * (m // step) for _ in range(len(times))]
        cost_batched = [[float('inf')] * (m // step) for _ in range(len(costs))]

        for num in range(len(costs)):
            cur_time = times[num]
            cur_cost = costs[num]

            for n in range((m // step)):
                if n >= len(cur_time):
                    time_batched[num][n] = time_batched[num][n-1]
                    cost_batched[num][n] = cost_batched[num][n-1]
                    continue

                cost_batched[num][int(n // step)] = min(cost_batched[num][int(n // step)], cur_cost[n])
                time_batched[num][int(n // step)] = max(time_batched[num][int(n // step)], cur_time[n])
        cost_batched = np.array(cost_batched)
        time_batched = np.array(time_batched)
        plt.plot(time_batched.mean(0), cost_batched.mean(0))
        
    plt.legend(meth_alg)
    plt.xlabel('Время оптимизации (секунды)', fontsize=7)
    plt.ylabel('Средняя стоимость (метры)', fontsize=7)
    plt.title(f'{count_towns-1} points', fontsize=16)

    plt.savefig('graphs/All_Algorithms.png')
