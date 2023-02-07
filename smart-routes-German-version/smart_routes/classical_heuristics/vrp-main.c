#include <Python.h>
#include <time.h> 
#include <signal.h>
#include <stdio.h> 
#include <stdlib.h>
#include "algorithms/LKH.h"
#include "algorithms/threeOpt.h"
#include "algorithms/twoOpt.h"
#include "algorithms/SA.h"

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
 
void separate_array(twtown arr[], int n, twtown *arr1, twtown *arr2, int *n1, int *n2, const int maxCapacity, const double startTime, const double endTime, halfmatrix* m) 
{ 
   int i; 
   int cap = 0;
   double time = startTime;
   for (i = 0; i < n - 1; i++) 
   {
      if(cap + arr[i].t.weight <= maxCapacity)
      {
         arr1[i+1] = arr[i]; 
         cap += arr[i].t.weight;
      }
      else
      {
         break;
      }
      
   } 
   
   *n1 = i + 1; 
   int index = i;

   for (; i < n - 1; i++) 
   { 
      arr2[i - index] = arr[i]; 
   } 

   *n2 = n - 1 - index; 
} 

void CVRPTW(struct twoResults (*algfunc)(twtown*, int , halfmatrix*, double*, const double, double, double, int), char *in, int tcountTown, double maxCapacity, double T, double t_end, int shuffle_param, char *fileout, int countTowns)
{
   FILE *out = fopen(fileout, "w"); 
   FILE *res_distance = fopen("classical_heuristics/current_result/res_distance.txt", "w");
   if(res_distance == NULL) {exit(-1);}
   if(out == NULL) {exit(-1);}
   twtown *towns; 
   towns = malloc(tcountTown * sizeof(twtown));
   halfmatrix m; 
   readOneTwTownByBinaryNoIndex(towns, &m, in); 
   printhalfmatrix(&m);
   twtown town0 = getTwTownByName(0, countTowns, towns);
   double startTime = town0.mTimeStart;
   double endTime = town0.mTimeEnd;
   
   printf("\n");
   for(int c = 0; c < countTowns; c++)
   {
      if((town0.mTimeStart < town0.mTimeEnd && town0.mTimeStart > towns[c].mTimeEnd && towns[c].mTimeEnd > towns[c].mTimeStart) || 
           (town0.mTimeEnd < towns[c].mTimeStart && town0.mTimeStart > towns[c].mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd && town0.mTimeEnd > town0.mTimeStart))
      {
         towns[c].t = zerotown;
      }
      else if((town0.mTimeStart > towns[c].mTimeStart && town0.mTimeStart < towns[c].mTimeEnd && town0.mTimeEnd > towns[c].mTimeEnd) || 
         (towns[c].mTimeStart < town0.mTimeStart && towns[c].mTimeEnd < town0.mTimeEnd && town0.mTimeStart > town0.mTimeEnd))
      {
         towns[c].mTimeStart = town0.mTimeStart;
      }
      else if(towns[c].mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart < town0.mTimeStart && towns[c].mTimeEnd > town0.mTimeEnd)
      {
         towns[c].mTimeStart = town0.mTimeStart;
         towns[c].mTimeEnd = town0.mTimeEnd;
      }
      else if((town0.mTimeEnd > towns[c].mTimeStart && town0.mTimeStart < towns[c].mTimeEnd && town0.mTimeEnd < town0.mTimeStart) || 
         (town0.mTimeStart < towns[c].mTimeStart && town0.mTimeEnd > towns[c].mTimeEnd && towns[c].mTimeEnd < towns[c].mTimeStart && town0.mTimeEnd < town0.mTimeStart) || 
         (town0.mTimeStart < towns[c].mTimeEnd && towns[c].mTimeStart > town0.mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd) || 
         (town0.mTimeStart < towns[c].mTimeEnd && towns[c].mTimeStart < town0.mTimeEnd && towns[c].mTimeStart > towns[c].mTimeEnd) || 
         (town0.mTimeStart > towns[c].mTimeEnd && town0.mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart > towns[c].mTimeEnd) || 
         (towns[c].mTimeEnd > towns[c].mTimeStart && town0.mTimeEnd > towns[c].mTimeEnd && town0.mTimeStart > town0.mTimeEnd) || 
         (towns[c].mTimeEnd > towns[c].mTimeStart && towns[c].mTimeStart > town0.mTimeStart && towns[c].mTimeEnd > town0.mTimeEnd))
      {
         continue;
      }
      if(towns[c].mTimeService > towns[c].mTimeEnd - towns[c].mTimeStart){
         towns[c].t = zerotown;
      }
      if(towns[c].t.weight > maxCapacity || (towns[c].mTimeStart - towns[c].mTimeEnd) == 0) {
         towns[c].t = zerotown;
      }
      if(c != 0 && towns[c].mTimeEnd < getByTown(&m, 0, c) + towns[c].mTimeService)
      {
         towns[c].t = zerotown;
      }
      if(c != 0 && ((getByTown(&m, 0, c) < towns[c].mTimeStart && towns[c].mTimeStart + towns[c].mTimeService + getByTown(&m, 0, c) > town0.mTimeEnd) || (getByTown(&m, 0, c) >= towns[c].mTimeStart && towns[c].mTimeService + 2 * getByTown(&m, 0, c) > town0.mTimeEnd)))
      {
         towns[c].t = zerotown;
      }
      if(town0.mTimeStart - town0.mTimeEnd == 0)
      {
         printf("Impossible to optimize tour");
         exit(-1);
      }
   }
   // printtwtown(towns[1]);
   twtown *sub = (twtown*) malloc((countTowns-1) * sizeof(twtown));
   int w = 0;
   twtown t;
   // считаем количество плохих городов
   int errorCounter = 0;
   for(int i = 1; i < countTowns; i++)
   {
      t = getTwTownByName(i, countTowns, towns);
      if(t.t.name == -1) 
      {
         errorCounter++;
         printf("Error town: %d\n", i); 
         continue;
      }
      sub[w] = t;
      w++;
   }
   int newCountTowns = w;
   printf("%d\n", newCountTowns);
   sub = realloc(sub, newCountTowns * sizeof(twtown));

   double td;
   double distanceInTourBest = -1.0, distanceInTourNew = 0.0;
   double runtime = clock();
   int days, cap, l, g;
   double full_time = 0;
   signal(SIGINT, sigfunc);

   if(shuffle_param == 0)
   {
      FILE *start_tour = fopen("classical_heuristics/current_result/start_tour.txt", "w");
      doShuffleTw(newCountTowns, sub);
      for(int h = 0; h < newCountTowns; h++){
         fprintf(start_tour, "%d ", sub[h].t.name);
      }
      fclose(start_tour);
   } 
   else if(shuffle_param == 1)
   {
      FILE *start_tour = fopen("classical_heuristics/current_result/start_tour.txt", "r");
      int name;
      twtown *sub_alg = (twtown*) malloc((newCountTowns) * sizeof(twtown));
      for(int h = 0; h < newCountTowns; h++){
         fscanf(start_tour, "%d ", &name);
         for(int s = 0; s < newCountTowns; s++){
            if(name == sub[s].t.name){
               sub_alg[h] = sub[s];
               break;
            }
         }
      }
      fclose(start_tour);
      for(int h = 0; h < newCountTowns; h++){
         sub[h] = sub_alg[h];
      }
   }

   g = newCountTowns + 1;

   td = -1;

   int len_temp = 0;
   twtown *temp = (twtown *)calloc(g, sizeof(twtown));
   twtown *other_temp = (twtown *)calloc(g, sizeof(twtown));
   twtown *addition_other_temp = (twtown *)calloc(g, sizeof(twtown));
   int len_other_temp = 0;

   struct twoResults tr;
   
   while(g > 1) 
   {
      if(g == newCountTowns + 1)
      {
         separate_array(sub, g, temp, other_temp, &len_temp, &len_other_temp, maxCapacity, startTime, endTime, &m);
      }
      else
      {
         for(int i = 0; i < g; i++)
         {
            addition_other_temp[i] = other_temp[i];
         }
         separate_array(addition_other_temp, g, temp, other_temp, &len_temp, &len_other_temp, maxCapacity, startTime, endTime, &m);
      }

      temp[0] = town0;
      
      while(td == -1) 
      {
         tr = subtourdistanceTw(temp, len_temp, &m, startTime, endTime);
         td = tr.localtimer;
         if(td == -1)
         {  
            if(len_temp == 1 && temp[len_temp - 1].t.name == 0)
            {
               break;
            }
            len_temp--;
            other_temp[len_other_temp] = temp[len_temp];
            len_other_temp++;
            
            startTime = town0.mTimeStart;
         }

      }

      startTime += td;
      
      if(len_temp > 1)
      {
         write_cvrptw_subtour(res_distance, temp, len_temp); 
         distanceInTourNew += tr.only_distance;
      }
   
      td = -1;
      g = len_other_temp + 1;

   }

   fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + errorCounter * 10), 0.0);
   
   while(!stop)
   {
      clock_t start = clock();
      days = 1;
      g = newCountTowns + 1;
      
      startTime = town0.mTimeStart;
      td = -1;
      
      distanceInTourNew = 0;
      tr.localtimer = 0;
      tr.only_distance = 0;

      int len_temp = 0;
      int len_other_temp = 0;

      while(g > 1)
      { 
         if(g == newCountTowns + 1)
         {
            separate_array(sub, g, temp, other_temp, &len_temp, &len_other_temp, maxCapacity, startTime, endTime, &m);
         }
         else
         {
            for(int i = 0; i < len_other_temp; i++)
            {
               addition_other_temp[i] = other_temp[i];
            }
            separate_array(addition_other_temp, g, temp, other_temp, &len_temp, &len_other_temp, maxCapacity, startTime, endTime, &m);
         }
         temp[0] = town0;

         while(td == -1) 
         {
            if(len_temp > 2) 
            {
               tr = algfunc(temp, len_temp, &m, &startTime, endTime, T, t_end, countTowns);
            } 
            else 
            {
               tr = subtourdistanceTw(temp, len_temp, &m, startTime, endTime);
            }
            td = tr.localtimer;
            if(td == -1)
            {  
               len_temp--;
               other_temp[len_other_temp] = temp[len_temp];
               len_other_temp++;
               
               startTime = town0.mTimeStart;
            }

         }

         startTime += td;
         
         if(len_temp > 1)
         {
            write_cvrptw_subtour(res_distance, temp, len_temp); 
            distanceInTourNew += tr.only_distance;
         }
         
         td = -1;
         g = len_other_temp + 1;
         
      }
      
      if(distanceInTourBest == -1.0) 
      {
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + errorCounter * 10), (clock() - runtime) / CLOCKS_PER_SEC);
         distanceInTourBest = distanceInTourNew;
      } 
      
      if(distanceInTourNew < distanceInTourBest) 
      {
         distanceInTourBest = distanceInTourNew;
         write_cvrptw_end_tour(res_distance, distanceInTourBest);
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + errorCounter * 10), (clock() - runtime) / CLOCKS_PER_SEC);
      }
      else 
      {
         write_cvrptw_end_tour(res_distance, -1);
      }
      
      clock_t end = clock();
      double seconds = (double)(end - start) / CLOCKS_PER_SEC;
      full_time += seconds;
      // 100 секунд - 50 городов
      // 200 секунд - 100 городов
      if(full_time > 10)
      {
         stop = 1;
      }
      // break;
      doShuffleTw(newCountTowns, sub);
   }
   /* данный параметр очень важно заново обнулять, так как он глобальный и при решении следующих задач
   будет сразу вызывать Ctrl+C*/
   stop = 0;
   double final_time = (clock() - runtime) / CLOCKS_PER_SEC;
   fprintf(out, "%lf\t%lf\n", (distanceInTourBest * 13 + errorCounter * 10), final_time);
   printf("2Opt:%d 3Opt:%d KOpt:%d\n", two_opt, three_opt, k_opt);
   two_opt = 0;
   three_opt = 0;
   k_opt = 0;
   printf("\nОкончательное время оптимизации: %lf \nОкончательная длина маршрута: %lf \n", final_time, (distanceInTourBest * 13 + errorCounter * 10));
   
   fputc('\n', out);
   free(sub);
   free(towns);
   fclose(out);
   fclose(res_distance);
   finalizehalfmatrix(&m);
}

static PyObject *modelMetaHeuristic(PyObject *self, PyObject *args) {
   char *in, *algname;
   int tcountTown, countTowns; 
   double maxCapacity;
   double T, t_end;
   int shuffle_param;

   if (!PyArg_ParseTuple(args, "ssidddi", &algname, &in, &tcountTown, &maxCapacity, &T, &t_end, &shuffle_param)) {
      return NULL;
   }

   countTowns = tcountTown;
   
   if(strcmp(algname, "cvrptw_lkh") == 0) {
      char fileout[] = "classical_heuristics/current_result/LKH_CVRPTW_result.txt";
      CVRPTW(lkhTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns); 
   } 
   else if(strcmp(algname, "cvrptw_2opt") == 0) {
      char fileout[] = "classical_heuristics/current_result/2opt_CVRPTW_result.txt";
      CVRPTW(lkh2optTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns);  
   } 
   else if(strcmp(algname, "cvrptw_3opt") == 0) {
      char fileout[] = "classical_heuristics/current_result/3opt_CVRPTW_result.txt";
      CVRPTW(lkh3optTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns);  
   } 
   else if(strcmp(algname, "cvrptw_sa") == 0) {
      char fileout[] = "classical_heuristics/current_result/SA_CVRPTW_result.txt";
      CVRPTW(saTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns); 
   } else {
      printf("Error algname: %s\n", algname);
      exit(-1);
   }
   return Py_BuildValue("f", "Hello, Python extensions!!");
};

static PyMethodDef main_funcs[] = { //TODO: Как сделать нормально переименование????
   {"parseOneTwTownPy", (PyCFunction)parseOneTwTownPy,
      METH_VARARGS},
   {"modelMetaHeuristic", (PyCFunction)modelMetaHeuristic,
      METH_VARARGS},
   {NULL}
};

static struct PyModuleDef cModPyDem =
{
   PyModuleDef_HEAD_INIT,
   "VRP", /* name of module */
   "Extension module example!",          /* module documentation, may be NULL */
   -1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   main_funcs
};

PyMODINIT_FUNC PyInit_vrp_c(void)
{
    return PyModule_Create(&cModPyDem);
}
