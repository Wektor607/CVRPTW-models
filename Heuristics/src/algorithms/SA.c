#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "SA.h"

void GenerateStateCandidateTw(twtown *sub, twtown *best, int lenSub) 
{
    int indexA = rand() % lenSub;
    int indexB = rand() % lenSub;
    while(indexA == indexB){
        indexB = rand() % lenSub;
    }

    for(int i = 0; i < my_min(indexA,indexB); i++) 
    {
        sub[i] = best[i];
    }

    for(int i = 0; i < my_max(indexA, indexB) - my_min(indexA, indexB) + 1; i++) 
    { 
        sub[my_min(indexA, indexB) + i] = best[my_max(indexA, indexB) - i]; 
    }

    for(int i = my_max(indexA, indexB) + 1; i < lenSub; i++) 
    {
        sub[i] = best[i];
    }
}

double saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin, int countTowns, int dist_param) {
    twtown subcopy[lenSub];
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        // printf("subcopy[%d]: %d\n", i, sub[i].t.name);
    }

    double best = 0, newd, p;
    if(dist_param == 1)
    {
        best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
    }
    else
    {
        for(int c = 0; c < lenSub-1; c++) { 
            best += getByTown(m, subcopy[c].t.name, subcopy[c+1].t.name);
        }
        best += getByTown(m, subcopy[0].t.name, subcopy[lenSub-1].t.name);
    }
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
    int T = tmax;
    for(int k = 0; T >= tmin; T = tmax / (k + 1), k++) {
        GenerateStateCandidateTw(subcopy, sub, lenSub);

        newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
        if(dist_param == 1)
        {
            newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
        }
        else
        {
            newd = 0;
            for(int c = 0; c < lenSub-1; c++) { 
                newd += getByTown(m, subcopy[c].t.name, subcopy[c+1].t.name);
            }
            newd += getByTown(m, subcopy[0].t.name, subcopy[lenSub-1].t.name);
        }
        if(best == -1 && newd != -1) {
            best = newd;
            //цикл копирования subcopy -> sub
            for(int j = 0; j < lenSub; j++)
            {
                sub[j] = subcopy[j];
            }
        }

        if(newd != -1 && newd < best) {
            best = newd;
            for(int i = 0; i < lenSub; i++) 
            {
                sub[i] = subcopy[i];
            }
        } else if(newd != -1) {
            p = exp((best - newd) / T);
            if(p > (rand() % 10000 / 10000.0)) 
            {
                best = newd;
                for(int i = 0; i < lenSub; i++) 
                {
                    sub[i] = subcopy[i];
                }
            }
        }
    }
    *timer += best;
    return best;
}
