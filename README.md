# CVRPTW Models

# Содержание файлов
- ***main.py*** - в файле передаются параметры в выбранный метод.
- ***twMethods.py*** - файл содержит главные класс VRP и подкласс CVRPTW, содержащий вызов алгоритмов SA, 2Opt, 3Opt и реализацию алгоритма, который использует инструментарий коммерческого решателя Gurobi. 
- ***generate_tw_data.py*** - файл генерирует данные. Необходимо указать количество городов, начало и конец работы депо.
- ***vrptw-logistic.c*** - файл содержит реализации алгоритмов SA, 2Opt, 3Opt.
- ***logistic.h*** - файл содержит реализации методов для вычисления евклидового расстояния и настоящего расстояния с использованием OSRM между городами.
- ***distanceMatrix.h*** - файл создает матрицу времен, а также содержит методы для печати ее в консоль.
- ***vrp-main.c*** - файл является оберткой для реализаций алгоритмов на языке **C** для обращения к ним из **Python**.

# Запуск решения

- Установить исходные файлы с решениями 
- Для запуска алгоритмов в командной строке Linux вызвать команду: ***make***
- После сборки проекта предлагается прочтение документации, а затем выбор алогритма, который будет решать выбранную задачу

# Генерация данных
Данные генерируются с помощью команды: *python3 generate_tw_data.py*. Далее предлагается выбрать количество городов и временное окно депо.

# Дополнительное пояснение
Данные хранятся в папках *"**20**"*, *"**50**"*, *"**100**"*. В папкаx *"**20_tw**"*, *"**50_tw**"*, *"**100_tw**"* хранятся бинарные файлы, содержащие матрицы времен (времена, которые требуются для переезда между городами, включая депо).
