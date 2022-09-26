from twMethods import *
from collections import deque
from datetime import datetime
import tools
import numpy as np
import pandas as pd
def translate_minute(sec):
    minute = sec // 60
    hour = minute // 60 
    minute %= 60 
    return "%02d:%02d" % (hour, minute)

def main():

    lst = ["instances/ORTEC-VRPTW-ASYM-00c5356f-d1-n258-k12.txt",
    ]
    for i in range(1, 2):
        lst.append(f1)
    
    data = tools.read_vrplib(lst[0])
    with open('parse_instance/example1.csv','w') as fout:
        fout.write('x_coord' + '\t' + 'y_coord' + '\t' + 'Weight' + '\t' + 'DeliveryTimeRange' + '\t' + 'UnloadingTime' + '\n')
        for i in range(1,len(data['coords'])):
            fout.write(np.array2string(data['coords'][i][0]) + '\t' + np.array2string(data['coords'][i][1])  + '\t' + np.array2string(data['demands'][i])  + '\t' + translate_minute(data['time_windows'][i][0]) + '-' + translate_minute(data['time_windows'][i][1]) + '\t' + np.array2string(data['service_times'][i]) + '\n')
        fout.write(np.array2string(data['coords'][0][0]) + '\t' + np.array2string(data['coords'][0][1])  + '\t' + np.array2string(data['demands'][0])  + '\t' + translate_minute(data['time_windows'][0][0]) + '-' + translate_minute(data['time_windows'][0][1]) + '\t' + np.array2string(data['service_times'][0]) + '\n')
    lst1 = []    
    lst1.append("example1.csv")
    # print('If you want open documentation write YES else press Enter')
    # doc = input()
    # if(doc == 'YES'):
    #     help(vrp_c) #TODO: в отредактировать текст в документации

    print('which method do you want to use to optimize the routse for CVRPTW: SA or opt?')
    method = input()
    idx = 0
    if(method == 'SA'):
        for nf in lst1:
            count_towns = len(data['is_depot'])
            max_capacity = data['capacity']
            iteretions   = 500
            start = data['time_windows'][0][0]
            end = data['time_windows'][0][1]
            kr = nf.split('.')[0]
            d = pd.DataFrame(data['duration_matrix'])
            d.to_csv(f'time_matrix_{kr}.csv',index = False,sep = '\t', columns = None)
            mnf = f'time_matrix_{kr}.csv'
            a = CVRPTW('SA', nf, mnf,  f"comp_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.sa())
            with open(f'SA_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'SA_RES_CVRPTW{count_towns-1}.txt', 'a') as write_file:
                    write_file.write(data)
    elif(method == 'LKH'):
        while(1):
            print("Input 2opt or 3opt:")
            name_opt = input()
            if(name_opt == '2opt' or name_opt == '3opt' or name_opt == 'lkh'):
                break
            else:
                print('Try again!')
        
        for nf in lst1:
            count_towns = len(data['is_depot'])
            max_capacity = data['capacity']
            iteretions   = 500
            start = data['time_windows'][0][0]
            end = data['time_windows'][0][1]
            kr = nf.split('.')[0]
            d = pd.DataFrame(data['duration_matrix'])
            d.to_csv(f'time_matrix_{kr}.csv',index = False,sep = '\t', columns = None)
            mnf = f'time_matrix_{kr}.csv'
            a = CVRPTW('LKH', nf, mnf,  f"comp_tw/test{idx}", count_towns, iteretions, max_capacity, start, end)#TODO: некоторые параметры брать автоматически из файла
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
    # else:
    #     for i in lst:
    #         with open(i) as f:
    #             count_towns = sum(1 for _ in f) - 1
    #         if(count_towns == 21):
    #             max_capacity = 500
    #             iteretions   = 700
    #             count_vehicles = 11
    #         elif(count_towns == 51):
    #             max_capacity = 750
    #             iteretions   = 500
    #             count_vehicles = 11
    #         elif(count_towns == 101):
    #             max_capacity = 1000
    #             iteretions   = 300
    #             count_vehicles = 21
    #         a = CVRPTW('Gurobi', i, f"{count_towns-1}_tw/test{idx}", count_towns, iteretions, max_capacity, count_vehicles).gurobi() #TODO: некоторые параметры брать автоматически из файла
    #         idx += 1
            
if __name__ == "__main__":
    main()