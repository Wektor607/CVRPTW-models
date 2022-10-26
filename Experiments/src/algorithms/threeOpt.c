#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "threeOpt.h"

double lkh3optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns, int dist_param) // timer - is a now time. Global time on the world.
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
    twtown *subcopy = (twtown*)malloc(lenSub * sizeof(twtown));
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = 0, newd;
    
    best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
    if(best == 0) {
        return -1;
    }

    // printf("\n--*--\nOld total time: %lf\n", best);
    // printf("Old list: "); printTwTownList(subcopy, lenSub); putchar('\n');

    int mode;

    for(int a = 0; a < lenSub; a++) 
    {
        for(int b = a + 1; b < lenSub; b++) 
        {
            for(mode = 0; mode < 7; mode++) 
            {
                switch(mode){
                    case(0): {reverseTownTw(subcopy, 0, a);break;}
                    case(1): {reverseTownTw(subcopy, a+1, b);break;}
                    case(2): {reverseTownTw(subcopy, b+1, lenSub-1);break;}
                    case(3): {moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);break;}
                    case(4): {
                        
                        reverseTownTw(subcopy, 0, a);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                        
                        break;
                    }
                    case(5): {
                        
                        reverseTownTw(subcopy, a+1, b);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                    
                        break;
                    }
                    case(6): {
                        reverseTownTw(subcopy, b+1, lenSub-1);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                        break;
                    }
                }

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
                    //цикл копирования subcopy -> sub
                    for(int j = 0; j < lenSub; j++)
                    {
                        sub[j] = subcopy[j];
                    }
                } else {
                    for(int j = 0; j < lenSub; j++)
                    {
                        subcopy[j] = sub[j];
                    }
                }
            }
        }
    }
    free(subcopy);

    if(best != -1)
    {
        *timer += best;
    }
    return best;
}
