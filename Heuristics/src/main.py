from itertools import count

from sklearn.utils import shuffle
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
    for i in range(1, 4):
        lst.append(f"{n}/Example{i}.csv")
    lst_results = []
    lst_legend = []
    print('Если хотиет открыть документацию,то напишите YES в противном случае нажмите Enter')
    doc = input()
    if(doc == 'YES'):
        help(vrp_c) #TODO: в отредактировать текст в документации
    idx = 0
    print('Какой из методов вы хотите использовать для оптимизация маршрута для CVRPTW: SA or LKH or OptAlg or Gurobi?')
    method = input()
    shuffle_param = 0
    i = 0
    open("current_result/start_tour.txt", "w")
    meth_alg = ['SA', 'LKH', 'OptAlg', 'Gurobi']
    z_b = ['y', 'n']
    Z_b = ['YES', 'NO']
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
                sa = CVRPTW('SA', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).sa() #TODO: некоторые параметры брать автоматически из файла
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
                opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
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
                lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
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
                infile = open('Gurobi_20.txt',  'r')
                outfile = open('Gurobi_20_CVRPTW_dist.txt',  'w')

                pst = ['-', 'best', 'continuous,', 'Mon','Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun','second', '']
                copy = False
                k = n
                for line in infile:
                    if 'Expl Unexpl |  Obj  Depth IntInf | Incumbent    BestBd   Gap | It/Node Time' in line:
                        copy = True
                    elif 'Cutting planes:' in line:
                        copy = False
                    elif copy:
                        content = line.split()
                        new_line = ''
                        if len(content) == 10:
                            if((content[5] not in pst) and (prev_content not in pst) and 'e+' not in content[5] and 'e+' not in prev_content):
                                if(float(content[5]) - float(prev_content) > 0.1):
                                    outfile.write('')
                                    
                            if(content[5] not in pst and (prev_content not in pst) and 'e+' not in content[5] and 'e+' not in prev_content):
                                new_line += content[5]
                                prev_content = content[5]
                        elif len(content) == 8:
                            if((content[3] not in pst) and (prev_content not in pst) and 'e+' not in content[3] and 'e+' not in prev_content):
                                if(float(content[3]) - float(prev_content) > 0.1):
                                    outfile.write('')

                            if(content[3] not in pst and (prev_content not in pst) and ('e+' not in content[3]) and ('e+' not in prev_content)):
                                new_line += content[3]
                                prev_content = content[3]
                        if len(content) != 0 and content[-1] != 'processors)' and '.' not in content[-1] and content[-1].isdigit() == False:
                            new_line += '\t' + content[-1] 
                            new_line = new_line[:-1] + '\n'
                            if(new_line[0] != '\t' and new_line[0] != ''):
                                outfile.write(new_line)
                            if(k == 20):
                                if(content[-1][:-1] == '300'):
                                    outfile.write('\n')
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
        cost_batched = np.array(cost_batched)
        plt.plot(cost_batched.mean(0))
        
    plt.legend(lst_legend)
    plt.xlabel('Время оптимизации (секунды)', fontsize=7)
    plt.ylabel('Средняя стоимость (секунды)', fontsize=7)
    plt.title(f'{count_towns-1} points', fontsize=16)

    plt.savefig('graphs/All_Algorithms.png')

    # import pandas as pd
    # import matplotlib.pyplot as plt
    # tour = lkh[1]

    # data = pd.read_csv('20/Example1.csv', sep='\t')
    # xcoord = list(data['x_coord'])
    # ycoord = list(data['y_coord'])

    # plt.figure(figsize=(12,8))
    # plt.scatter(xcoord, ycoord, color="red")

    # plt.scatter(xcoord[0], ycoord[0], color="black", marker='D')
    # plt.annotate("Depo", (xcoord[0]-1, ycoord[0]-5.5))

    # N = count_towns - 1
    # clients = [i for i in range(N+1) if i != 0]
    # for i in clients:
    #     plt.annotate('${%d}$' % i, (xcoord[i]-1, ycoord[i]-5))

    # colors = ["black", "blue", "red", "orange", "yellow", "grey"]
    # i = 0
    # for r in tour:
    # #     print(r)
    #     for t in range(len(r)-1):
    # #         print(r[t])
    #         plt.plot([xcoord[r[t]], xcoord[r[t+1]]], [ycoord[r[t]], ycoord[r[t+1]]], color=colors[i],alpha=0.4)
    #     plt.plot([xcoord[r[len(r)-1]], xcoord[r[0]]], [ycoord[r[len(r)-1]], ycoord[r[0]]],color=colors[i],alpha=0.4)
    #     i += 1
    
    # plt.savefig('LKH_TOUR.png')

            
if __name__ == "__main__":
    main()


#   twtown temp[newCountTowns+1];\
#    twtown** full_temp = calloc(newCountTowns+1, sizeof(twtown*));\
#    int* len_full_temp = calloc(newCountTowns+1, sizeof(int));\
#    int n_temp = 0;\
#    double td;\
#    double distanceInTourBest = -1.0, distanceInTourNew = 0.0;\
#    double runtime = clock();\
#    int days, cap, l, g;\
#    double full_time = 0;\
#    signal(SIGINT, sigfunc);\
#    /*doShuffleTw(newCountTowns, sub);*/\
#    l = 1;\
#    g = 0;\
#    cap = 0;\
#    for(g = 0; g < newCountTowns; g++) { \
#       if(cap + sub[g].t.weight <= maxCapacity && g != newCountTowns - 1) {\
#          temp[l] = sub[g];\
#          l++;\
#          cap += sub[g].t.weight;\
#       } else {\
#          if(g == newCountTowns - 1){\
#             temp[l] = sub[g];\
#             l++;\
#          }\
#          temp[0] = town0;\
#          td = subtourdistanceTw(temp, l, &m, timer, endTime);\
#          while(td == -1) {\
#             timer = town0.mTimeStart;\
#             td = subtourdistanceTw(temp, l, &m, timer, endTime);\
#             if(td == -1) {l--; g--;}\
#          }\
#          full_temp[n_temp] = calloc(l, sizeof(twtown));\
#          for(int t = 0; t < l; ++t){\
#             full_temp[n_temp][t] = temp[t];\
#          }\
#          len_full_temp[n_temp] = l;\
#          write_cvrptw_subtour(res_distance, temp, l); \
#          distanceInTourNew += td;\
#          n_temp += 1;\
#          l = 1;cap = 0;\
#       }\
#    }\
#    printf("\nSTART_LEN: %lf\n[", distanceInTourNew);\
#    for(int num = 0; num < n_temp; ++num){\
#       printf("[");\
#       for(int t = 0; t < len_full_temp[num]; ++t){\
#          printf("%d ", full_temp[num][t].t.name);\
#       }\
#       printf("]\n");\
#       printf("len_full_temp: %d", len_full_temp[num]);\
#    }\
#    printf("]\n");\
#    fprintf(out, "%lf\t%lf\n", (distanceInTourNew), 0.0);\
#    distanceInTourNew = 0;\
#    while(!stop){\
#       clock_t start = clock();\
#       /*printf("countTaks: %d\n", i);\*/\
#       /*doShuffleTw(newCountTowns, sub);*/\
#       for(int num = 0; num < n_temp; ++num){\
#          if(len_full_temp[num] >= 3) {\
#             td = algfunc(full_temp[num], len_full_temp[num], &m, &timer, endTime);  \
#          } else {\
#             td = subtourdistanceTw(full_temp[num], len_full_temp[num], &m, timer, endTime);\
#          }\
#          while(td == -1){\
#             td = algfunc(full_temp[num], len_full_temp[num], &m, &timer, endTime);\
#          }\
#          write_cvrptw_subtour(res_distance, full_temp[num], len_full_temp[num]); \
#          distanceInTourNew += td;\
#       }\
#       if(distanceInTourBest == -1.0) {\
#          /*distanceInTourNew * 60 * 1000 / 3600*/\
#          fprintf(out, "%lf\t%lf\n", (distanceInTourNew), (clock() - runtime) / CLOCKS_PER_SEC);\
#          distanceInTourBest = distanceInTourNew;\
#       } \
#       if(distanceInTourNew < distanceInTourBest) {\
#          distanceInTourBest = distanceInTourNew;\
#          write_cvrptw_end_tour(res_distance, distanceInTourBest);\
#          /*distanceInTourBest * 60 * 1000 / 3600*/\
#          fprintf(out, "%lf\t%lf\n", (distanceInTourBest), (clock() - runtime) / CLOCKS_PER_SEC);\
#       }\
#       else {\
#          write_cvrptw_end_tour(res_distance, -1);\
#       }\
#       distanceInTourNew = 0.0;\
#       clock_t end = clock();\
#       double seconds = (double)(end - start) / CLOCKS_PER_SEC;\
#       full_time += seconds;\
#       if(!stop)\
#          printf("Время оптимизации: %lf Текущая длина: %lf \n", full_time, distanceInTourBest);\