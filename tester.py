import pandas as pd
import numpy as np
from gurobi_functions import *
m = [735162.6, [[81, 1, 12, 47, 32, 15, 8, 68, 55, 53, 98, 66, 70, 92, 100, 18, 27, 60, 76, 77, 33, 42, 59], 
[43, 86, 61, 56, 74, 94, 67, 26, 72, 73, 28, 10, 23, 7, 29, 41, 90, 63, 14, 49, 30, 71], 
[58, 31, 11, 95, 21, 17, 34, 84, 79, 19, 83, 9, 36, 25, 89, 22, 82, 65, 91, 38, 39, 45, 87, 64, 50], 
[24, 78, 69, 54, 51, 3, 44, 40, 13, 75, 80, 6, 4, 35, 62, 46, 48, 37, 99, 85, 52, 57], [93, 20, 2, 16, 5, 97, 88, 96]]]
counttowns = 100
distance, tour = m
for i in range(len(tour)):
    for j in range(len(tour[i])):
        tour[i][j] = counttowns - tour[i][j]
name_file = "100/20201020_182616.csv"
data_file    = pd.read_csv(name_file, sep="\t", error_bad_lines=True)
# distance_file(51, "50/20201007_185322.csv", "50/20201007_185322_dist.csv")
arr_data     = data_file.values
xc = np.zeros(len(arr_data))
yc = np.zeros(len(arr_data))
capacity = np.zeros(len(arr_data))
w = 0
Q_max = 1000
for i in range(len(arr_data)):
    if(arr_data[i][2] <= Q_max):
        xc[w]       = arr_data[i][0]
        yc[w]       = arr_data[i][1]
        capacity[w] = arr_data[i][2]
        w += 1

# Убираем координаты и вес депо
depo_xc = xc[w-1]
depo_yc = yc[w-1]
xc = xc[:w-1]
yc = yc[:w-1]
capacity = capacity[:w-1]

# Исключаем депо из списка клиентов
w = w - 1
# Создаём список клиентов
clients  = []
for i in range(0, w):
    clients.append(i)
nodes   = [len(arr_data) - 1] + clients
        # Создаём список всевохможных пар между клиентами, включая депо
paires  = [(i, j) for i in nodes for j in nodes if i != j]
distance  = {}
arr_dist  = pd.read_csv("100/20201020_182616_dist.csv", sep=",", error_bad_lines=False)
for i,j in paires:    
    if(i == len(arr_data) - 1 and j <= max(paires)[1]):
        for k in range(len(arr_dist)):
            if(depo_xc == arr_dist.values[k][0] and depo_yc == arr_dist.values[k][1] and xc[j] == arr_dist.values[k][2] and yc[j] == arr_dist.values[k][3]):
                distance[(i,j)] = arr_dist.values[k][4]
                break

    elif(i <= max(paires)[1] and j == len(arr_data) - 1):
        for k in range(len(arr_dist)):
            if(xc[i] == arr_dist.values[k][0] and yc[i] == arr_dist.values[k][1] and depo_xc == arr_dist.values[k][2] and depo_yc == arr_dist.values[k][3]):
                distance[(i,j)] = arr_dist.values[k][4]
                break

    else:
        for k in range(len(arr_dist)):
            if(xc[i] == arr_dist.values[k][0] and yc[i] == arr_dist.values[k][1] and xc[j] == arr_dist.values[k][2] and yc[j] == arr_dist.values[k][3]):
                distance[(i,j)] = arr_dist.values[k][4]
                break

print(distance[(0,50)])