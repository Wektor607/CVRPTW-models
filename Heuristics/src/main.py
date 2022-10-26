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
    for i in range(1, 5):
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
    print('Какой из методов вы хотите использовать для оптимизация маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')
    method = input()
    shuffle_param = 0
    i = 0
    open("current_result/start_tour.txt", "w")
    meth_alg = ['SA', 'LKH', 'OptAlg', 'Gurobi']
    while(1):
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

            if(method == 'SA'):
                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                sa = CVRPTW('SA', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end, dist_param).sa() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                with open(f'current_result/SA_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'results/SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
        
                if(f'results/SA_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                    lst_results.remove(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
                    lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
                else:
                    lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')

            elif(method == 'OptAlg'):
                while(1):
                    print("Input 2opt or 3opt:")
                    name_opt = input()
                    if(name_opt == '2opt' or name_opt == '3opt'):
                        break
                    else:
                        print('Try again!')

                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end, dist_param).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
                idx += 1
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

            elif(method == 'LKH'):
                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, dist_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                with open(f'current_result/LKH_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'results/LKH_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
                
                if(f'results/LKH_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                    lst_results.remove(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')
                    lst_results.append(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')
                else:
                    lst_results.append(f'results/LKH_RES_CVRPTW{count_towns-1}.txt')

            elif(method == 'Gurobi'):
                gurobi = CVRPTW('Gurobi', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, count_vehicles).gurobi() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                infile = open(f'Gurobi_{n}.txt',  'r')
                outfile = open(f'Gurobi_{n}_CVRPTW_dist.txt',  'w')

                pst = ['-', 'best', 'continuous,', 'Mon','Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun','second', '']
                copy = False
                k = n
                prev_content = '0'
                if(k == '20'):
                    time_constraint = 300
                elif(k == '50'):
                    time_constraint = 1000

                infile = open(f'Gurobi_{n}_dist.txt',  'r')
 
                for line in infile:
                    if 'Expl Unexpl |  Obj  Depth IntInf | Incumbent    BestBd   Gap | It/Node Time' in line:
                        copy = True
                    elif 'Cutting planes:' in line:
                        copy = False
                    elif copy:
                        content = line.split()
                        new_line = ''
                        if len(content) == 10:
                            if(content[5] not in pst):
                                flag = True
                                for i in pst:
                                    if(content[5].find(i) != -1):
                                        flag = False
                                if(flag):
                                    new_line += content[5]
                                    if(content[-1][:-1].isdigit() == True):
                                        if(int(content[-1][:-1]) < time_constraint):
                                            prev_content = int(content[-1][:-1])
                                else:
                                    outfile.write('')
                            else:
                                outfile.write('')
                        elif len(content) == 8:
                            if(content[3] not in pst):
                                flag = True
                                for i in pst:
                                    if(content[3].find(',') != -1):
                                        flag = False
                                if(flag):
                                    new_line += content[3]
                                    if(content[-1][:-1].isdigit() == True):
                                        if(int(content[-1][:-1]) < time_constraint):
                                            prev_content = int(content[-1][:-1])
                                else:
                                    outfile.write('')
                            else:
                                outfile.write('')
                        if len(content) != 0 and content[-1] != 'processors)' and '.' not in content[-1] and content[-1].isdigit() == False:
                            new_line += '\t' + content[-1] 
                            new_line = new_line[:-1] + '\n'
                            if(new_line[0] != '\t' and new_line[0] != ''):
                                outfile.write(new_line)
                            if(k == 20):
                                if(content[-1][:-1].isdigit() == True):
                                    print(content[-1][:-1], prev_content)
                                    if(int(content[-1][:-1]) == time_constraint or int(content[-1][:-1]) < int(prev_content)):
                                        outfile.write('\n')
                                        prev_content = '0'
                            elif(k == 50):
                                if(content[-1][:-1] == '1000'):
                                    outfile.write('\n')    
                infile.close()
                outfile.close()
                #TODO: добавить сюда сохранение результатов в файл
            if(method not in lst_legend):
                if(method == 'OptAlg'):
                    method = name_opt    
                lst_legend.append(method)

            z = input()
            if(z == ''):
                print('Вы хотите использовать другой алгоритм для оптимизации этой задачи (y/n):')
                z = input()
            while(1):
                if(z == 'n'):
                    break
                elif(z == 'y'):
                    print('Какой из методов вы хотите использовать для оптимизация маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')
                    method = input()
                    while(method not in meth_alg):
                        print('Попробуйте ещё раз!')
                        print('Какой из методов вы хотите использовать для оптимизация маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')    
                        method = input()
                    shuffle_param = 1
                    break
                else:
                    print('Попробуйте ещё раз!')
                    print('Вы хотите использовать другой алгоритм для оптимизации этой задачи (y/n):')
                    z = input()
            if(z == 'n'):
                print('Вы хотите закончить работу программы (YES/NO):')
                z = input()
                while(1):
                    if(z == 'YES'):
                        break
                    elif(z == 'NO'):
                        print('Какой из методов вы хотите использовать для решения новой задачи оптимизации маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')
                        method = input()
                        while(method not in meth_alg):
                            print('Попробуйте ещё раз!')
                            print('Какой из методов вы хотите использовать для решения новой задачи оптимизации маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')    
                            method = input()
                        shuffle_param = 0
                        i += 1
                        break
                    else:
                        print('Попробуйте ещё раз!')
                        print('Вы хотите закончить работу программы (YES/NO):')
                        z = input()
            if(z == 'YES'):
                break
        if(z == 'YES'):
            break


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
    plt.xlabel('Время оптимизации (секунды)', fontsize=7)
    plt.ylabel('Средняя стоимость (секунды)', fontsize=7)
    plt.title(f'{count_towns-1} points', fontsize=16)

    plt.savefig('graphs/All_Algorithms.png')

            
if __name__ == "__main__":
    main()