import numpy as np

lst = [ 
        # "SA_RES_CVRPTW20.txt",
        # "SA_RES_CVRPTW50.txt",
        # "results/SA_RES_CVRPTW100.txt",
        # "LKH_2opt_RES_CVRPTW20.txt",
        # "LKH_2opt_RES_CVRPTW50.txt",
        # "results/LKH_2opt_RES_CVRPTW100.txt",
        # "LKH_3opt_RES_CVRPTW20.txt",
        "LKH_3opt_RES_CVRPTW50.txt",
        # "results/LKH_3opt_RES_CVRPTW100.txt"
    ]
        
for p in lst:
    res_file = open(p, 'r')
    arr = []
    for line in res_file:
        arr.append(line)
        
    small_arr = []
    for i in range(len(arr)):
        if(arr[i] == '\n'):
            small_arr.append(arr[i-2])
    small_arr.append(arr[len(arr)-2])

    res = []
    all_costs = []
    all_time = []
    for i in range(len(small_arr)):
        cost = ''
        time = ''
        was_t = False
        for j in small_arr[i]:
            if(j != '\t' and was_t == False):
                cost += j
            else:
                was_t = True
            if(j != '\t' and j != '\n' and was_t == True):
                time += j
        all_costs.append(float(cost))
        all_time.append(float(time))
    print(p, np.mean(all_costs), np.mean(all_time))