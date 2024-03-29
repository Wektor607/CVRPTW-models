#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "SA.h"

void GenerateStateCandidateTw(twtown *sub, int lenSub) 
{
    twtown *sub_copy = (twtown*)malloc((lenSub) * sizeof(twtown));    
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        sub_copy[i] = sub[i];
    }
    
    int indexA = rand() % lenSub;
    int indexB = rand() % lenSub;
    while(indexA == indexB){
        indexB = rand() % lenSub;
    }

    for(int i = 0; i < my_min(indexA,indexB); i++) 
    {
        sub[i] = sub_copy[i];
    }

    for(int i = 0; i < my_max(indexA, indexB) - my_min(indexA, indexB) + 1; i++) 
    { 
        sub[my_min(indexA, indexB) + i] = sub_copy[my_max(indexA, indexB) - i]; 
    }

    for(int i = my_max(indexA, indexB) + 1; i < lenSub; i++) 
    {
        sub[i] = sub_copy[i];
    }
    
    free(sub_copy);
}

struct twoResults saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin, int countTowns) {
    depoShift(lenSub, sub);
    
    twtown *sub_current = (twtown*)malloc((lenSub) * sizeof(twtown));
    twtown *sub_old_current = (twtown*)malloc((lenSub) * sizeof(twtown));
    
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        sub_old_current[i] = sub[i];
    }
    
    double candidate_Energy, p;
    struct twoResults tr = subtourdistanceTw(sub_old_current, lenSub, m, *timer, endTime);
    double current_Energy = tr.localtimer;

    int T = tmax;

    for(int k = 0; T >= tmin; T = (tmax * 0.1 / k), k++) 
    {
        // кладем старый маршрут в новый и подаем в генерацию
        for(int i = 0; i < lenSub; i++)
        {
            sub_current[i] = sub_old_current[i];
        }
        GenerateStateCandidateTw(sub_current, lenSub);
        tr = subtourdistanceTw(sub_old_current, lenSub, m, *timer, endTime);
        candidate_Energy = tr.localtimer;

        if((current_Energy == -1 || candidate_Energy < current_Energy) && candidate_Energy != -1)
        {
            current_Energy = candidate_Energy;
            for(int i = 0; i < lenSub; i++)
            {
                sub_old_current[i] = sub_current[i];
            }
        }
        else if(current_Energy != -1 && candidate_Energy != -1 && candidate_Energy > current_Energy)
        {
            p = exp((current_Energy - candidate_Energy) / T);
            
            if(p >= ((double) rand() / (double) RAND_MAX))
            {
                current_Energy = candidate_Energy;
                for(int i = 0; i < lenSub; i++)
                {
                    sub_old_current[i] = sub_current[i];
                }
            }
        }
    }
    
    if(current_Energy != -1)
    {
        for(int p = 0; p < lenSub; ++p)
        {
            sub[p] = sub_old_current[p];
        }
    }
    
    free(sub_current);
    free(sub_old_current);

    return tr;
}