import vrp_c
from datetime import datetime
from utils.distance import *
from pydantic import BaseModel
import os
import numpy as np

class MIP_OrTools:
    """
    Класс, в котором объявлены основные параметры, использующиеся в классе CVRP:
        :type data instance:      содержит базовые параметры: количество точек, матрицу расстояний, capacity клиентов;
        :type int p:              количество транспортных средств;
        :type string solver_name: название решателя;
    """ 
    def __init__ (self, instance, p, solver_name):
        self.instance    = instance
        self.p           = p 
        self.solver_name = solver_name
        
class CVRP(MIP_OrTools):
    """
    Основной класс для решения задачи CVRP.
    """
    def __init__ (self, instance, p, Q, solver_name):
        super().__init__(instance, p, solver_name)
        self.Q = Q 
   
    def MIP_OrTools(self):
        n_customers = self.instance.generation_parameters.n_customer
        coords = self.instance.coordinates
        capacity = self.instance.vehicle_capacities
        dist_file = EuclidianDistancer().create_distance_matrix_and_save_file(coords[np.newaxis, ...])
        
        file_cap = open('utils/capacity.txt', 'w')
        
        for i in range(1, len(capacity)):
            file_cap.write(str(capacity[i]) + '\n')
        file_cap.close()
        
        vrp_c.OrToolsModels(n_customers, self.p, self.Q, self.solver_name, dist_file, file_cap.name)
        return "The END"
