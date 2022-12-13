import pandas as pd
import numpy as np
from gurobipy import Model, GRB, quicksum
import math

def check(all_pairs, timers):
    for i, j, k in all_pairs:
        if(j == 0):
            local_timer = timers[f't[{i},{k}]']
            break
    return local_timer

def main():
    min_res = []
    allResultFile = "allResults_50.txt"
    for num in range(1, 100):
        final_res = []
        name_file = f"VRP_50/Example{num}.csv"
        data_file = pd.read_csv(name_file, sep="\t", error_bad_lines=True)

        # Запишем все данные в виде массива списков, где каждый список это строка с значениями конкретных столбцов
        arr_data = data_file.values

        # Задаём объем грузовика
        Q_max    = 750 # Для 20:500, 50:750, 100:1000

        # Создаём списки координат и грузов для каждого клиента
        xc       = np.zeros(len(arr_data))
        yc       = np.zeros(len(arr_data))

        for i in range(len(arr_data)):
            if(arr_data[i][2] <= Q_max):
                xc[i] = arr_data[len(arr_data) - 1 - i][0] * 100
                yc[i] = arr_data[len(arr_data) - 1 - i][1] * 100

        # Создаём список клиентов
        clients  = []
        for i in range(1, len(arr_data)):
            clients.append(i)

        # Cоздаем список клиентов вместе с депо. Депо обозначено, как последний элемент в arr_data
        nodes   = [0] + clients

        # Задаём словари временных ограничений на посещение

        # Минимальное, максимальное время прибытия + перевод в секунды и время обслуживания
        # Также из списка клиентов удаляются те у кого временное окно меньше времени обслуживания
        start_lst  = {}
        finish_lst = {}
        service    = {}
        q = {}
        del_elem = []
        for i in nodes:
            st = int(arr_data[len(arr_data) - 1 - i][3].split('-')[0]) 
            fs = int(arr_data[len(arr_data) - 1 - i][3].split('-')[1]) 
            sv = int(arr_data[len(arr_data) - 1 - i][4]) 
            if(sv <= fs - st):
                start_lst[i] = st
                finish_lst[i] = fs
                service[i] = sv
                q[i] = arr_data[len(arr_data) - 1 - i][2]
            else:
                del_elem.append(i)

        for i in del_elem:
            clients.remove(i)
            nodes.remove(i)

        # print("Удаленные точки:", del_elem)
        # Создаём список всевохможных пар между клиентами, включая депо
        paires  = [(i, j) for i in nodes for j in nodes if i != j]

        # # Задаем словарь всевозможных времен необходоимых для перемещения между всеми городами, включая депо
        time = {}
        distance = {}
        for i, j in paires:
            distance[i, j] = math.sqrt((xc[i] - xc[j]) ** 2 + (yc[i] - yc[j]) ** 2)
            time[i, j] = distance[i, j]
            # print([i,j], ':', time[i, j], '\n')

        for count_vehicles in range(1, 10):
            #Список транспортных средств
            vehicles = [i for i in range(0, count_vehicles)]

            #Словарь вместимости транспортных средств
            Q         = {i: Q_max for i in range(0, count_vehicles)}

            arco_var  = [(i, j, k) for i in nodes for j in nodes for k in vehicles if i != j]

            arco_time = [(i, k) for i in nodes for k in vehicles]

            # Модель
            model = Model('CVRPTW')

            # Переменные для решения
            x = model.addVars(arco_var, vtype=GRB.BINARY, name = 'x')
            t = model.addVars(arco_time, vtype=GRB.CONTINUOUS, name = 't')

            #Целевая функция
            model.setObjective(quicksum(distance[i, j] * x[i,j,k] for i, j, k in arco_var), GRB.MINIMIZE)

            #Ограничения

            #TODO: Дописать комментарии к ограничениям

            # Прибытие и отъезд
            model.addConstrs(quicksum(x[0, j, k] for j in clients) == 1 for k in vehicles)
            model.addConstrs(quicksum(x[i, 0, k] for i in clients) == 1 for k in vehicles)

            # Ограничение по посещению маршрута ровно один раз и ровно одним грузовиком
            model.addConstrs(quicksum(x[i, j, k] for j in nodes for k in vehicles if i != j) == 1 for i in clients)

            #
            model.addConstrs(quicksum(x[i, j, k] for j in nodes if i != j) - quicksum(x[j, i, k] for j in nodes if i != j) == 0 for i in clients for k in vehicles)

            #Ограничение на вместимость грузовика
            model.addConstrs(quicksum(q[i] * quicksum(x[i, j, k] for j in nodes if i != j) for i in clients) <= Q[k] for k in vehicles)

            #Ограничение по времени
            model.addConstrs((x[i,j,k] * (t[i, k] + service[i] + time[i, j] - t[j, k]) <= 0) for i in clients for j in clients for k in vehicles if i != j)
            model.addConstrs(t[i, k] >= start_lst[i] for i, k in arco_time)
            model.addConstrs(t[i, k] <= finish_lst[i] for i, k in arco_time)

            # Optimizing the model
            model.Params.TimeLimit = 600 # Временной лимит 10 минут
            model.Params.MIPGap = 0.05   # Лимит GAP = 5%
            model.Params.LogFile= f"Gurobi_{len(arr_data)-1}.txt"
            model.optimize()
            if model.status == GRB.OPTIMAL:
                print('1.Optimal objective: %g' % model.objVal)
                # print('Optimal cost: %g' % model.objVal)
            elif model.status == GRB.INFEASIBLE:
                print('3.Model is infeasible')
            elif model.status == GRB.UNBOUNDED:
                print('4.Model is unbounded')
            else:
                print('5.Optimization ended with status %d' % model.status)

            # Список времен в секундах, которое понадобилось для прохождения каждого подмаршрута
            # print("model.objVal: ", model.objVal)
            print()
            if(model.status == GRB.OPTIMAL):
                timers = {}
                for v in model.getVars():
                    if(v.x > 1):
                        timers[v.VarName] = v.x
                print(timers)
                print()
                if (model.objVal != math.inf):
                    active_arcs = [a for a in arco_var if x[a].x > 0.99]
                    print(active_arcs)
                    res_gur = []
                    timer = start_lst[0]
                    for c in range(0, count_vehicles):
                        active_arcs_1 = []
                        for i in range(len(active_arcs)):
                            if(active_arcs[i][2] == c):
                                active_arcs_1.append(active_arcs[i])

                        res_sort = []
                        if(active_arcs_1 != []):
                            res_sort.append(active_arcs_1[0])
                            while(res_sort[len(res_sort)-1][1] != 0):
                                for i in range(len(active_arcs_1)):
                                    if(active_arcs_1[i][0] == res_sort[len(res_sort)-1][1]):
                                        tu = active_arcs_1[i]
                                        break
                                res_sort.append(tu)
                            print(res_sort)
                            time_len = check(res_sort, timers)
                            res_gur.append(time_len)
                            print("RES: ", res_gur)
                    # Общее время прохождения всего маршрута
                    s = 0
                    r = model.objVal
                    for i in res_gur:
                        if(i != 'BAD'):
                            s += i
                        else:
                            s = "NoSolution"
                            r = "NoSolution"
                            break
                    print('')
                    print('Real summary time: ',s)
                else:
                    s = "NoSolution"
                    r = "NoSolution" 
                    
                with open("allResultFile.txt", "a") as f:
                    f.write(name_file.split('/')[1].split('.csv')[0] + ' ' + str(count_vehicles) + ' ' + str(s) + ' ' + str(model.Runtime) + '\n')
                f.close()

                if(s != "NoSolution"):
                    final_res.append(s)
        if(final_res == []):
            min_res.append("NoSolution")
        else:
            min_res.append(min(final_res))
    print("MIN_LST: ", min_res)

if __name__ == "__main__":
    main()