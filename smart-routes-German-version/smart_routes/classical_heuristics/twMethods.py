import vrp_c
import classical_heuristics.supportFunctions.gurobiFunctions as gur_f
from gurobipy import Model, GRB, quicksum
from datetime import datetime
import os
import numpy as np
import pandas as pd
import math

class VRP:
    """
    Класс, в котором объявлены основные параметры, использующиеся в классах CVRP, CVRPTW:
        :type string name_file:   Название файла с данными;
        :type string path_folder: Название папки, в которой буду сохранены бинарные файлы с матрицами расстояний;
        :type int count_towns: Количество городов;
        :type int countTasks:  Количество итераций для решения одной оптимизационной задачи.
    """ 
    def __init__ (self, alg_name, name_file, path_folder, count_towns, shuffle_param):
        self.alg_name    = alg_name
        self.name_file   = name_file 
        self.path_folder = path_folder 
        self.count_towns = count_towns
        self.shuffle_param = shuffle_param
        self.parse_file()


def parse_dist_and_tour(file_name, max_capacity, count_vehicles):
    """
    Функция, возвращающая список из 2 элементов:
        :type float max_count: Длина (в метрах) оптимизированного маршрута;
        :type list mas_dist:  Список подмаршрута оптимизированного маршрута.
    """
    max_count = -1
    mas_dist = []
    with open('classical_heuristics/current_result/res_distance.txt', 'r') as read_file:
        data = read_file.read().split('@')[:-1]
        for i in range(len(data)-1, 0, -2):
            if(float(data[i]) != -1):
                max_count = float(data[i])
                mas_dist = list(map(lambda t: list(map(int, t.split(' '))), data[i-1][:-1].split('#')))
                break
    
    print('Итоговый маршрут: ', mas_dist)
    return [max_count, mas_dist]    
        
class CVRPTW (VRP):
    """
    Это основной класс для решения задачи CVRPTW
    """
    def __init__ (self, alg_name, name_file, path_folder, count_towns, shuffle_param, capacity: int = 30, time_start: int = 0, time_end: int = 0, count_vehicles: int = 10):
        super().__init__(alg_name, name_file, path_folder, count_towns, shuffle_param)
        self.capacity = capacity
        self.time_start = time_start
        self.time_end   = time_end
        self.count_vehicles = count_vehicles # 20, 50: 10, 100, 200: 20
    
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
        if(self.alg_name != 'Gurobi'):
            vrp_c.parseOneTwTownPy(self.name_file, self.path_folder, self.count_towns)

    def sa(self) -> [float, list]: #TODO: В SA передавать температуру с клавиатуры
        """
        Функция, вызывающая алгоритм "Имитации отжига" для решения задачи CVRPTW. На вход подается два параметра:
            :type float T:     начальная температура, которая с течением времени убывает;
            :type float t_min: конечная температура, до которой опускается температура T.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """ 
        # print('Введите начальную температуру: ')
        Tstart = 100000
        # print('Введите конечную температуру: ')
        tmin = 1
        print('SHUFFLE Param', self.shuffle_param)
        vrp_c.modelMetaHeuristic("cvrptw_sa", self.path_folder, self.count_towns, self.capacity, Tstart, tmin, self.shuffle_param, self.count_vehicles)
        return parse_dist_and_tour(self.name_file, self.capacity, self.count_vehicles)
    def opt(self, name_opt: str = '3opt') -> [float, list]:
        """
        Функция, вызывающая алгоритмы локальной оптимизации для решения задачи CVRPTW. На вход подается один параметр.
            :type string name_opt: название метода оптимизации: 2-opt, 3-opt.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """
        print('SHUFFLE Param', self.shuffle_param)
        
        vrp_c.modelMetaHeuristic(f"cvrptw_{name_opt}", self.path_folder, self.count_towns, self.capacity, 0.0, 0.0, self.shuffle_param, self.count_vehicles)
        
        return parse_dist_and_tour(self.name_file, self.capacity, self.count_vehicles)

    def lkh(self) -> [float, list]:
        """
        Функция, вызывающая алгоритм "Эвристика Лина-Кёрнигана" для решения задачи CVRPTW.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """
        print('SHUFFLE Param', self.shuffle_param)
        vrp_c.modelMetaHeuristic("cvrptw_lkh", self.path_folder, self.count_towns, self.capacity, 0.0, 0.0, self.shuffle_param, self.count_vehicles)
        return parse_dist_and_tour(self.name_file, self.capacity, self.count_vehicles)

    def gurobi(self) -> [float, list]:
        """
        Функция, использующая коммерческий проект "Gurobi" для решения задачи CVRPTW.
        
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """ 
        data_file = pd.read_csv(self.name_file, sep="\t", error_bad_lines=True)

        # Запишем все данные в виде массива списков, где каждый список это строка с значениями конкретных столбцов
        arr_data  = data_file.values

        # Задаём объем грузовика
        if(countTowns == 20):
            Q_max = 500
        elif(countTowns == 50):
            Q_max = 750
        elif(countTowns == 100):
            Q_max = 1000

        # Создаём списки координат и грузов для каждого клиента
        xc = np.zeros(len(arr_data))
        yc = np.zeros(len(arr_data))

        for i in range(len(arr_data)):
            if(arr_data[i][2] <= Q_max):
                xc[i] = arr_data[len(arr_data) - 1 - i][0]
                yc[i] = arr_data[len(arr_data) - 1 - i][1]

        # Создаём список клиентов
        clients = []
        for i in range(1, len(arr_data)):
            clients.append(i)

        # Cоздаем список клиентов вместе с депо. Депо обозначено, как последний элемент в arr_data
        nodes = [0] + clients

        # Задаём словари временных ограничений на посещение

        # Минимальное, максимальное время прибытия + перевод в секунды и время обслуживания
        # Также из списка клиентов удаляются те у кого временное окно меньше времени обслуживания
        start_lst  = {}
        finish_lst = {}
        service    = {}
        q = {}
        error_cost = 0
        for i in nodes:
            st = int(arr_data[len(arr_data) - 1 - i][3].split('-')[0]) # start time
            fs = int(arr_data[len(arr_data) - 1 - i][3].split('-')[1]) # finish time
            sv = int(arr_data[len(arr_data) - 1 - i][4])               # service time
            
            start_lst[i] = st
            finish_lst[i] = fs
            service[i] = sv
            q[i] = arr_data[len(arr_data) - 1 - i][2]

        # Создаём список всевохможных пар между клиентами, включая депо
        paires  = [(i, j) for i in nodes for j in nodes if i != j]

        # # Задаем словарь всевозможных времен необходоимых для перемещения между всеми городами, включая депо
        time = {}
        distance = {}
        for i, j in paires:
            distance[i, j] = math.sqrt((xc[i] - xc[j]) ** 2 + (yc[i] - yc[j]) ** 2)
            time[i, j] = distance[i, j] * 100
            
        # 100 точек - минимум 44 машин
        for count_vehicles in range(15, 25):
            #Список транспортных средств
            vehicles  = [i for i in range(0, count_vehicles)]

            #Словарь вместимости транспортных средств
            
            arco_var  = [(i, j, k) for i in nodes for j in nodes for k in vehicles if i != j]

            arco_time = [(i, k) for i in nodes for k in vehicles]

            # Модель
            model = Model('CVRPTW')

            # Переменные для решения
            x = model.addVars(arco_var, vtype=GRB.BINARY, name = 'x')
            t = model.addVars(arco_time, vtype=GRB.CONTINUOUS, name = 't')

            #Целевая функция
            model.setObjective(quicksum(distance[i, j] * x[i,j,k] for i, j, k in arco_var) * 13 + error_cost * 10, GRB.MINIMIZE)

            #Ограничения

            #TODO: Дописать комментарии к ограничениям

            # Прибытие и отъезд
            model.addConstrs(quicksum(x[0, j, k] for j in clients) == 1 for k in vehicles)
            model.addConstrs(quicksum(x[i, 0, k] for i in clients) == 1 for k in vehicles)

            # Ограничение по посещению маршрута ровно один раз и ровно одним грузовиком
            model.addConstrs(quicksum(x[i, j, k] for j in nodes for k in vehicles if i != j) == 1 for i in clients)

            #Ограничение на количество выездов в города и выезов из городов
            model.addConstrs(quicksum(x[i, h, k] for i in nodes if i != h) - quicksum(x[h, j, k] for j in nodes if h != j) == 0 for h in clients for k in vehicles)

            #Ограничение на вместимость грузовика
            model.addConstrs(quicksum(q[i] * quicksum(x[i, j, k] for j in nodes if i != j) for i in clients) <= Q_max for k in vehicles)

            #Ограничение на временя
            model.addConstrs((x[i,j,k] * (t[i, k] + service[i] + time[i, j] - t[j, k]) <= 0) for i in clients for j in clients for k in vehicles if i != j)
            
            model.addConstrs(t[i, k] >= start_lst[i] for i, k in arco_time)
            model.addConstrs(t[i, k] <= finish_lst[i] for i, k in arco_time)

            # Optimizing the model
            model.Params.TimeLimit = 3000 # Временной лимит 
            model.Params.MIPGap = 0.3  # Лимит GAP = 10%
            model.Params.LogFile= f"Gurobi_{len(arr_data)-1}_more_cars.txt"
            model.optimize()
            if model.status == GRB.OPTIMAL:
                print('1.Optimal objective: %g' % model.objbound)
                print("Итоговое расстояние: ", model.objbound)
                value_result = model.objbound
            elif model.status == GRB.INFEASIBLE:
                print('3.Model is infeasible')
            elif model.status == GRB.UNBOUNDED:
                print('4.Model is unbounded')
            else:
                print('5.Optimization ended with status %d' % model.status)

            if(model.status == GRB.OPTIMAL):
                break
