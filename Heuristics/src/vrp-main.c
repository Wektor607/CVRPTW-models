#include <Python.h>
#include <time.h> 
#include <signal.h>
// #include "cvrptw-logistic.c"
#include "logistic.h"
#include "supporting_functions/townActions.c"
#include "supporting_functions/dataProcessing.c"
#include "algorithms/SA.c"
#include "algorithms/twoOpt.c"
#include "algorithms/threeOpt.c"
#include "algorithms/LKH.c"
// #include "logistic.h"

static PyObject *parseOneTwTownPy(PyObject *self, PyObject *args) {
   char *in;
   char *out;
   int tcountTown;

   if (!PyArg_ParseTuple(args, "ssi", &in, &out, &tcountTown)) {
      return NULL;
   }

   parseOneTwTownNoIndex(in, out, tcountTown);
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

volatile sig_atomic_t stop;

void sigfunc(int sig){
   char c;
   printf("\nХотите прекратить оптимизацию маршрута (y/n):");
   while((c=getchar()) != 'y'){
      return;
   }
   stop = 1;
}
#define CVRPTW(algfunc) \
   srand(time(NULL)); \
   FILE *out = fopen(fileout, "w"); \
   FILE *res_distance = fopen("current_result/res_distance.txt", "w");\
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
   twtown *sub = (twtown*) malloc((countTowns-1) * sizeof(twtown));\
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
   twtown temp[newCountTowns+1];\
   twtown** full_temp = calloc(newCountTowns+1, sizeof(twtown*));\
   int* len_full_temp = calloc(newCountTowns+1, sizeof(int));\
   int n_temp = 0;\
   double td;\
   double distanceInTourBest = -1.0, distanceInTourNew = 0.0;\
   double runtime = clock();\
   int days, cap, l, g;\
   double full_time = 0;\
   signal(SIGINT, sigfunc);\
   /*doShuffleTw(newCountTowns, sub);*/\
   l = 1;\
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
         temp[0] = town0;\
         td = subtourdistanceTw(temp, l, &m, timer, endTime);\
         while(td == -1) {\
            timer = town0.mTimeStart;\
            td = subtourdistanceTw(temp, l, &m, timer, endTime);\
            if(td == -1) {l--; g--;}\
         }\
         full_temp[n_temp] = calloc(l, sizeof(twtown));\
         for(int t = 0; t < l; ++t){\
            full_temp[n_temp][t] = temp[t];\
         }\
         len_full_temp[n_temp] = l;\
         write_cvrptw_subtour(res_distance, temp, l); \
         distanceInTourNew += td;\
         n_temp += 1;\
         l = 1;cap = 0;\
      }\
   }\
   printf("\nSTART_LEN: %lf\n[", distanceInTourNew);\
   /*for(int num = 0; num < n_temp; ++num){\
      printf("[");\
      for(int t = 0; t < len_full_temp[num]; ++t){\
         printf("%d ", full_temp[num][t].t.name);\
      }\
      printf("]\n");\
      printf("len_full_temp: %d", len_full_temp[num]);\
   }\
   printf("]\n");\*/\
   fprintf(out, "%lf\t%lf\n", (distanceInTourNew), 0.0);\
   distanceInTourNew = 0;\
   printf("newCountTowns: %d", newCountTowns);\
   while(!stop){\
      clock_t start = clock();\
      /*printf("countTaks: %d\n", i);\*/\
      days = 1;\
      doShuffleTw(newCountTowns, sub);\
      l = 1;\
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
            temp[0] = town0;\
            printf("l: %d", l);\
            if(l >= 3) {\
               td = algfunc(temp, l, &m, &timer, endTime, T, t_end);  \
            } else {\
               td = subtourdistanceTw(temp, l, &m, timer, endTime);\
            }\
            if(td == -1){\
               days++;\
            }\
            while(td == -1) {\
               timer = town0.mTimeStart;\
               if(l >= 3)\
               {\
                  td = algfunc(temp, l, &m, &timer, endTime, T, t_end);\
               } else {\
                  td = subtourdistanceTw(temp, l, &m, timer, endTime);\
               }\
               if(td == -1) {l--; g--;}\
            }\
            write_cvrptw_subtour(res_distance, temp, l); \
            distanceInTourNew += td;\
            l = 1;cap = 0;\
         }\
      }\
      if(distanceInTourBest == -1.0) {\
      /*distanceInTourNew * 60 * 1000 / 3600*/\
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew), (clock() - runtime) / CLOCKS_PER_SEC);\
         distanceInTourBest = distanceInTourNew;\
      } \
      if(distanceInTourNew < distanceInTourBest) {\
         distanceInTourBest = distanceInTourNew;\
         printf("\nAll days: %d %lf\n", days, distanceInTourBest); \
         write_cvrptw_end_tour(res_distance, distanceInTourBest);\
         /*distanceInTourBest * 60 * 1000 / 3600*/\
         fprintf(out, "%lf\t%lf\n", (distanceInTourBest), (clock() - runtime) / CLOCKS_PER_SEC);\
      }\
      else {\
         write_cvrptw_end_tour(res_distance, -1);\
      }\
      distanceInTourNew = 0.0;\
      clock_t end = clock();\
      double seconds = (double)(end - start) / CLOCKS_PER_SEC;\
      full_time += seconds;\
      if(!stop)\
         printf("Время оптимизации: %lf Текущая длина: %lf \n", full_time, distanceInTourBest);\
   }\
   /* данный параметр очень важно заново обнулять, так как он глобальный и при решении следующих задач
   будет сразу вызывать Ctrl+C*/\
   stop = 0;\
   /*distanceInTourBest * 60 * 1000 / 3600*/\
   double final_time = (clock() - runtime) / CLOCKS_PER_SEC;\
   fprintf(out, "%lf\t%lf\n", (distanceInTourBest), final_time);\
   printf("\nОкончательное время оптимизации: %lf \nОкончательная длина маршрута: %lf \n", final_time, distanceInTourBest);\
   fputc('\n', out);\
   free(sub);\
   free(towns);\
   fclose(out);\
   fclose(res_distance);\
   finalizehalfmatrix(&m);
   
static PyObject *modelMetaHeuristic(PyObject *self, PyObject *args) {
   char *in, *algname;
   int tcountTown, countTowns; 
   double maxCapacity;
   double T, t_end;

   if (!PyArg_ParseTuple(args, "ssiddd", &algname, &in, &tcountTown, &maxCapacity, &T, &t_end)) {
      return NULL;
   }
   countTowns = tcountTown;
   if(strcmp(algname, "cvrptw_lkh") == 0) {
      char fileout[] = "current_result/LKH_CVRPTW_result.txt";
      CVRPTW(lkhTw); 
   } 
   else if(strcmp(algname, "cvrptw_2opt") == 0) {
      char fileout[] = "current_result/2opt_CVRPTW_result.txt";
      CVRPTW(lkh2optTw); 
   } 
   else if(strcmp(algname, "cvrptw_3opt") == 0) {
      char fileout[] = "current_result/3opt_CVRPTW_result.txt";
      CVRPTW(lkh3optTw); 
   } 
   else if(strcmp(algname, "cvrptw_sa") == 0) {
      char fileout[] = "current_result/SA_CVRPTW_result.txt";
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