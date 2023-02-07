from distutils.core import setup, Extension

setup(name='vrp_c', version='1.1', ext_modules=[
    Extension('vrp_c', ['wrap.cpp'],
    include_dirs=["../or-tools/ortools",
                  "../or-tools/include"],
    libraries=["ortools"],
    library_dirs=["../or-tools/lib"],
    runtime_library_dirs=["../or-tools/lib"],
    extra_compile_args=['-std=c++11','-Wall'])
])