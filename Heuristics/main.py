from twMethods import *
from collections import deque
from datetime import datetime
import matplotlib.pyplot as plt

def main():
    print("Введите количество городов(20, 50, 100): ")
    n = input()
    lst = []
    for i in range(1, 2):
        lst.append(f"{n}/Example{i}.csv")
    lst_results = []
    print('If you want open documentation write YES else press Enter')
    doc = input()
    if(doc == 'YES'):
        help(vrp_c) #TODO: в отредактировать текст в документации
    idx = 0
    print('which method do you want to use to optimize the routse for CVRPTW: SA or LKH or OptAlg or Gurobi?')
    method = input()
    while(1):
        for i in lst:
            with open(i) as f:
                count_towns = sum(1 for _ in f) - 1
            with open(i) as f:
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
            
            if(method == 'SA'):
                start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
                end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
                a = CVRPTW('SA', i, f"{count_towns-1}_tw/test{idx}", count_towns, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                print(a.sa())
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
                a = CVRPTW('OptAlg', i, f"{count_towns-1}_tw/test{idx}", count_towns, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                print(a.opt(name_opt))
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
                a = CVRPTW('LKH', i, f"{count_towns-1}_tw/test{idx}", count_towns, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
                idx += 1
                print(a.lkh())
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
                a = CVRPTW('Gurobi', i, f"{count_towns-1}_tw/test{idx}", count_towns, max_capacity, count_vehicles).gurobi() #TODO: некоторые параметры брать автоматически из файла
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

        z = input()
        if(z == ''):
            print('Do you want to use a different algorithm for optimize these instances(y/n):')
            z = input()
        while(z != 'y' or z != 'n'):
            if(z == 'n'):
                break
            elif(z == 'y'):
                print('which method do you want to use to optimize the routse for CVRPTW: SA or LKH or OptAlg or Gurobi?')
                method = input()
                break
            else:
                print('Try again!')
                print('Do you want to use a different algorithm for optimize these instances(y/n):')
                z = input()
        if(z == 'n'):
            break
    
    # Отсортировываем название файлов с результатами в порядке, как названия алгоритмов находятся в legend
    final_lst_results = []
    for i in lst_results:
        letter = i.split('/')[1][:1]
        if(letter == 'S')  : final_lst_results.insert(0, i)
        elif(letter == '2'): final_lst_results.insert(1, i)
        elif(letter == '3'): final_lst_results.insert(2, i)
        elif(letter == 'L'): final_lst_results.insert(3, i)
        elif(letter == 'G'): final_lst_results.insert(4, i)

    print(final_lst_results)
    for name in final_lst_results:
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
        
    plt.legend(['SA', '2Opt', '3Opt', 'LKH', 'Gurobi'])
    plt.xlabel('optimization time', fontsize=16)
    plt.ylabel('mean cost', fontsize=16)
    plt.title('20 points', fontsize=16)

    plt.savefig('All_Algorithms.png')
            
if __name__ == "__main__":
    main()