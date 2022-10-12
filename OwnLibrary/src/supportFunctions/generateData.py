import numpy as np
def generate_data(n_samples=1, 
                  n_customer=20,
                  service_window=1000,
                  service_duration=10,
                  time_factor=100.0,
                  tw_expansion=3.0):
    
    rnds = np.random

    # sample locations
    dloc = rnds.uniform(size=(n_samples, 1, 2))  # depot location
    nloc = rnds.uniform(size=(n_samples, n_customer, 2))  # node locations
    
    # generate capacities 
    min_capasity = 1
    max_capasity = 42
    capacity = np.minimum(np.maximum(np.abs(np.random.normal(15, 10, size=[n_samples, n_customer])), min_capasity), max_capasity)
    
    # TW start needs to be feasibly reachable directly from depot
    min_t = np.ceil(np.linalg.norm(
        dloc[:, :1] * time_factor - nloc * time_factor, axis=-1)) + 1
    # TW end needs to be early enough to perform service and return to depot until end of service window
    max_t = np.ceil(np.linalg.norm(
        dloc[:, :1] * time_factor - nloc * time_factor, axis=-1) + service_duration) + 1
    
    # horizon allows for the feasibility of reaching nodes / returning from nodes within the global tw (service window)
    horizon = list(zip(min_t, service_window - max_t))
    epsilon = np.maximum(np.abs(rnds.standard_normal(
        [n_samples, n_customer])), 1 / time_factor)
    
    # sample earliest start times a
    a = [rnds.randint(*h) for h in horizon]
    # calculate latest start times b, which is
    # a + service_time_expansion x normal random noise, all limited by the horizon
    # and combine it with a to create the time windows
    tw = [np.transpose(np.vstack((rt,
                                  np.minimum(
                                      rt + tw_expansion * time_factor * sd, h[-1]).astype(int)  # b
                                  ))).tolist()
          for rt, sd, h in zip(a, epsilon, horizon)]
    
    return dloc, nloc, capacity, tw, service_duration

def translate_minute(minute): 
    hour = minute // 60 
    minute %= 60 
    return "%02d:%02d" % (hour, minute)

if __name__ == "__main__":

    # Размерность задачи
    print("Введиите количество городов: ")
    n_customer = int(input())

    # Время открытия и закрытия депо в часах
    print("Введите начало временного окна депо от 0 до 24 часов в формате XX:XX : ")
    depot_start_time = input()
    print("Введите конец временного окна депо от 0 до 24 часов в формате XX:XX : ")
    depot_end_time = input()

    for i in range(1, 101):
        depot, points, capacity, tw, deliver_time = generate_data(n_customer=n_customer)
        with open(f"{n_customer}/Example{i}.csv", "w") as f:
            f.write('x_coord' + '\t' + 'y_coord' + '\t' + 'Weight' + '\t' + 'DeliveryTimeRange' + '\t' + 'UnloadingTime' + '\n')
            
            for j in range(len(points[0])):
                f.write(str(points[0][j][0] * 100) + '\t' + str(points[0][j][1] * 100) + '\t' + str(capacity[0][j]) 
                        + '\t' + str(translate_minute(tw[0][j][0])) + '-' + str(translate_minute(tw[0][j][1])) + '\t' + str(deliver_time) + '\n')
            
            f.write(str(depot[0][0][0] * 100) + '\t' + str(depot[0][0][1] * 100) + '\t' + str(0) 
                        + '\t' + depot_start_time + '-' + depot_end_time + '\t' + str(0))