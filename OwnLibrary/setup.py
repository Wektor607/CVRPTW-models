from distutils.core import setup, Extension
setup(
      name='vrp_c', 
      version='1.1',

      author= 'German',
      author_email='1069035F@mail.ru',

      url=
      ext_modules=[Extension('vrp_c', ['src/vrp-main.c'])
])