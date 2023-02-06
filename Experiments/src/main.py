from itertools import count

from twMethods import *
from supportFunctions.paintGraphs import *
from collections import deque
from datetime import datetime
import os

def main():
    print("Введите количество городов(50, 100): ")
    n = input()
    while(n != '20' and n != '50' and n != '100'):
        print("Введите количество городов(50, 100): ")
        n = input()
    lst = []
    for i in range(2, 3): #100
    # for i in [8, 4, 87, 45, 65, 29, 57, 34, 82, 20]:
        lst.append(f"VRP_{n}/Example{i}.csv")
    lst_results = []
   
    idx = 0
    i = 0
    open("current_result/start_tour.txt", "w")

    while(i < len(lst)):
        with open(lst[i]) as f:
            count_towns = sum(1 for _ in f) - 1
        with open(lst[i]) as f:
            [last_line] = deque(f, maxlen=1)
            last_line = last_line.split('\t')[3].split('-')
        if(count_towns == 21):
            max_capacity = 500
        elif(count_towns == 51 or count_towns == 11):
            max_capacity = 750
        elif(count_towns == 101):
            max_capacity = 1000
        
        folder_name = f"{count_towns-1}_tw"
        if(os.path.exists(folder_name) == False):
            os.mkdir(folder_name)

        shuffle_param = 0
        # start = int(last_line[0]) #(int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        # end   = int(last_line[1]) #(int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        # print(f'SA_{idx}')
        # sa = CVRPTW('SA', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).sa() #TODO: некоторые параметры брать автоматически из файла
        # with open(f'current_result/SA_CVRPTW_result.txt', 'r') as res_file:
        #     data = res_file.read()
        #     with open(f'results/SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
        #         write_file.write(data)

        # if(f'results/SA_RES_CVRPTW{count_towns-1}.txt' in lst_results):
        #     lst_results.remove(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
        #     lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')
        # else:
        #     lst_results.append(f'results/SA_RES_CVRPTW{count_towns-1}.txt')

        # shuffle_param = 1
        # name_opt = '2opt'

        # start = int(last_line[0]) #(int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        # end   = int(last_line[1]) #(int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        # print(f'2Opt_{idx}')
        # opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
        # if(name_opt == '2opt' or name_opt == '3opt'):
        #     with open(f'current_result/{name_opt}_CVRPTW_result.txt', 'r') as res_file:
        #         data = res_file.read()
        #         with open(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
        #             write_file.write(data)

        # if(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt' in lst_results):
        #     lst_results.remove(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        #     lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        # else:
        #     lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        
        # shuffle_param = 1
        # name_opt = '3opt'

        # start = int(last_line[0]) #(int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
        # end   = int(last_line[1]) #(int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
        # print(f'3Opt_{idx}')
        # opt_alg = CVRPTW('OptAlg', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).opt(name_opt) #TODO: некоторые параметры брать автоматически из файла
        # if(name_opt == '2opt' or name_opt == '3opt'):
        #     with open(f'current_result/{name_opt}_CVRPTW_result.txt', 'r') as res_file:
        #         data = res_file.read()
        #         with open(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
        #             write_file.write(data)

        # if(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt' in lst_results):
        #     lst_results.remove(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        #     lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')
        # else:
        #     lst_results.append(f'results/{name_opt}_RES_CVRPTW{count_towns-1}.txt')

        # shuffle_param = 1
        start = int(last_line[0]) 
        end   = int(last_line[1])
        print(f'LKH_{idx}')
        lkh = CVRPTW('LKH', lst[i], f"{folder_name}/test{idx}", count_towns, shuffle_param, max_capacity, start, end).lkh() #TODO: некоторые параметры брать автоматически из файла
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
    paintGraphs(lst_results, count_towns)
            
if __name__ == "__main__":
    main()