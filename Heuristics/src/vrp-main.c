#include <Python.h>
#include <time.h> 
#include <signal.h>
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

void CVRPTW(double (*algfunc)(twtown*, int , halfmatrix*, double*, const double, double, double, int, int), char *in, int tcountTown, double maxCapacity, double T, double t_end, int shuffle_param, char *fileout, int countTowns, int dist_param){
   srand(time(NULL)); 
   FILE *out = fopen(fileout, "w"); 
   FILE *res_distance = fopen("current_result/res_distance.txt", "w");
   if(res_distance == NULL) {exit(-1);}
   if(out == NULL) {exit(-1);}
   twtown *towns; 
   towns = malloc(tcountTown * sizeof(twtown));
   halfmatrix m; 
   readOneTwTownByBinaryNoIndex(towns, &m, in); 
   twtown town0 = getTwTownByName(0, countTowns, towns);
   double timer = town0.mTimeStart;
   double endTime = town0.mTimeEnd;
   // printTwTownList(towns, tcountTown);
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
   for(int i = 1; i < countTowns; i++){
      t = getTwTownByName(i, countTowns, towns);
      if(t.t.name == -1) {printf("Error town: %d\n", i); continue;}
      sub[w] = t;
      w++;
   }
   int newCountTowns = w;
   sub = realloc(sub, newCountTowns * sizeof(twtown));
   // printf("sub: ");
   // for(int i = 0; i < newCountTowns; i++) {
   //    printf("%d ", sub[i].t.name);
   // }
   // putchar('\n');
   // printtwtown(sub[1]);
   twtown temp[newCountTowns+1];
   twtown** full_temp = calloc(newCountTowns+1, sizeof(twtown*));
   int* len_full_temp = calloc(newCountTowns+1, sizeof(int));
   int n_temp = 0;
   double td;
   double distanceInTourBest = -1.0, distanceInTourNew = 0.0;
   double runtime = clock();
   int days, cap, l, g;
   double full_time = 0;
   signal(SIGINT, sigfunc);

   if(shuffle_param == 0){
      FILE *start_tour = fopen("current_result/start_tour.txt", "w");
      doShuffleTw(newCountTowns, sub);
      for(int h = 0; h < newCountTowns; h++){
         fprintf(start_tour, "%d ", sub[h].t.name);
      }
      fclose(start_tour);
   } else if(shuffle_param == 1){
      FILE *start_tour = fopen("current_result/start_tour.txt", "r");
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
   // printf("SHUFFLE_param: %d\n", shuffle_param);
   // printf("SHUFFLE_sub: ");
   
   // for(int i = 0; i < newCountTowns; i++) {
   //    printf("%d ", sub[i].t.name);
   // }

   l = 1;
   g = 0;
   cap = 0;
   for(g = 0; g < newCountTowns; g++) { 
      if(cap + sub[g].t.weight <= maxCapacity && g != newCountTowns - 1) {
         temp[l] = sub[g];
         l++;
         cap += sub[g].t.weight;
      } else {
         if(g == newCountTowns - 1){
            temp[l] = sub[g];
            l++;
         }
         temp[0] = town0;

         td = subtourdistanceTw(temp, l, &m, timer, endTime);
         timer += td;
         
         while(td == -1) 
         {
            timer = town0.mTimeStart;
            td = subtourdistanceTw(temp, l, &m, timer, endTime);
            timer += td;
            if(td == -1) {l--; g--;}
         }
         
         full_temp[n_temp] = calloc(l, sizeof(twtown));
         for(int t = 0; t < l; ++t){
            full_temp[n_temp][t] = temp[t];
         }
         len_full_temp[n_temp] = l;
         write_cvrptw_subtour(res_distance, temp, l); 
         distanceInTourNew += td;
         n_temp += 1;
         l = 1;cap = 0;
      }
   }
   // printf("\nSTART_LEN: %lf\n[", distanceInTourNew);
   fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 60 * 1000 / 3600), 0.0);
   double start_tour_len = distanceInTourNew;
   distanceInTourNew = 0;
   // printf("newCountTowns: %d", newCountTowns);
   while(!stop){
      clock_t start = clock();
      days = 1;
      l = 1;
      g = 0;
      cap = 0;
      timer = town0.mTimeStart;
      for(g = 0; g < newCountTowns; g++) { 
         if(cap + sub[g].t.weight <= maxCapacity && g != newCountTowns - 1) {
            temp[l] = sub[g];
            l++;
            cap += sub[g].t.weight;
         } else {
            if(g == newCountTowns - 1){
               temp[l] = sub[g];
               l++;
            }
            temp[0] = town0;
            
            if(l >= 3) {
               td = algfunc(temp, l, &m, &timer, endTime, T, t_end, countTowns, dist_param);  
            } else {
               td = subtourdistanceTw(temp, l, &m, timer, endTime);
            }
            timer += td;
            if(td == -1){days++;}

            while(td == -1) 
            {
               timer = town0.mTimeStart;
               if(l >= 3)
               {
                  td = algfunc(temp, l, &m, &timer, endTime, T, t_end, countTowns, dist_param);
               } else {
                  td = subtourdistanceTw(temp, l, &m, timer, endTime);
               }
               timer += td;
               if(td == -1) {l--; g--;}
            }
            write_cvrptw_subtour(res_distance, temp, l); 
            distanceInTourNew += td;
            l = 1;cap = 0;
         }
      }
      if(distanceInTourBest == -1.0) {
      /*distanceInTourNew * 60 * 1000 / 3600*/
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 60 * 1000 / 3600), (clock() - runtime) / CLOCKS_PER_SEC);
         distanceInTourBest = distanceInTourNew;
      } 
      if(distanceInTourNew < distanceInTourBest) {
         distanceInTourBest = distanceInTourNew;
         // printf("\nAll days: %d %lf\n", days, distanceInTourBest); 
         write_cvrptw_end_tour(res_distance, distanceInTourBest);
         /*distanceInTourBest * 60 * 1000 / 3600*/
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 60 * 1000 / 3600), (clock() - runtime) / CLOCKS_PER_SEC);
      }
      else {
         write_cvrptw_end_tour(res_distance, -1);
      }
      distanceInTourNew = 0.0;
      clock_t end = clock();
      double seconds = (double)(end - start) / CLOCKS_PER_SEC;
      full_time += seconds;
      if(full_time > 20)
      {
         stop = 1;
      }
      // printf("Печать перемешанного тура: ");
      // for(int i = 0; i < newCountTowns; i++) {
      //    printf("%d ", sub[i].t.name);
      // }
      // putchar('\n');
      doShuffleTw(newCountTowns, sub);
   }
   /* данный параметр очень важно заново обнулять, так как он глобальный и при решении следующих задач
   будет сразу вызывать Ctrl+C*/
   stop = 0;
   /*distanceInTourBest * 60 * 1000 / 3600*/
   double final_time = (clock() - runtime) / CLOCKS_PER_SEC;
   fprintf(out, "%lf\t%lf\n", (distanceInTourBest * 60 * 1000 / 3600), final_time);
   printf("\nОкончательное время оптимизации: %lf \nОкончательная длина маршрута: %lf \n", final_time, distanceInTourBest);
   printf("\nНачальная длина маршрута: %lf\n", start_tour_len);
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
   int dist_param;

   if (!PyArg_ParseTuple(args, "ssidddii", &algname, &in, &tcountTown, &maxCapacity, &T, &t_end, &shuffle_param, &dist_param)) {
      return NULL;
   }

   countTowns = tcountTown;
   
   if(strcmp(algname, "cvrptw_lkh") == 0) {
      char fileout[] = "current_result/LKH_CVRPTW_result.txt";
      CVRPTW(lkhTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns, dist_param); 
   } 
   else if(strcmp(algname, "cvrptw_2opt") == 0) {
      char fileout[] = "current_result/2opt_CVRPTW_result.txt";
      CVRPTW(lkh2optTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns, dist_param);  
   } 
   else if(strcmp(algname, "cvrptw_3opt") == 0) {
      char fileout[] = "current_result/3opt_CVRPTW_result.txt";
      CVRPTW(lkh3optTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns, dist_param);  
   } 
   else if(strcmp(algname, "cvrptw_sa") == 0) {
      char fileout[] = "current_result/SA_CVRPTW_result.txt";
      CVRPTW(saTw, in, tcountTown, maxCapacity, T, t_end, shuffle_param, fileout, countTowns, dist_param); 
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
