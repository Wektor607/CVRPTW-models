from distutils.core import setup, Extension
setup(
      version='1.1',

      author= 'German',
      author_email='1069035F@mail.ru',

      url='https://github.com/Wektor607/CVRPTW-models/tree/main/OwnLibrary',
      
      ext_modules=[Extension(
            'vrp_c', 
            sources=[   
            'src/vrp-main.c', 
            'src/algorithms/SA.c', 
            'src/algorithms/twoOpt.c',
            'src/algorithms/threeOpt.c',
            'src/algorithms/LKH.c',
            'src/extraFunctions/distanceMatrix.c',
            'src/extraFunctions/dataProcessing.c',
            'src/extraFunctions/townActions.c',
            'src/extraFunctions/createObjects.c',
            'src/extraFunctions/logistic.c',
      ])]
)