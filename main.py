from functions import *
from collections import deque
from datetime import datetime

def main():
    lst = [
        # "20/20201025_141430.csv",
        "20/Example1.csv"
        # "20/20201025_142105.csv", 
        # "20/20201101_150113.csv", 
        # "20/20201115_132354.csv", 
        # "20/20201116_232344.csv", 
        # "20/20201224_222701.csv", 
        # "20/20201225_204810.csv", 

        # "20/20201228_230627.csv", 
        # "20/20200925_093755.csv", 
        # "20/20200927_100114.csv", 
        # "20/20200928_114457.csv", 
        # "20/20200930_163005.csv", 
        # "20/20201005_160200.csv", 
        # "20/20201008_165020.csv", 
        # "20/20201014_154416.csv" 
    ]
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
                iteretions = 700
            elif(count_towns == 51):
                max_capacity = 750
                iteretions = 500
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions = 300
            start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
            end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
            a = CVRPTW(i, f"20_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.sa())
            with open(f'SA_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open('SA_RES_CVRPTW.txt', 'a') as write_file:
                    write_file.write(data)
    elif(method == 'LKH'):
        while(1):
            print("Input lkh2opt or lkh3opt:")
            name_opt = input()
            if(name_opt == 'lkh2opt' or name_opt == 'lkh3opt'):
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
                iteretions = 700
            elif(count_towns == 51):
                max_capacity = 750
                iteretions = 500
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions = 300
            start = (int(last_line[0].split(':')[0]) + int(last_line[0].split(':')[1])) * 60
            end   = (int(last_line[1].split(':')[0]) + int(last_line[1].split(':')[1])) * 60
            a = CVRPTW(i, f"20_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.lkh(name_opt))
            with open(f'LKH_{name_opt[3:]}_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open('LKH_RES_CVRPTW.txt', 'a') as write_file:
                    write_file.write(data)
    else:
        for i in lst:
            with open(i) as f:
                count_towns = sum(1 for _ in f) - 1
            if(count_towns == 21):
                max_capacity = 500
                iteretions = 700
                count_vehicles = 10
            elif(count_towns == 51):
                max_capacity = 750
                iteretions = 500
                count_vehicles = 10
            elif(count_towns == 101):
                max_capacity = 1000
                iteretions = 300
                count_vehicles = 20
            a = CVRPTW(i, f"20_tw/test{idx}", count_towns, iteretions, max_capacity, count_vehicles) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.gurobi())
        p = count_towns - 1
        infile = open(f'result_Branch_and_Cut_CVRPTW_{p}.txt',  'r')
        outfile = open(f'result_Gurobi_{p}_CVRPTW.txt',  'w')

        copy = False
        prev_content = '10000000000000000000000000000000000000000000'
        for line in infile:
            if 'Expl Unexpl |  Obj  Depth IntInf | Incumbent    BestBd   Gap | It/Node Time' in line:
                copy = True
            elif 'Cutting planes:' in line:
                copy = False
            elif copy:
                content = line.split()
                new_line = ''
                if len(content) == 10:
                    if(float(content[5]) > float(prev_content)):
                        outfile.write('\n')
                    new_line += content[5]
                    prev_content = content[5]
                elif len(content) == 8:
                    if(float(content[3]) > float(prev_content)):
                        outfile.write('\n')
                    new_line += content[3]
                    prev_content = content[3]

                if len(content) != 0:
                    new_line += '\t' + content[-1] 
                    new_line = new_line[:-1] + '\n'
                    if(new_line[0] != '\t'):
                        outfile.write(new_line)
        infile.close()
        os.remove(f'result_Branch_and_Cut_CVRPTW_{p}.txt')
        outfile.close()
            
if __name__ == "__main__":
    main()