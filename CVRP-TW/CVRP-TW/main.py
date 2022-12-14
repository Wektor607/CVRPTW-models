from twMethods import *
from collections import deque
import matplotlib.pyplot as plt
import os

def main():
    if(os.path.exists('CVRP-TW/current_result') == False):
        os.mkdir('CVRP-TW/current_result')
    if(os.path.exists('CVRP-TW/results') == False):
        os.mkdir('CVRP-TW/results')
    if(os.path.exists('CVRP-TW/graphs') == False):
        os.mkdir('CVRP-TW/graphs')

    print("Введите количество городов(20, 50, 100): ")
    n = input()
    while(n != '20' and n != '50' and n != '100'):
        print("Введите количество городов(20, 50, 100): ")
        n = input()
    lst = []
    for i in range(1, 4):
        lst.append(f"CVRP-TW/instances/{n}/Example{i}.csv")

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

    open("CVRP-TW/current_result/start_tour.txt", "w")
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
            
            folder_name = f"CVRP-TW/{count_towns-1}_tw"
            if(os.path.exists(folder_name) == False):
                os.mkdir(folder_name)

            if(method == 'SA'):
                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                sa = CVRPTW('SA', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).sa() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                with open(f'CVRP-TW/current_result/SA_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'CVRP-TW/results/SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
        
                if(f'CVRP-TW/results/SA_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                    lst_results.remove(f'CVRP-TW/results/SA_RES_CVRPTW{count_towns-1}.txt')
                    lst_results.append(f'CVRP-TW/results/SA_RES_CVRPTW{count_towns-1}.txt')
                else:
                    lst_results.append(f'CVRP-TW/results/SA_RES_CVRPTW{count_towns-1}.txt')

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
                    with open(f'CVRP-TW/current_result/{name_opt}_CVRPTW_result.txt', 'r') as res_file:
                        data = res_file.read()
                        with open(f'CVRP-TW/results/{name_opt}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                            write_file.write(data)

                if(f'CVRP-TW/results/{name_opt}_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                    lst_results.remove(f'CVRP-TW/results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
                    lst_results.append(f'CVRP-TW/results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
                else:
                    lst_results.append(f'CVRP-TW/results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')

            elif(method == 'LKH'):
                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                with open(f'CVRP-TW/current_result/LKH_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'CVRP-TW/results/LKH_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
                
                if(f'CVRP-TW/results/LKH_RES_CVRPTW{count_towns-1}.txt' in lst_results):
                    lst_results.remove(f'CVRP-TW/results/LKH_RES_CVRPTW{count_towns-1}.txt')
                    lst_results.append(f'CVRP-TW/results/LKH_RES_CVRPTW{count_towns-1}.txt')
                else:
                    lst_results.append(f'CVRP-TW/results/LKH_RES_CVRPTW{count_towns-1}.txt')

            elif(method == 'Gurobi'):
                gurobi = CVRPTW('Gurobi', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, count_vehicles).gurobi() #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                infile = open('CVRP-TW/results/Gurobi_20.txt',  'r')
                outfile = open('CVRP-TW/results/Gurobi_20_CVRPTW_dist.txt',  'w')

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

    plt.savefig('CVRP-TW/graphs/All_Algorithms.png')

            
if __name__ == "__main__":
    main()