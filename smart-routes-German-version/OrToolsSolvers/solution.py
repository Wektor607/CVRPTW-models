from mip_or_tools import *
from utils.distance import *
import numpy as np
from problem import Problem
from parameters import GenerationParameters
from instance import Instance
import random

if __name__ == '__main__':
    N = 50
    p = 40
    maxCapacity = 100
    solver_name = 'SCIP'

    generate_param = GenerationParameters(n_customer=N)
    create_problem = Problem(capacity=True)

    cities = np.random.uniform(size=(N, 2))
    coords = []
    for i in range(len(cities)):
        coords.append([cities[i][0] * 100, cities[i][1] * 100])
    coords = np.array(coords)

    capacity = np.array(list(np.random.randint(1, 42, N+1)))
    capacity[0] = 0
    
    instance = Instance(problem=create_problem, 
    generation_parameters=generate_param, 
    coordinates=coords, vehicle_capacities=capacity)

    cvrp = CVRP(instance, p, maxCapacity, solver_name)
    print(cvrp.MIP_OrTools())
