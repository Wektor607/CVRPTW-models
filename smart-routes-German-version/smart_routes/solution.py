import numpy as np
from pydantic import BaseModel

from instance import Instance
from classical_heuristics.twMethods import *
from collections import deque
from utils.visualizer import *
import numpy as np
from problem import Problem
from parameters import GenerationParameters


class Solution(BaseModel):
    algorithm: str
    cost: float
    data: Instance
    routes: np.ndarray
    lst_results: list 
    count_towns: int

    class Config:
        arbitrary_types_allowed = True
        
    def compute_metrics(self):
        raise NotImplementedError

    def visualize_solution(self):
        # call visualize_solution
        raise NotImplementedError
    
    def visualize_results(self):
        # call visualize_metrics
        visualize_metrics(self.lst_results, self.count_towns)


if __name__ == '__main__':
    print("Введите название алгоритма, который будет решать задачи (SA, 2Opt, 3Opt, LKH, JAMPR): ")
    alg_name = input()
    if(alg_name in ['SA', '2Opt', '3Opt', 'LKH']):
        print("Введите количество городов(50, 100): ")
        n = input()
        while(n != '20' and n != '50' and n != '100'):
            print("Введите количество городов(50, 100): ")
            n = input()
        
        if(os.path.exists(f"classical_heuristics/current_result") == False):
            os.mkdir(f"classical_heuristics/current_result")

        if(os.path.exists(f"classical_heuristics/results") == False):
            os.mkdir(f"classical_heuristics/results")

        lst = []
        for i in range(2, 3): #100
            lst.append(f"instances/VRP_{n}/Example{i}.csv")
        lst_results = []
    
        idx = 0
        i = 0
        open("classical_heuristics/current_result/start_tour.txt", "w")
        
        while(i < len(lst)):
            with open(lst[i]) as f:
                count_towns = sum(1 for _ in f) - 1
                folder_name = f"classical_heuristics/{count_towns-1}_tw"
                if(os.path.exists(folder_name) == False):
                    os.mkdir(folder_name)
            with open(lst[i]) as f:
                [last_line] = deque(f, maxlen=1)
                last_line = last_line.split('\t')[3].split('-')
            if(count_towns == 21):
                max_capacity = 500
            elif(count_towns == 51 or count_towns == 11):
                max_capacity = 750
            elif(count_towns == 101):
                max_capacity = 1000
            
            shuffle_param = 0

            start = int(last_line[0]) 
            end   = int(last_line[1])
            print(f'LKH_{idx}')

            lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
            with open(f'classical_heuristics/current_result/LKH_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'classical_heuristics/results/LKH_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                    write_file.write(data)
            
            if(f'classical_heuristics/results/LKH_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                lst_results.remove(f'classical_heuristics/results/LKH_RES_CVRPTW{count_towns-1}.txt')
                lst_results.append(f'classical_heuristics/results/LKH_RES_CVRPTW{count_towns-1}.txt')
            else:
                lst_results.append(f'classical_heuristics/results/LKH_RES_CVRPTW{count_towns-1}.txt')

            idx += 1
            i   += 1
        
        generate_param = GenerationParameters(n_customer=n, n_samples=1)
        create_problem = Problem(capacity=True, time_windows=True)
        RS = Solution(
                    algorithm = '',
                    cost = 1.0,
                    data = Instance(problem=create_problem, generation_parameters=generate_param, coordinates=np.array([[0, 1]])),
                    routes = np.array([[0, 1]]),
                    lst_results = lst_results, 
                    count_towns = count_towns)
        RS.visualize_results()

    elif(alg_name == 'JAMPR'):
        gp = GenerationParameters()
        rp = Problem()
        ri = Instance(problem=rp, generation_parameters=gp, coordinates=np.array([[0, 1]]))

        RS = Solution(algorithm='jampr',
                    cost=1.0,
                    data=ri,
                    routes=np.array([[0, 1]]))
        print(RS.dict())
