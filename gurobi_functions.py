import pandas as pd
import requests

def try_request(url):
    """
    Функция, повторно подающая http запрос для получения расстояния между двумя точками (между клиентами или между клиентом и депо) в случае возникновения ошибки. На вход подается 1 параметр:
        :type string url: запрос для полученя расстояния между двумя точками.
    """
    while(1):
        try:
            r = requests.get(url)
            return r
        except:
            print("ERROR")
            
def distance_file(n, lst, lst_distance):
    """
    Функция, возвращающая файл с расстояниями между всеми городами, в том числе и депо. На вход подается три параметра:
            :type int n: количество городов в неоптимизированном маршруте;
            :type list lst: список с ссылками на файлы с задачами;
            :type list lst_distance: список с ссылками на файлы с расстояними для каждой задачи;
    """
    for i in range(len(lst)):
        file_name = lst[i]
        file_dist = lst_distance[i]
        dist = pd.DataFrame(dict(
            x1 = [],
            y1 = [],
            x2 = [],
            y2 = [],
            distance = []))
        acnc = pd.read_csv(file_name, sep= "\t", error_bad_lines=False)
        j_start = 0
        for k in range(n):
            x1 = acnc.values[k][0]
            y1 = acnc.values[k][1]
            j = 0
            while(j < n):
                x2 = acnc.values[j][0]
                y2 = acnc.values[j][1]
                url = "http://router.project-osrm.org/route/v1/driving/{},{};{},{}".format(y1, x1, y2, x2)
                r = try_request(url)
                res = r.json()
                dist.loc[j_start, 'x1'] = x1
                dist.loc[j_start, 'y1'] = y1
                dist.loc[j_start, 'x2'] = x2
                dist.loc[j_start, 'y2'] = y2
                dist.loc[j_start, 'distance'] = res['routes'][0]['legs'][0]['distance']
                j += 1
                j_start += 1
        dist.to_csv(file_dist, index = False)