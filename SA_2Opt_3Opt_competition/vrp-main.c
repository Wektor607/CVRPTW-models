#include <Python.h>
#include "cvrptw-logistic.c"
#include <time.h> 
// #include "logistic.h"

// Компиляция программы: gcc -I/usr/include/python3.8 -c vrp-main.c -lm -o mac 
// sudo python3 setup.py install
// python3

static PyObject *parseOneTwTownPy(PyObject *self, PyObject *args) {
   char *in;
   char *in_arr;
   char *out;
   int tcountTown;

   if (!PyArg_ParseTuple(args, "sssi", &in, &in_arr,&out, &tcountTown)) {
      return NULL;
   }

   parseOneTwTownNoIndex(in, in_arr, out, tcountTown);
   return Py_BuildValue("s", "Hello, Python extensions!!");
};

void write_cvrptw_subtour(FILE* res_f, twtown* sub, int len_sub)
{
   for(int i = 0; i < len_sub-1; i++)
   {
      fprintf(res_f, "%d ", sub[i].t.name);
   }
   fprintf(res_f, "%d#", sub[len_sub-1].t.name);
}

void write_cvrptw_end_tour(FILE* res_f, double distanceInTour)
{
   if(distanceInTour == -1)
   {
      fprintf(res_f, "@%lf@\n", distanceInTour);
   } else {
      fprintf(res_f, "@%lf@\n", distanceInTour);
   }
}

twtown save_request_to_sub(twtown *sub, int lensub, int idx, twtown town0)
{
   if(idx > (lensub-1) || idx < 0)
   {
      return town0;
   }
   return sub[idx];
}

#define CVRPTW(algfunc) \
   srand(time(NULL)); \
   FILE *out = fopen(fileout, "w"); \
   FILE *res_distance = fopen("res_distance.txt", "w");\
   if(res_distance == NULL) {exit(-1);}\
   if(out == NULL) {exit(-1);}\
   twtown *towns; \
   towns = malloc(tcountTown * sizeof(twtown));\
   halfmatrix m; \
   readOneTwTownByBinaryNoIndex(towns, &m, in); \
   twtown town0 = getTwTownByName(0, countTowns, towns);\
   double timer = town0.mTimeStart;\
   double endTime = town0.mTimeEnd;\
   printTwTownList(towns, tcountTown);\
   printf("\n");\
   for(int c = 0; c < countTowns; c++)\
   {\
      if((town0.mTimeStart < town0.mTimeEnd && town0.mTimeStart > towns[c].mTimeEnd && towns[c].mTimeEnd > towns[c].mTimeStart) || \
           (town0.mTimeEnd < towns[c].mTimeStart && town0.mTimeStart > towns[c].mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd && town0.mTimeEnd > town0.mTimeStart))\
      {\
         towns[c].t = zerotown;\
      }\
      else if((town0.mTimeStart > towns[c].mTimeStart && town0.mTimeStart < towns[c].mTimeEnd && town0.mTimeEnd > towns[c].mTimeEnd) || \
         (towns[c].mTimeStart < town0.mTimeStart && towns[c].mTimeEnd < town0.mTimeEnd && town0.mTimeStart > town0.mTimeEnd))\
      {\
         towns[c].mTimeStart = town0.mTimeStart;\
      }\
      else if(towns[c].mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart < town0.mTimeStart && towns[c].mTimeEnd > town0.mTimeEnd)\
      {\
         towns[c].mTimeStart = town0.mTimeStart;\
         towns[c].mTimeEnd = town0.mTimeEnd;\
      }\
      else if((town0.mTimeEnd > towns[c].mTimeStart && town0.mTimeStart < towns[c].mTimeEnd && town0.mTimeEnd < town0.mTimeStart) || \
         (town0.mTimeStart < towns[c].mTimeStart && town0.mTimeEnd > towns[c].mTimeEnd && towns[c].mTimeEnd < towns[c].mTimeStart && town0.mTimeEnd < town0.mTimeStart) || \
         (town0.mTimeStart < towns[c].mTimeEnd && towns[c].mTimeStart > town0.mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd) || \
         (town0.mTimeStart < towns[c].mTimeEnd && towns[c].mTimeStart < town0.mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd) || \
         (town0.mTimeStart > towns[c].mTimeEnd && town0.mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart > towns[c].mTimeEnd) || \
         (towns[c].mTimeEnd > towns[c].mTimeStart && town0.mTimeEnd > towns[c].mTimeEnd && town0.mTimeStart > town0.mTimeEnd) || \
         (towns[c].mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart > town0.mTimeStart && towns[c].mTimeEnd > town0.mTimeEnd))\
      {\
         continue;\
      }\
      if(towns[c].mTimeService > towns[c].mTimeEnd - towns[c].mTimeStart){\
         towns[c].t = zerotown;\
      }\
      if(towns[c].t.weight > maxCapacity || (towns[c].mTimeStart - towns[c].mTimeEnd) == 0) {\
         towns[c].t = zerotown;\
      }\
      if(town0.mTimeStart - town0.mTimeEnd == 0)\
      {\
         printf("Impossible to optimize tour");\
         exit(-1);\
      }\
   }\
   printtwtown(towns[1]);\
   twtown *sub = (twtown*)malloc((countTowns - 1) * sizeof(twtown));\
   int w = 0;\
   twtown t;\
   for(int i = 1; i < countTowns; i++){\
      t = getTwTownByName(i, countTowns, towns);\
      if(t.t.name == -1) {printf("Error town: %d\n", i); continue;}\
      sub[w] = t;\
      w++;\
   }\
   int newCountTowns = w;\
   sub = realloc(sub, newCountTowns * sizeof(twtown));\
   printf("sub: ");\
   for(int i = 0; i < newCountTowns; i++) {\
      printf("%d ", sub[i].t.name);\
   }\
   putchar('\n');\
   printtwtown(sub[1]);\
   twtown temp[newCountTowns];\
   double td;\
   double distanceInTourBest = -1.0, distanceInTourNew = 0.0;\
   double runtime = clock();\
   int days, cap, l, g;\
   double full_time = 0;\
   for(int i = 0; i < countTasks;i++){\
      clock_t start = clock();\
      days = 1;\
      doShuffleTw(newCountTowns, sub);\
      l = 0;\
      g = 0;\
      cap = 0;\
      for(g = 0; g < newCountTowns; g++) { \
         if(cap + sub[g].t.weight <= maxCapacity && g != newCountTowns - 1) {\
            temp[l] = sub[g];\
            l++;\
            cap += sub[g].t.weight;\
         } else {\
            if(g == newCountTowns - 1){\
               temp[l] = sub[g];\
               l++;\
            }\
            if(l >= 3) {\
               td = algfunc(temp, l, &m, &timer, endTime);  \
               if(td == -1){\
                  days++;\
               }\
               while(td == -1) {\
                  /*printf("g:%d l:%d\n", g, l);\
                  printTwTownList(sub, l);\
                  putchar('\n');*/\
                  timer = town0.mTimeStart;\
                  /*td = algfunc(temp, l, &m, &timer, endTime);*/\
                  if(l >= 3)\
                  {\
                     td = algfunc(temp, l, &m, &timer, endTime);\
                  } else {\
                     td = subtourdistanceTw(temp, l, &m, timer, endTime);\
                  }\
                  /*if(td == -1) {write_cvrptw_end_tour(res_distance, -1); continue;}*/\
                  if(td == -1) {l--; g--;}\
               }\
               write_cvrptw_subtour(res_distance, temp, l); \
               distanceInTourNew += td;\
            } else {\
               td = subtourdistanceTw(temp, l, &m, timer, endTime);\
               if(td == -1){\
                  days++;\
               }\
               while(td == -1) {\
                  /*printf("g:%d l:%d\n", g, l);\
                  printTwTownList(sub, l);\
                  putchar('\n');*/\
                  timer = town0.mTimeStart;\
                  /*td = algfunc(temp, l, &m, &timer, endTime);*/\
                  if(l >= 3)\
                  {\
                     td = algfunc(temp, l, &m, &timer, endTime);\
                  } else {\
                     td = subtourdistanceTw(temp, l, &m, timer, endTime);\
                  }\
                  /*if(td == -1) {write_cvrptw_end_tour(res_distance, -1); continue;}*/\
                  if(td == -1) {l--; g--;}\
               }\
               write_cvrptw_subtour(res_distance, temp, l); \
               distanceInTourNew += td;\
            }\
            l = 0;cap = 0;\
           /* g--;*/\
         }\
      }\
      if(distanceInTourBest == -1.0) {\
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew), 0.0);\
         distanceInTourBest = distanceInTourNew;\
      } \
      if(distanceInTourNew < distanceInTourBest) {\
         distanceInTourBest = distanceInTourNew;\
         printf("\nAll days: %d %lf\n", days, distanceInTourBest); \
         write_cvrptw_end_tour(res_distance, (distanceInTourBest));\
         fprintf(out, "%lf\t%lf\n", (distanceInTourBest), (clock() - runtime) / CLOCKS_PER_SEC);\
      }\
      else {\
         write_cvrptw_end_tour(res_distance, -1);\
      }\
      distanceInTourNew = 0.0;\
      clock_t end = clock();\
      double seconds = (double)(end - start) / CLOCKS_PER_SEC;\
      full_time += seconds;\
   }\
   printf("Full time: %lf\n", full_time);\
   printf("Average time on one task: %lf\n", full_time / countTasks);\
   fprintf(out, "%lf\t%lf\n", (distanceInTourBest), (clock() - runtime) / CLOCKS_PER_SEC);\
   fputc('\n', out);\
   free(sub);\
   free(towns);\
   fclose(out);\
   fclose(res_distance);\
   finalizehalfmatrix(&m);
   
static PyObject *modelMetaHeuristic(PyObject *self, PyObject *args) {
   char *in, *algname;
   int tcountTown; 
   double maxCapacity;
   double countTasks;

   if (!PyArg_ParseTuple(args, "ssidd", &algname, &in, &tcountTown, &maxCapacity, &countTasks)) {
      return NULL;
   }
   countTowns = tcountTown;
   if(strcmp(algname, "cvrptw_lkh") == 0) {
      char fileout[] = "LKH_CVRPTW_result.txt";
      CVRPTW(lkhTw); 
   } 
   else if(strcmp(algname, "cvrptw_lkh_2opt") == 0) {
      char fileout[] = "LKH_2opt_CVRPTW_result.txt";
      CVRPTW(lkh2optTw); 
   } 
   else if(strcmp(algname, "cvrptw_lkh_3opt") == 0) {
      char fileout[] = "LKH_3opt_CVRPTW_result.txt";
      CVRPTW(lkh3optTw); 
   } 
   else if(strcmp(algname, "cvrptw_sa") == 0) {
      char fileout[] = "SA_CVRPTW_result.txt";
      CVRPTW(saTw);
   } else {
      printf("Error algname: %s\n", algname);
      exit(-1);
   }
   return Py_BuildValue("f", "Hello, Python extensions!!");
};

static char helloworld_docs1[] = 
   "Для просмотра документации к программе необходимо:\n"
   "  Если Вы работает в ОС Windows:\n"
   "     1. Открыть Проводник и в пути до папки с файлами написать: cmd. Автоматически должна открыть командная строка с полным путем до основной папки.\n"
   "     2. Написать в командной строке команду: start sphinx/_build/html/index.html .\n";

static char helloworld_docs2[] = 
   "  Если Вы работает в ОС Linux:\n"
   "     1. Прописать в командной строке полный путь до папки с файлами.\n"
   "     2. Написать команду: open sphinx\\_build\\html\\index.html .\n" ;
static char helloworld_docs3[] = 
   "  После чего автоматически откроется документация в одном из Ваших браузеров на локальном компьютере.\n"
   "Для выхода из файла для помощи доступа к документации введите команду: :wq .";
static PyMethodDef helloworld_funcs[] = { //TODO: Как сделать нормально переименование????
   // {"parseOneTownPy", (PyCFunction)parseOneTownPy,
   //    METH_VARARGS, helloworld_docs2},
   {"parseOneTwTownPy", (PyCFunction)parseOneTwTownPy,
      METH_VARARGS, helloworld_docs3},
   {"modelMetaHeuristic", (PyCFunction)modelMetaHeuristic,
      METH_VARARGS, helloworld_docs1},
   {NULL}
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