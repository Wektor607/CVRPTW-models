#include <Python.h>
#include "OrTools_LP_Modify_Version.cc"

static PyObject *OrToolsModels(PyObject *self, PyObject *args) {
   int N, p, maxCapacity;
   char *solver_name, *dist_file, *cap_file;

   if (!PyArg_ParseTuple(args, "iiisss", &N, &p, &maxCapacity, &solver_name, &dist_file, &cap_file)) {
      return NULL;
   }
   
   operations_research::mipModel(N, p, maxCapacity, solver_name, dist_file, cap_file);
   return Py_BuildValue("f", "Hello, Python extensions!!");
};

static PyMethodDef helloworld_funcs[] = {
   {"OrToolsModels", (PyCFunction)OrToolsModels,
      METH_VARARGS},
   {NULL, NULL, 0, NULL}
};

static struct PyModuleDef cModPyDem =
{
    PyModuleDef_HEAD_INIT,
    "VRP", /* name of module */
    "Extension module example!",          /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    helloworld_funcs
};

PyMODINIT_FUNC PyInit_vrp_c(void)
{
   return PyModule_Create(&cModPyDem);
}
