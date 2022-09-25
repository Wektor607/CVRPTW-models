from twMethods import *
from collections import deque
from datetime import datetime

def main():
    print("Введите количество городов(20, 50, 100): ")
    n = input()
    lst = []
    for i in range(1, 2):
        lst.append(f"{n}/Example{i}.csv")
    
    print('If you want open documentation write YES else press Enter')
    doc = input()
    if(doc == 'YES'):
        help(vrp_c) #TODO: в отредактировать текст в документации

    print('which method do you want to use to optimize the routse for CVRPTW: SA or LKH or Gurobi?')
    method = input()
    idx = 0
    if(method == 'SA'):
        for i in lst:
            with open(i) as f:
                count_towns = sum(1 for _ in f) - 1
            with open(i) as f:
                [last_line] = deque(f, maxlen=1)
                last_line = last_line.split('\t')[3].split('-')
            if(count_towns == 21):
                max_capacity = 500
                iteretions   = 700
            elif(count_towns == 51):
                max_capacity = 750
                iteretions   = 500
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions   = 300
            start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
            end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
            a = CVRPTW('SA', i, f"{count_towns-1}_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.sa())
            with open(f'SA_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                    write_file.write(data)
    elif(method == 'LKH'):
        while(1):
            print("Input lkh2opt or lkh3opt:")
            name_opt = input()
            if(name_opt == 'lkh2opt' or name_opt == 'lkh3opt' or name_opt == 'lkh'):
                break
            else:
                print('Try again!')
        
        for i in lst:
            with open(i) as f:
                count_towns = sum(1 for _ in f) - 1
            with open(i) as f:
                [last_line] = deque(f, maxlen=1)
                last_line = last_line.split('\t')[3].split('-')
            if(count_towns == 21):
                max_capacity = 500
                iteretions   = 700
            elif(count_towns == 51):
                max_capacity = 750
                iteretions   = 500
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions   = 300
            start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
            end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
            a = CVRPTW('LKH', i, f"{count_towns-1}_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.lkh(name_opt))
            if(name_opt == 'lkh2opt' or name_opt == 'lkh3opt'):
                with open(f'LKH_{name_opt[3:]}_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'LKH_{name_opt[3:]}_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
            else:
                with open(f'LKH_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'LKH_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                        write_file.write(data)
    else:
        for i in lst:
            with open(i) as f:
                count_towns = sum(1 for _ in f) - 1
            if(count_towns == 21):
                max_capacity = 500
                iteretions   = 700
                count_vehicles = 11
            elif(count_towns == 51):
                max_capacity = 750
                iteretions   = 500
                count_vehicles = 11
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions   = 300
                count_vehicles = 21
            a = CVRPTW('Gurobi', i, f"{count_towns-1}_tw/test{idx}", count_towns, iteretions, max_capacity, count_vehicles).gurobi() #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            
if __name__ == "__main__":
    main()