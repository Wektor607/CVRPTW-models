#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "twoOpt.h"

double lkh2optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns, int dist_param)
{
    twtown *subcopy = (twtown*)malloc((lenSub) * sizeof(twtown));
    //цикл копирования sub -> subcopy

    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = 0, newd;
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
    
    if(best == 0) {
        return -1;
    }

	for(int a = 0; a < lenSub; a++)
	{
		for(int b = a + 1; b < lenSub; b++)
		{
		
			reverseTownTw(subcopy, my_min(a, b), my_max(a, b));
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
            if(best == -1 && newd != -1) 
            {
                best = newd;
                //цикл копирования subcopy -> sub
                for(int j = 0; j < lenSub; j++)
                {
                    sub[j] = subcopy[j];
                }
            }

            if(newd != -1 && newd < best) 
            {
                best = newd;
                //цикл копирования subcopy -> sub
                for(int j = 0; j < lenSub; j++)
                {
                    sub[j] = subcopy[j];
                }
            } 
            else 
            {
                for(int j = 0; j < lenSub; j++)
                {
                    subcopy[j] = sub[j];
                }
            }
            printf("REVERSE TOWN: %d %d ", my_min(a, b), my_max(a, b));
            printTwTownList(sub, 6);
            printf("\n");
		}
	}
    free(subcopy);

    // *timer += best;  

	return best;
}