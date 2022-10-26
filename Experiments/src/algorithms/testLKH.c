#include <stdio.h>
#include <stdlib.h>
// #include "LKH.h"
// #include "twoOpt.h"
// #include "SA.h"

//gcc -c ../mainFunctions/distanceMatrix.c ../mainFunctions/logistic.c ../mainFunctions/createObjects.c ../mainFunctions/townActions.c ../mainFunctions/dataProcessing.c twoOpt.c LKH.c testLKH.c
//gcc logistic.o distanceMatrix.o  createObjects.o townActions.o dataProcessing.o twoOpt.o LKH.o testLKH.o -o test -lm
//./test
void testLKH(){
    int lenSub = 6;
    int sub[6] = {1, 2, 0, 3, 4, 5};
    int subcopy[6];
    int start = 0;
    for (int e = 0; e < lenSub; ++e)
    {
        if (sub[e] == 0)
        {
            start = e;
            break;
        }
    }
    for (int i = 0; i < lenSub; ++i)
    {
        if (i < start)
            subcopy[lenSub - start + i] = sub[i];
        else
            subcopy[i - start] = sub[i];
    }

    for(int i = 0; i < lenSub; ++i)
    {
        sub[i] = subcopy[i];
        printf("sub[%d]: %d\n", i, sub[i]);
    }
    // twtown *subb = (twtown*) malloc((newCountTowns) * sizeof(twtown));
    // halfmatrix m; 
    // readOneTwTownByBinaryNoIndex(subb, &m, "../../5_tw/test0");
    // m.data[1][1] = 5.0;
    // m.data[2][0] = 5.0;
    // m.data[0][4] = 5.0;
    // m.data[0][0] = 5.0;
    // m.data[2][2] = 5.0;
    // m.data[3][1] = 5.0;
    // printhalfmatrix(&m);
    // twtown town0 = getTwTownByName(0, newCountTowns, subb);
    // double timer = town0.mTimeStart;
    // double endTime = town0.mTimeEnd;
    
    // int dist_param = 0; // расстояние без ограничений

    // int g;
    // // subb[newCountTowns] = town0;
    // printf("START SUBB: ");
    // for(int i = 0; i < newCountTowns; i++) {
    //     printf("%d ", subb[i].t.name);
    // }
    // printf("\n");
    // double start_len = 0;
    // if(dist_param == 0)
    // {
    //     for(g = 0; g < newCountTowns-1; g++) { 
    //         start_len += getByTown(&m, subb[g].t.name, subb[g+1].t.name);
    //     }
    //     start_len += getByTown(&m, subb[0].t.name, subb[newCountTowns-1].t.name);
    // }
    // else
    // {
    //     start_len = subtourdistanceTw(subb, newCountTowns, &m, timer, endTime);
    // }
    // printf("Начальная длина: %lf\n", start_len);

    // double best = lkhTw(subb, newCountTowns, &m, &timer, endTime, 0, 0, newCountTowns, dist_param);
    // double curr_len = start_len;
    // while(curr_len > best || curr_len == -1 || best == -1 || curr_len == best)
    // {
    //     curr_len = best;
    //     best = lkhTw(subb, newCountTowns, &m, &timer, endTime, 0, 0, newCountTowns, dist_param);
    // }
    
    // start_len = 0;
    // if(dist_param == 0)
    // {
    //     for(g = 0; g < newCountTowns-1; g++) { 
    //         start_len += getByTown(&m, subb[g].t.name, subb[g+1].t.name);
    //     }
    //     start_len += getByTown(&m, subb[0].t.name, subb[newCountTowns-1].t.name);
    // }
    // else
    // {
    //     start_len = subtourdistanceTw(subb, newCountTowns, &m, timer, endTime);
    // }
    // printf("Конечная длина: %lf\n", start_len);

    // printf("END SUBB: ");
    // for(int i = 0; i < newCountTowns; i++) {
    //     printf("%d ", subb[i].t.name);
    // }
    // printf("\n");
}

int main(){
    testLKH();
}