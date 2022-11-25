#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "twoOpt.h"

double lkh2optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns)
{
    depoShift(lenSub, sub);
    twtown *subcopy = (twtown*)malloc((lenSub) * sizeof(twtown));
    //цикл копирования sub -> subcopy

    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = 0, newd;
    
    best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
    
    if(best == 0) {
        return -1;
    }

    double lengthDelta = 0;
	for(int a = 0; a < lenSub - 2; a++)
	{
		for(int b = a + 2; b < lenSub; b++)
		{
            lengthDelta = -getByTown(m, sub[a].t.name, sub[(a + 1) % lenSub].t.name) - getByTown(m, sub[b].t.name, sub[(b + 1) % lenSub].t.name) 
            + getByTown(m, sub[a].t.name, sub[b].t.name) + getByTown(m, sub[(b + 1) % lenSub].t.name, sub[(a + 1) % lenSub].t.name);

            if(lengthDelta < 0)
            {
                reverseTownTw(subcopy, my_min(a, b), my_max(a, b));
                
                newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
                
                // В случае успеха запоминаем лучший тур
                if(newd != -1 && (best == -1 || newd < best)) 
                {
                    best = newd;
                    //цикл копирования subcopy -> sub
                    for(int j = 0; j < lenSub; j++)
                    {
                        sub[j] = subcopy[j];
                    }
                } 
                // В противном случае возвращаемся к предыдущему лучшему туру
                else 
                {
                    for(int j = 0; j < lenSub; j++)
                    {
                        subcopy[j] = sub[j];
                    }
                }
            }
		}
	}
    
    if(best != -1)
    {
        *timer += best;  
    }

    free(subcopy);
	return best;
}