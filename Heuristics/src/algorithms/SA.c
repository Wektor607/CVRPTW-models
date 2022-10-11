#include <stdio.h>
#include <stdlib.h>

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

double saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin, int countTowns) {
    twtown subcopy[lenSub];
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        // printf("subcopy[%d]: %d\n", i, sub[i].t.name);
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd, p;
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
    double runtime = clock(); 
    int T = tmax;
    for(int k = 0; T >= tmin; T = tmax / (k + 1), k++) {
        GenerateStateCandidateTw(subcopy, sub, lenSub);

        newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
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
