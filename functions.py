import vrp_c
import gurobi_functions as gur_f
from gurobipy import Model, GRB, quicksum
from datetime import datetime
import os
import numpy as np
import pandas as pd

class VRP:
    """
    Класс, в котором объявлены основные параметры, использующиеся в классах CVRP, CVRPTW:
        :type string name_file:   Название файла с данными;
        :type string path_folder: Название папки, в которой буду сохранены бинарные файлы с матрицами расстояний;
        :type int count_towns: Количество городов;
        :type int countTasks:  Количество итераций для решения одной оптимизационной задачи.
    """ 
    def __init__ (self, name_file, path_folder, count_towns, countTasks: int = 10000):
        self.name_file   = name_file 
        self.path_folder = path_folder 
        self.count_towns = count_towns
        self.count_Tasks = countTasks
        self.parse_file()


def parse_dist_and_tour():
    """
    Функция, возвращающая список из 2 элементов:
        :type float max_count: Длина (в метрах) оптимизированного маршрута;
        :type list mas_dist:  Список подмаршрута оптимизированного маршрута.
    """
    max_count = -1
    mas_dist = []
    with open('res_distance.txt', 'r') as read_file:
        data = read_file.read().split('@')[:-1]
        for i in range(len(data)-1, 0, -2):
            if(float(data[i]) != -1):
                max_count = float(data[i])
                mas_dist = list(map(lambda t: list(map(int, t.split(' '))), data[i-1][:-1].split('#')))
                break 
    return [max_count, mas_dist]    
        
class CVRPTW (VRP):
    """
    Это основной класс для решения задачи CVRPTW
    """
    def __init__ (self,  name_file, path_folder, count_towns, countTasks: int = 10000, capacity: int = 30, time_start: int = 0, time_end: int = 0):
        super().__init__(name_file, path_folder, count_towns, countTasks)
        self.capacity = capacity
        self.countTasks = countTasks
        self.time_start = time_start
        self.time_end   = time_end
    
    #TODO: ограничительная функция на все параметры
    
    def parse_file(self):
        """
        Функция, создающая матрицы расстояний, которые записываются в бинарные файлы для задачи CVRPTW. На вход подаются следующие параметры:
            :type string name_file:   Имя файла с данными;
            :type string path_folder: Название папки, в которой буду сохранены бинарные файлы с матрицами расстояний;
            :type int count_towns: Количество городов.
        """
        # для SA и LKH моей реализации
        print("Parse from CVRPTW")
        # gur_f.distance_file(self.count_towns, [self.name_file], [self.name_file[ :self.name_file.rfind('.csv')] + '_dist.csv']) #TODO: сделать только для Gurobi
        
        vrp_c.parseOneTwTownPy(self.name_file, self.path_folder, self.count_towns)

    def sa(self, T: float = 1000, t_min: float = 10) -> [float, list]:
        """
        Функция, вызывающая алгоритм "Имитации отжига" для решения задачи CVRPTW. На вход подается два параметра:
            :type float T:     начальная температура, которая с течением времени убывает;
            :type float t_min: конечная температура, до которой опускается температура T.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """ 
        vrp_c.modelMetaHeuristic("cvrptw_sa", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        return parse_dist_and_tour()
    
    def lkh(self, name_opt: str = 'lkh3opt') -> [float, list]:
        """
        Функция, вызывающая алгоритм "Эвристика Лина-Кёрнигана" для решения задачи CVRPTW. На вход подается один параметр.
            :type string name_opt: название метода оптимизации: 2-opt, 3-opt.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """
        if(name_opt == 'lkh2opt'):
            vrp_c.modelMetaHeuristic("cvrptw_lkh_2opt", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        else:
            vrp_c.modelMetaHeuristic("cvrptw_lkh_3opt", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        return parse_dist_and_tour()
    
    def gurobi(self) -> [float, list]:
        """
        Функция, использующая коммерческий проект "Gurobi" для решения задачи CVRPTW.
        
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """ 
        data_file    = pd.read_csv(self.name_file, sep="\t", error_bad_lines=True)

        # Запишем все данные в виде массива списков, где каждый список это строка с значениями конкретных столбцов
        arr_data     = data_file.values

        # Задаём объем грузовика
        Q_max = self.capacity # Для 20:500, 50:750, 100:1000

        # Создаём списки координат и грузов для каждого клиента
        xc = np.zeros(len(arr_data))
        yc = np.zeros(len(arr_data))
        capacity = np.zeros(len(arr_data))
        w = 0
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
        # Cоздаем список клиентов вместе с депо. Депо обозначено, как последний элемент в arr_data
        nodes   = [len(arr_data) - 1] + clients
        # Создаём список всевохможных пар между клиентами, включая депо
        paires  = [(i, j) for i in nodes for j in nodes if i != j]

        # Создаём словарь весов
        q                    = {i: capacity[i] for i in clients}
        q[len(arr_data) - 1] = 0 # добавляем вес в депо в конец словаря
        print(int(arr_data[0][3].split('-')[0].split(':')[1]))
        # Задаём словари временных ограничений на посещение

        # Минимальное время прибытия + перевод в минуты
        start     = {i: (int(arr_data[i][3].split('-')[0].split(':')[0]) + int(arr_data[0][3].split('-')[0].split(':')[1]) )* 60 for i in clients}
        start_lst = {len(arr_data) - 1: (int(arr_data[len(arr_data) - 1][3].split('-')[0].split(':')[0]) + int(arr_data[len(arr_data) - 1][3].split('-')[0].split(':')[1]))* 60}
        start_lst.update(start)
        
        # Максимальное время прибытия + перевод в минуты
        finish     = {i: (int(arr_data[i][3].split('-')[1].split(':')[0]) + int(arr_data[i][3].split('-')[1].split(':')[1])) * 60 for i in clients}
        finish_lst = {len(arr_data) - 1: (int(arr_data[len(arr_data) - 1][3].split('-')[1].split(':')[0]) + int(arr_data[len(arr_data) - 1][3].split('-')[1].split(':')[1]))* 60}
        finish_lst.update(finish)

        # Задаём словарь времени обслуживания каждого клиента (в минутах)
        service                    = {n: int(arr_data[n][4]) for n in clients}
        # Добавляем в словарь время обслуживания в депо
        service[len(arr_data) - 1] = 0

        # Задаем скорость грузовика 60 км/ч = 1000 м/мин
        v = 1000

        # Задаем словарь всевозможных расстояний между всеми городами, включая депо
        distance  = {}
        arr_dist  = pd.read_csv(self.name_file[ :self.name_file.rfind('.csv')] + '_dist.csv', sep=",", error_bad_lines=False)
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
        
        #Список транспортных средств
        vehicles = [i for i in range(1, 3)]

        #Словарь вместимости транспортных средств
        Q        = {i: Q_max for i in range(1, 3)}

        # Задаем словарь времени, требуемое для перемещение из одного города в другой
        # Время выражено в МИНУТАХ
        time = {}
        for i,j in paires:
            time[(i, j)] = float(distance[i, j] / v)

        arco_var  = [(i, j, k) for i in nodes for j in nodes for k in vehicles if i != j]

        arco_time = [(i, k) for i in nodes for k in vehicles]
        
        # Модель
        model = Model('VRPTW')

        # Переменные для решения
        x = model.addVars(arco_var, vtype=GRB.BINARY, name = 'x')
        t = model.addVars(arco_time, vtype=GRB.CONTINUOUS, name = 't')

        #Целевая функция
        model.setObjective(quicksum(distance[i, j]*x[i,j,k] for i, j, k in arco_var), GRB.MINIMIZE)

        #Ограничения

        # Прибытие и отъезд
        model.addConstrs(quicksum(x[len(arr_data) - 1, j, k] for j in clients) <= 1 for k in vehicles)
        model.addConstrs(quicksum(x[i, len(arr_data) - 1, k] for i in clients) <= 1 for k in vehicles)

        # Ограничение по посещению маршрута ровно один раз и ровно одним грузовиком
        model.addConstrs(quicksum(x[i, j, k] for j in nodes for k in vehicles if i != j) == 1 for i in clients)

        #
        model.addConstrs(quicksum(x[i, j, k] for j in nodes if i != j) - quicksum(x[j, i, k] for j in nodes if i != j) == 0 for i in nodes for k in vehicles)

        #Ограничение на вместимость грузовика
        model.addConstrs(quicksum(q[i] * quicksum(x[i, j, k] for j in nodes if i != j) for i in clients) <= Q[k] for k in vehicles)

        #Ограничение по времени
        model.addConstrs((x[i,j,k] == 1) >> (t[i, k] + service[i] + time[i, j] == t[j, k]) for i in clients for j in clients for k in vehicles if i != j)
        model.addConstrs(t[i, k] >= start_lst[i] for i, k in arco_time)
        model.addConstrs(t[i, k] <= finish_lst[i] for i, k in arco_time)

        # Optimizing the model
        model.Params.TimeLimit = 100  # seconds
        model.Params.LogFile= f'result_Branch_and_Cut_CVRPTW_{w}.txt'
        model.optimize()
        if model.status == GRB.OPTIMAL:
            print('1.Optimal objective: %g' % model.objVal)
            print('Optimal cost: %g' % model.objVal)
        elif model.status == GRB.INFEASIBLE:
            print('3.Model is infeasible')
        elif model.status == GRB.UNBOUNDED:
            print('4.Model is unbounded')
        else:
            print('5.Optimization ended with status %d' % model.status)
            print('Optimal cost: %g' % model.objVal)

