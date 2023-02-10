#include <Python.h>
#include <time.h> 
#include <signal.h>
#include <limits.h>
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

#define MAX_ITERATIONS 100
// #define NUM_POINTS  5
#define NUM_CLUSTERS 10
#define DIMENSIONS 2 
 
double euclidean_distance(twtown point1, double point2[DIMENSIONS]) 
{ 
   double distance = pow(point1.t.x - point2[0], 2) + pow(point1.t.y - point2[1], 2);  
   return sqrt(distance); 
} 
 
void initialize_cluster_centers(int lenSub, twtown* sub, double** cluster_centers) 
{ 
   int j;

   for (int i = 0; i < NUM_CLUSTERS; i++) 
   { 
      cluster_centers[i][0] = sub[i].t.x;
      cluster_centers[i][1] = sub[i].t.y; 
   } 
} 
 
void update_cluster_assignments(twtown* sub, int lenSub, double** distance_matrix, int* cluster_assignments, int maxCapacity) { 
   int cap[NUM_CLUSTERS] = {0};

   for (int i = 0; i < lenSub; i++) 
   { 
      double min_distance = distance_matrix[i][0]; 
      int closest_cluster = 0; 
      for (int j = 1; j < NUM_CLUSTERS; j++) 
      { 
         if (distance_matrix[i][j] < min_distance && cap[j] < maxCapacity) 
         { 
            min_distance = distance_matrix[i][j]; 
            closest_cluster = j;
         } 
      }
      
      cap[closest_cluster] += sub[i].t.weight;
      cluster_assignments[i] = closest_cluster; 
   } 

} 
 
int update_cluster_centers(int lenSub, twtown* sub, int* cluster_assignments, double** cluster_centers, int* cnt) { 
   int cluster_sizes[NUM_CLUSTERS] = {0}; 
   double cluster_sum[NUM_CLUSTERS][DIMENSIONS] = {{0}};
   double tmp_centers[NUM_CLUSTERS][DIMENSIONS] = {{0}};
   
   for (int i = 0; i < NUM_CLUSTERS; i++) 
   {
      for (int j = 0; j < DIMENSIONS; j++) 
      { 
         tmp_centers[i][j] = cluster_centers[i][j];
      }
   }
   
   for (int i = 0; i < lenSub; i++) 
   { 
      int cluster = cluster_assignments[i]; 
      cluster_sizes[cluster]++; 

      cluster_sum[cluster][0] += sub[i].t.x;
      cluster_sum[cluster][1] += sub[i].t.y; 
   } 

   for (int i = 0; i < NUM_CLUSTERS; i++) 
   {
      for (int j = 0; j < DIMENSIONS; j++) 
      { 
         cluster_centers[i][j] = cluster_sum[i][j] / cluster_sizes[i]; 
      }
   } 

   int stop_flag = 1;
   for (int i = 0; i < NUM_CLUSTERS; i++) 
   {
      for (int j = 0; j < DIMENSIONS; j++) 
      { 
         if (fabs(cluster_centers[i][j] - tmp_centers[i][j]) > 0.00001)
         {
            stop_flag = 0;
            break;
         }
      }
      if (!stop_flag)
      {
         break;
      }
   } 
   return stop_flag;
} 
 
void run_kmeans(twtown** routes, int lenSub, twtown* sub, double** distance_matrix, double** cluster_centers, int* cluster_assignments, int maxCapacity, twtown town0, int* cnt) 
{  
   initialize_cluster_centers(lenSub-1, sub, cluster_centers);
   for (int i = 0; i < MAX_ITERATIONS; i++) 
   { 
      for (int j = 0; j < lenSub-1; j++) 
      { 
         for (int k = 0; k < NUM_CLUSTERS; k++) 
         { 
            distance_matrix[j][k] = euclidean_distance(sub[j], cluster_centers[k]);
         } 
      } 
      
      update_cluster_assignments(sub, lenSub-1, distance_matrix, cluster_assignments, maxCapacity);
      
      int stop = update_cluster_centers(lenSub-1, sub, cluster_assignments, cluster_centers, cnt);
      if(stop)
      {
         break;
      }
   }
   
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      cnt[i] = 0;
   }
   
   for (int j = 0; j < lenSub-1; j++) 
   {
      routes[cluster_assignments[j]][cnt[cluster_assignments[j]]] = sub[j]; 
      cnt[cluster_assignments[j]]++;
   } 
   
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      routes[i][cnt[i]] = town0;
      cnt[i]++;
   }

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
   int len_other_temp = 0;

   struct twoResults tr;
   
   double** distance_matrix = calloc(newCountTowns, sizeof(double *));
   for (int i=0; i < newCountTowns; i++)
   {
      distance_matrix[i] = calloc(NUM_CLUSTERS, sizeof(double));
   }  
   int* cluster_assignments = calloc(newCountTowns, sizeof(int)); 
   
   double** cluster_centers = calloc(NUM_CLUSTERS, sizeof(double *));
   for (int i=0; i < NUM_CLUSTERS; i++)
   {
      cluster_centers[i] = calloc(DIMENSIONS, sizeof(double));
   }   
   
   twtown** routes = calloc(NUM_CLUSTERS, sizeof(twtown *));
   for (int i=0; i < NUM_CLUSTERS; i++)
   {
      routes[i] = calloc((newCountTowns+1), sizeof(twtown));
   }

   int* cnt = calloc(NUM_CLUSTERS, sizeof(int));
   
   run_kmeans(routes, newCountTowns+1, sub, distance_matrix, cluster_centers, cluster_assignments, maxCapacity, town0, cnt);
   
   printf("НАЧАЛЬНЫ МАРШРУТ БЕЗ ОКОН: ");
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      printf("[");
      for(int j = 0; j < cnt[i]; j++)
      {
         printf("%d, ", routes[i][j].t.name);
      }
      printf("],\n");
   }

   bool clusters_flags[NUM_CLUSTERS] = {0};
   bool** not_return = calloc(NUM_CLUSTERS, sizeof(bool *));
   for (int i=0; i < NUM_CLUSTERS; i++)
   {
      not_return[i] = calloc(countTowns, sizeof(bool));
   }

   int err_towns = 0;
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      if(clusters_flags[i] == 1)
      {
         continue;
      }
      while(td == -1)
      {
         if(clusters_flags[i] == 1)
         {
            break;
         }
         
         tr = subtourdistanceTw(routes[i], cnt[i], &m, startTime, endTime);
         td = tr.localtimer;
         if(td == -1)
         {
            startTime = town0.mTimeStart;
            if(cnt[i] == 1 && routes[i][cnt[i] - 1].t.name == 0)
            {
               break;
            }
            cnt[i]--;
            not_return[i][routes[i][cnt[i]].t.name] = 1;

            int min_distance = INT_MAX;
            int closest_cluster = 0; 
            for(int j = 0; j < NUM_CLUSTERS; j++)
            {
               int dist = euclidean_distance(routes[i][cnt[i]], cluster_centers[j]);
               if (dist < min_distance && not_return[j][routes[i][cnt[i]].t.name] == 0) 
               { 
                  min_distance = dist; 
                  closest_cluster = j;
               }   
            }
            
            if(min_distance == INT_MAX)
            {
               err_towns++;
            }
            else
            {   
               routes[closest_cluster][cnt[closest_cluster]] = routes[i][cnt[i]];
               cnt[closest_cluster]++;
               clusters_flags[closest_cluster] = 0;
               if(closest_cluster < i)
               {
                  i = closest_cluster;
               }
            }

         }
      }

      clusters_flags[i] = 1;
     
      td = -1;
   }
   
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      startTime = town0.mTimeStart;
      tr = subtourdistanceTw(routes[i], cnt[i], &m, startTime, endTime); 
      printf("td: %lf\n", tr.localtimer);
      distanceInTourNew += tr.only_distance;
      write_cvrptw_subtour(res_distance, routes[i], cnt[i]);
   }

   printf("НАЧАЛЬНЫ МАРШРУТ С ОКНАМИ: ");
   for(int i = 0; i < NUM_CLUSTERS; i++)
   {
      
      printf("%d [", cnt[i]);
      for(int j = 0; j < cnt[i]; j++)
      {
         printf("%d, ", routes[i][j].t.name);
      }
      printf("]\n");
   }

   distanceInTourBest = distanceInTourNew * 13 + (errorCounter + err_towns) * 10;
   fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + (errorCounter + err_towns) * 10), 0.0);
   
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
      err_towns = 0;

      int len_temp = 0;
      int len_other_temp = 0;

      doShuffleTw(newCountTowns, sub);
      run_kmeans(routes, newCountTowns+1, sub, distance_matrix, cluster_centers, cluster_assignments, maxCapacity, town0, cnt);
      
      bool clusters_flags[NUM_CLUSTERS] = {0};
      for(int i = 0; i < NUM_CLUSTERS; i++)
      {
         for(int j = 0; j < countTowns; j++)
         {
            not_return[i][j] = 0;
         }
      }
      
      for(int i = 0; i < NUM_CLUSTERS; i++)
      {
         if(clusters_flags[i] == 1)
         {
            continue;
         }
         
         while(td == -1)
         {
            if(clusters_flags[i] == 1)
            {
               break;
            }
            startTime = town0.mTimeStart;
            if(cnt[i] > 2) 
            {
               tr = algfunc(routes[i], cnt[i], &m, &startTime, endTime, T, t_end, countTowns);
            } 
            else 
            {
               tr = subtourdistanceTw(routes[i], cnt[i], &m, startTime, endTime);
            }
            td = tr.localtimer;
            if(td == -1)
            {
               if(cnt[i] == 1 && routes[i][cnt[i] - 1].t.name == 0)
               {
                  break;
               }
               cnt[i]--;
               not_return[i][routes[i][cnt[i]].t.name] = 1;
               
               int min_distance = INT_MAX;
               int closest_cluster = 0; 
               for(int j = 0; j < NUM_CLUSTERS; j++)
               {
                  int dist = euclidean_distance(routes[i][cnt[i]], cluster_centers[j]);
                  if (dist < min_distance && not_return[j][routes[i][cnt[i]].t.name] == 0) 
                  { 
                     min_distance = dist; 
                     closest_cluster = j;
                  }   
               }
               
               if(min_distance == INT_MAX)
               {
                  err_towns++;
               }
               else
               {   
                  routes[closest_cluster][cnt[closest_cluster]] = routes[i][cnt[i]];
                  
                  cnt[closest_cluster]++;
                  clusters_flags[closest_cluster] = 0;
                  if(closest_cluster < i)
                  {
                     i = closest_cluster;
                  }
               }

            }
         }

         clusters_flags[i] = 1;

         td = -1;
      }

      for(int i = 0; i < NUM_CLUSTERS; i++)
      {
         startTime = town0.mTimeStart;
         tr = subtourdistanceTw(routes[i], cnt[i], &m, startTime, endTime); 
         distanceInTourNew += tr.only_distance;
         write_cvrptw_subtour(res_distance, routes[i], cnt[i]);
      }
      
      if(distanceInTourBest == -1.0) 
      {
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + (errorCounter + err_towns) * 10), (clock() - runtime) / CLOCKS_PER_SEC);
         distanceInTourBest = distanceInTourNew * 13 + (errorCounter + err_towns) * 10;
      } 
      
      if(distanceInTourNew * 13 + (errorCounter + err_towns) * 10 < distanceInTourBest) 
      {
         distanceInTourBest = distanceInTourNew * 13 + (errorCounter + err_towns) * 10;
         write_cvrptw_end_tour(res_distance, distanceInTourBest);
         fprintf(out, "%lf\t%lf\n", (distanceInTourNew * 13 + (errorCounter + err_towns) * 10), (clock() - runtime) / CLOCKS_PER_SEC);
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
      if(full_time > 20)
      {
         stop = 1;
      }
      
   }
   /* данный параметр очень важно заново обнулять, так как он глобальный и при решении следующих задач
   будет сразу вызывать Ctrl+C*/
   stop = 0;
   double final_time = (clock() - runtime) / CLOCKS_PER_SEC;
   fprintf(out, "%lf\t%lf\n", (distanceInTourBest), final_time);
   printf("2Opt:%d 3Opt:%d KOpt:%d\n", two_opt, three_opt, k_opt);
   two_opt = 0;
   three_opt = 0;
   k_opt = 0;
   printf("\nОкончательное время оптимизации: %lf \nОкончательная длина маршрута: %lf \n", final_time, (distanceInTourBest));
   
   fputc('\n', out);
   printf("FINISHHHH\n");
   for (int i=0; i < newCountTowns; i++)
   {
      free(distance_matrix[i]);
   }
   free(distance_matrix);

   for (int i=0; i < NUM_CLUSTERS; i++)
   {
      free(cluster_centers[i]);
   }
   free(cluster_centers);

   free(cluster_assignments);
   for (int i=0; i < NUM_CLUSTERS; i++)
   {
      free(routes[i]);
   }

   free(routes);
   free(cnt);
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
