from distutils.core import setup, Extension
setup(name='vrp_c', version='1.1',  \
      ext_modules=[Extension('vrp_c', ['vrp-main.c'])])