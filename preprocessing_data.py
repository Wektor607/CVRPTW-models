from selenium import webdriver
from tqdm import tqdm_notebook as tqdmn
import numpy as np
import pandas as pd
from list import *
from main import *
import os

def preprocess(task):
    """
    Функция, обрабатывающая наборы в зависимости от решаемой задачи CVRP или CVRPTW. Подается 1 аргумент:
        :type string task: название решаемой задачи CVRP или CVRP-TW
    """
    if(task == 'CVRPTW'):
        for i in range(len(lst)):
            file_name = lst[i]
            acnc = pd.read_csv(file_name, sep= "\t", keep_default_na=False)
            mel = acnc[
                ['OrderID', 'Weight', 'PickupAddress', 'DeliveryAddress', 'DeliveryTimeRange', 'UnloadingTime']].copy()
            mel['Url_depo'] = ['https://www.google.com/maps/search/' + i for i in mel['PickupAddress'] ]
            mel['Url_clients'] = ['https://www.google.com/maps/search/' + i for i in mel['DeliveryAddress'] ]
            Url_With_Coordinates_depo = []
            Url_With_Coordinates_clients = []

            option = webdriver.ChromeOptions()
            prefs = {'profile.default_content_setting_values': {}}
            option.add_experimental_option('prefs', prefs)
            driver = webdriver.Chrome(os.getcwd() + "\chromedriver.exe", options=option)

            for url in tqdmn(mel.Url_depo, leave=False):
                driver.get(url)
                Url_With_Coordinates_depo.append(driver.find_element_by_css_selector('meta[itemprop=image]').get_attribute('content'))

            for url in tqdmn(mel.Url_clients, leave=False):
                driver.get(url)
                Url_With_Coordinates_clients.append(driver.find_element_by_css_selector('meta[itemprop=image]').get_attribute('content'))

            driver.close()

            mel['Url_With_Coordinates_depo'] = Url_With_Coordinates_depo
            mel['Url_With_Coordinates_clients'] = Url_With_Coordinates_clients
            mel = mel[mel.Url_With_Coordinates_clients.str.contains('&zoom=')].copy()

            mel['x_depo'] = [ url.split('?center=')[1].split('&zoom=')[0].split('%2C')[0] for url in mel['Url_With_Coordinates_depo'] ]
            mel['y_depo'] = [url.split('?center=')[1].split('&zoom=')[0].split('%2C')[1] for url in mel['Url_With_Coordinates_depo'] ]
            mel['x_client'] = [ url.split('?center=')[1].split('&zoom=')[0].split('%2C')[0] for url in mel['Url_With_Coordinates_clients'] ]
            mel['y_client'] = [url.split('?center=')[1].split('&zoom=')[0].split('%2C')[1] for url in mel['Url_With_Coordinates_clients'] ]


            mel = mel.drop('OrderID', 1)
            mel = mel.drop('PickupAddress', 1)
            mel = mel.drop('DeliveryAddress', 1)
            mel = mel.drop('Url_depo', 1)
            mel = mel.drop('Url_clients', 1)
            mel = mel.drop('Url_With_Coordinates_depo', 1)
            mel = mel.drop('Url_With_Coordinates_clients', 1)

            str_data = {'Weight': 0.0, 'DeliveryTimeRange': mel['DeliveryTimeRange'][0], 'x_client':mel['x_depo'][0], 
                        'y_client': mel['y_depo'][0]}
            mel = mel.drop('x_depo', 1)
            mel = mel.drop('y_depo', 1)
            mel = mel.append(str_data, ignore_index=True)

            mel['UnloadingTime'] = np.where((mel.Weight == 0), 0.0, mel.UnloadingTime)
            mel = mel.reindex(columns=['x_client', 'y_client', 'Weight', 'DeliveryTimeRange', 'UnloadingTime'])

            mel.to_csv(file_name, sep='\t', index = False)
    else:
        for i in range(len(lst)):
            file_name = lst[i]
            acnc = pd.read_csv(file_name, sep= "\t", keep_default_na=False)
            mel = acnc[
                ['OrderID', 'Weight', 'PickupAddress', 'DeliveryAddress']].copy()
            mel['Url_depo'] = ['https://www.google.com/maps/search/' + i for i in mel['PickupAddress'] ]
            mel['Url_clients'] = ['https://www.google.com/maps/search/' + i for i in mel['DeliveryAddress'] ]
            Url_With_Coordinates_depo = []
            Url_With_Coordinates_clients = []

            option = webdriver.ChromeOptions()
            prefs = {'profile.default_content_setting_values': {}}
            option.add_experimental_option('prefs', prefs)
            print('NIHAO')
            driver = webdriver.Chrome(os.getcwd() + "\chromedriver.exe", options=option)

            for url in tqdmn(mel.Url_depo, leave=False):
                driver.get(url)
                Url_With_Coordinates_depo.append(driver.find_element_by_css_selector('meta[itemprop=image]').get_attribute('content'))

            for url in tqdmn(mel.Url_clients, leave=False):
                driver.get(url)
                Url_With_Coordinates_clients.append(driver.find_element_by_css_selector('meta[itemprop=image]').get_attribute('content'))

            driver.close()

            mel['Url_With_Coordinates_depo'] = Url_With_Coordinates_depo
            mel['Url_With_Coordinates_clients'] = Url_With_Coordinates_clients
            mel = mel[mel.Url_With_Coordinates_clients.str.contains('&zoom=')].copy()

            mel['x_depo'] = [ url.split('?center=')[1].split('&zoom=')[0].split('%2C')[0] for url in mel['Url_With_Coordinates_depo'] ]
            mel['y_depo'] = [url.split('?center=')[1].split('&zoom=')[0].split('%2C')[1] for url in mel['Url_With_Coordinates_depo'] ]
            mel['x_client'] = [ url.split('?center=')[1].split('&zoom=')[0].split('%2C')[0] for url in mel['Url_With_Coordinates_clients'] ]
            mel['y_client'] = [url.split('?center=')[1].split('&zoom=')[0].split('%2C')[1] for url in mel['Url_With_Coordinates_clients'] ]

            mel = mel.drop('OrderID', 1)
            mel = mel.drop('PickupAddress', 1)
            mel = mel.drop('DeliveryAddress', 1)
            mel = mel.drop('Url_depo', 1)
            mel = mel.drop('Url_clients', 1)
            mel = mel.drop('Url_With_Coordinates_depo', 1)
            mel = mel.drop('Url_With_Coordinates_clients', 1)
            mel = mel.drop('DeliveryTimeRange', 1)
            mel = mel.drop('UnloadingTime', 1)

            str_data = {'Weight': 0.0, 'x_client':mel['x_depo'][0], 
                        'y_client': mel['y_depo'][0]}
            mel = mel.drop('x_depo', 1)
            mel = mel.drop('y_depo', 1)
            mel = mel.append(str_data, ignore_index=True)

            mel = mel.reindex(columns=['x_client', 'y_client', 'Weight'])

            mel.to_csv(file_name, sep='\t', index = False)