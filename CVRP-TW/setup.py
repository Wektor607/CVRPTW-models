from setuptools import setup, Extension, find_packages
setup(
      name='CVRP-TW', 
      version='0.0.1',

      author= 'German',
      author_email='1069035F@mail.ru',

      url='https://github.com/Wektor607/CVRPTW-models/tree/main/OwnLibrary',
      
      packages=['CVRP-TW'],
      ext_modules=[Extension(
      'vrp_c', 
      sources=[   
            'CVRP-TW/vrp-main.c', 
            'CVRP-TW/algorithms/SA.c', 
            'CVRP-TW/algorithms/twoOpt.c',
            'CVRP-TW/algorithms/threeOpt.c',
            'CVRP-TW/algorithms/LKH.c',
            'CVRP-TW/mainFunctions/distanceMatrix.c',
            'CVRP-TW/mainFunctions/dataProcessing.c',
            'CVRP-TW/mainFunctions/townActions.c',
            'CVRP-TW/mainFunctions/createObjects.c',
            'CVRP-TW/mainFunctions/logistic.c',
      ])]
)