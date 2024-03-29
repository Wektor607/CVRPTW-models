# Smart Routes

Smart Routes - это платформа оптимизации маршрутов доставки грузов.

## Цель проекта

Цель проекта - создание программной платформы, позволяющей:
- Решать задачи класса VRP с **популярными известными ограничениями**,
  например, (C)VRP(TW) и (C)PDP(TW), а также прозрачно добавлять 
  новые **нестандартные ограничения**
- Быстро решать задачи VRP **большой размерности**
- Использовать для решения задачи **реальные или реалистичные данные**:
  карты городов, распределения заказов, данные о пробках 
- Решать задачи VRP в **stochastic** постановке, в которой некоторый из параметров задачи известны неточно - в виде 
  прогнозов (например, прогнозы матриц расстояний с учетом пробок)
- Решать задачи VRP в **dynamic** постановке, при которой структура решаемой 
  задачи может меняться во времени (пример - добавление / модификация / отмена заказов)
- **Проводить эксперименты** с алгоритмами решения задачи VRP разных классов:
  - классические:
    - эвристические
    - MIP
  - обучаемые:
    - комбинация классических методов и ML
    - RL
- **Визуально интерпретировать** решения, предложенные алгоритмами
- **Сравнивать метрики качества** работы алгоритмов

## Необходимый инструментарий
- В папке **smart-routes** необходимо иметь папку **or-tools**.
- Её можно скачать по данной ссылке: https://developers.google.com/optimization/install/cpp

## Milestones

### v0.1

