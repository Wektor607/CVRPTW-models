import pandas as pd
import numpy as np
from gurobipy import Model, GRB, quicksum
import math

def main():
    print('Введите количество городов (50, 100): ')
    countTowns = int(input())
    for num in range(8, 101):
        name_file = f"VRP_{countTowns}/Example{num}.csv"
        data_file = pd.read_csv(name_file, sep="\t", error_bad_lines=True)

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

if __name__ == "__main__":
    main()