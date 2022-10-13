from distutils.core import setup, Extension
setup(
      # name='CVRP-TW', 
      version='1.1',

      author= 'German',
      author_email='1069035F@mail.ru',

      url='https://github.com/Wektor607/CVRPTW-models/tree/main/OwnLibrary',
      
      # packages=['vrp_c'],
      ext_modules=[Extension(
            'vrp_c', 
            ['src/vrp-main.c'],
            include_dirs=['src/mainFunctions', 'src/algorithms'])]
)