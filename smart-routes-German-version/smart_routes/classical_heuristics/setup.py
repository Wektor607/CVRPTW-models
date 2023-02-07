from distutils.core import setup, Extension
setup(
      version='1.1',

      author= 'German',
      author_email='1069035F@mail.ru',

      url='https://github.com/Wektor607/CVRPTW-models/tree/main/Experiments',
      
      ext_modules=[Extension(
            'vrp_c', 
            sources=[   
            'classical_heuristics/vrp-main.c', 
            'classical_heuristics/algorithms/SA.c', 
            'classical_heuristics/algorithms/twoOpt.c',
            'classical_heuristics/algorithms/threeOpt.c',
            'classical_heuristics/algorithms/LKH.c',
            'classical_heuristics/extraFunctions/distanceMatrix.c',
            'classical_heuristics/extraFunctions/dataProcessing.c',
            'classical_heuristics/extraFunctions/townActions.c',
            'classical_heuristics/extraFunctions/createObjects.c',
            'classical_heuristics/extraFunctions/logistic.c',
      ])]
)