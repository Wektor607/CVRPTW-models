#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "threeOpt.h"

double lkh3optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns) // timer - is a now time. Global time on the world.
{
    /*
    2-opt
    0: Or O  O
    1: O  Or O
    2: O  O  Or
    3-opt
    3: O  [O  O ]
    4: Or [O  O ]
    5: O  [Or O ]
    6: O  [O  Or]
    */
    depoShift(lenSub, sub);
    
    twtown *subcopy = (twtown*)malloc(lenSub * sizeof(twtown));
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best, newd;
    
    best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
    if(best == 0) {
        return -1;
    }

    for(int a = 0; a < lenSub - 2; a++) 
    {
        for(int b = a + 1; b < lenSub - 1; b++) 
        {
            for(int c = b + 1; c < lenSub; c++)
            {
                reverse_segment_if_better(m, subcopy, a, b, c, lenSub);
                newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
                
                if(newd != -1 && (best == -1 || newd < best)) 
                {
                    best = newd;
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
            } 
        }
    }
    
    free(subcopy);
    return best;
}
