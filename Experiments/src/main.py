from itertools import count

from twMethods import *
from collections import deque
from datetime import datetime
import matplotlib.pyplot as plt
import os

def main():
    print("Введите количество городов(20, 50, 100): ")
    n = input()
    while(n != '20' and n != '50' and n != '100'):
        print("Введите количество городов(20, 50, 100): ")
        n = input()
    lst = []
    for i in range(1, 16):
        lst.append(f"{n}/Example{i}.csv")
    lst_results = []
    lst_legend = []
    print("Для вычисления расстояния использовать subtourDistanceTw?(y/n):")
    dt = input()
    if(dt == 'y'):
        dist_param = 1
    elif(dt == 'n'):
        dist_param = 0
   
    idx = 0
    i = 0
    open("current_result/start_tour.txt", "w")
    
    lst_legend = ['SA', '2Opt', '3Opt', 'LKH']
    while(i < len(lst)):
        with open(lst[i]) as f:
            count_towns = sum(1 for _ in f) - 1
        with open(lst[i]) as f:
            [last_line] = deque(f, maxlen=1)
            last_line = last_line.split('\t')[3].split('-')
        if(count_towns == 21):
            max_capacity   = 500
            count_vehicles = 11
        elif(count_towns == 51):
            max_capacity   = 750
            count_vehicles = 11
        elif(count_towns == 101):
            max_capacity   = 1000
            count_vehicles = 21
        
        folder_name = f"{count_towns-1}_tw"
        if(os.path.exists(folder_name) == False):
            os.mkdir(folder_name)

        shuffle_param = 0
        start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        print(f'SA_{idx}')
        sa = CVRPTW('SA', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end, dist_param).sa() #TODO: некоторые параметры брать автоматически из файла
        with open(f'current_result/SA_CVRPTW_result.txt', 'r') as res_file:
            data = res_file.read()
            with open(f'results/SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                write_file.write(data)

        if(f'results/SA_RES_CVRPTW{count_towns-1}.txt' in lst_results):
            lst_results.remove(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
            lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
        else:
            lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')

        shuffle_param = 1
        name_opt = '2opt'
        start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        print(f'2Opt_{idx}')
        opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end, dist_param).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
        if(name_opt == '2opt' or name_opt == '3opt'):
            with open(f'current_result/{name_opt}_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                    write_file.write(data)

        if(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt' in lst_results):
            lst_results.remove(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
            lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        else:
            lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        
        shuffle_param = 1
        name_opt = '3opt'
        start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        print(f'3Opt_{idx}')
        opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end, dist_param).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
        if(name_opt == '2opt' or name_opt == '3opt'):
            with open(f'current_result/{name_opt}_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                    write_file.write(data)

        if(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt' in lst_results):
            lst_results.remove(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
            lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        else:
            lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')

        shuffle_param = 1
        start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        print(f'LKH_{idx}')
        lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
        with open(f'current_result/LKH_CVRPTW_result.txt', 'r') as res_file:
            data = res_file.read()
            with open(f'results/LKH_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                write_file.write(data)
        
        if(f'results/LKH_RES_CVRPTW{count_towns-1}.txt' in lst_results):
            lst_results.remove(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')
            lst_results.append(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')
        else:
            lst_results.append(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')

        idx += 1
        i   += 1
    # вычисление средних значений по всем примерам и на основе этого построение графиков каждого из алгоритмов
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
        
    plt.legend(lst_legend)
    plt.xlabel('Время оптимизации (секунды)', fontsize=12)
    plt.ylabel('Средняя стоимость (секунды)', fontsize=12)
    plt.title(f'{count_towns-1} точек', fontsize=16)

    plt.savefig('graphs/All_Algorithms.png')

            
if __name__ == "__main__":
    main()