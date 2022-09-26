import vrp_c

class VRP:
    """
    Класс, в котором объявлены основные параметры, использующиеся в классах CVRP, CVRPTW:
        :type string name_file:   Название файла с данными;
        :type string path_folder: Название папки, в которой буду сохранены бинарные файлы с матрицами расстояний;
        :type int count_towns: Количество городов;
        :type int countTasks:  Количество итераций для решения одной оптимизационной задачи.
    """ 
    def __init__ (self, alg_name, name_file, name_matrix_file, path_folder, count_towns, countTasks: int = 10000):
        self.alg_name    = alg_name
        self.name_file   = name_file 
        self.name_matrix_file = name_matrix_file
        self.path_folder = path_folder 
        self.count_towns = count_towns
        self.count_Tasks = countTasks
        self.parse_file()


def parse_dist_and_tour(file_name, max_capacity, count_vehicles):
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

def check(all_pairs, timer, distance, start_lst, finish_lst, service):
    local_timer = timer
    for i, j, k in all_pairs:
        local_timer += distance[i, j] + service[j]
        if(not(local_timer >= start_lst[j])):
            local_timer = start_lst[j] + service[j]
        if(not(local_timer >= start_lst[j] and local_timer <= finish_lst[j] and local_timer <= finish_lst[0])):
            return "BAD"
    return local_timer - timer
        
class CVRPTW (VRP):
    """
    Это основной класс для решения задачи CVRPTW
    """
    def __init__ (self, alg_name, name_file, name_matrix_file, path_folder, count_towns, countTasks: int = 10000, capacity: int = 30, time_start: int = 0, time_end: int = 0, count_vehicles: int = 10):
        super().__init__(alg_name, name_file, name_matrix_file, path_folder, count_towns, countTasks)
        self.capacity = capacity
        self.countTasks = countTasks
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
            vrp_c.parseOneTwTownPy(self.name_file, self.name_matrix_file, self.path_folder, self.count_towns)

    def sa(self, T: float = 1000, t_min: float = 10) -> [float, list]: #TODO: В SA передавать температуру с клавиатуры
        """
        Функция, вызывающая алгоритм "Имитации отжига" для решения задачи CVRPTW. На вход подается два параметра:
            :type float T:     начальная температура, которая с течением времени убывает;
            :type float t_min: конечная температура, до которой опускается температура T.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """ 
        self.parse_file()
        vrp_c.modelMetaHeuristic("cvrptw_sa", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        return parse_dist_and_tour(self.name_file, self.capacity, self.count_vehicles)
    
    def opt(self, name_opt: str = 'lkh3opt') -> [float, list]:
        """
        Функция, вызывающая алгоритм "Эвристика Лина-Кёрнигана" для решения задачи CVRPTW. На вход подается один параметр.
            :type string name_opt: название метода оптимизации: 2-opt, 3-opt.
        По окончании работы создается файл с данными, состоящий из двух колонок:
            1. В первом столбце записывается длина маршрута, выраженная в метрах, в определенный момент времени;
            2. Во втором столбце записывается время, которое потребовалось, чтобы оптимизировать маршрут до некоторой длины.
        """
        if(name_opt == '2opt'):
            vrp_c.modelMetaHeuristic("cvrptw_lkh_2opt", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        elif(name_opt == '3opt'):
            vrp_c.modelMetaHeuristic("cvrptw_lkh_3opt", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        # elif(name_opt == 'lkh'):
        #     vrp_c.modelMetaHeuristic("cvrptw_lkh", self.path_folder, self.count_towns, self.capacity, self.countTasks)
        return parse_dist_and_tour(self.name_file, self.capacity, self.count_vehicles)