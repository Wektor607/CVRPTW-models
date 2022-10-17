#include <stdio.h>
#include <stdlib.h>
#include "LKH.h"
#include "twoOpt.h"
// #include "SA.h"

//gcc -c ../mainFunctions/distanceMatrix.c ../mainFunctions/logistic.c ../mainFunctions/createObjects.c ../mainFunctions/townActions.c ../mainFunctions/dataProcessing.c twoOpt.c LKH.c testLKH.c
//gcc logistic.o distanceMatrix.o  createObjects.o townActions.o dataProcessing.o twoOpt.o LKH.o testLKH.o -o test -lm
//./test
void testLKH(){
    int newCountTowns = 6;
    twtown *subb = (twtown*) malloc((newCountTowns) * sizeof(twtown));
    halfmatrix m; 
    readOneTwTownByBinaryNoIndex(subb, &m, "../../5_tw/test0");
    m.data[1][1] = 5.0;
    m.data[2][0] = 5.0;
    m.data[0][4] = 5.0;
    m.data[0][0] = 5.0;
    m.data[2][2] = 5.0;
    m.data[3][1] = 5.0;
    printhalfmatrix(&m);
    twtown town0 = getTwTownByName(0, newCountTowns, subb);
    double timer = town0.mTimeStart;
    double endTime = town0.mTimeEnd;
    
    int g;
    // subb[newCountTowns] = town0;
    printf("START SUBB: ");
    for(int i = 0; i < newCountTowns; i++) {
        printf("%d ", subb[i].t.name);
    }
    printf("\n");
    double start_len = 0;
    for(g = 0; g < newCountTowns; g++) { 
        start_len += getByTown(&m, subb[g].t.name, subb[g+1].t.name);
    }
    start_len += getByTown(&m, subb[0].t.name, subb[newCountTowns].t.name);
    printf("Начальная длина: %lf\n", start_len);

    double best = lkhTw(subb, newCountTowns, &m, &timer, endTime, 0, 0, newCountTowns);
    
    start_len = 0;
    for(g = 0; g < newCountTowns; g++) { 
        start_len += getByTown(&m, subb[g].t.name, subb[g+1].t.name);
    }
    start_len += getByTown(&m, subb[0].t.name, subb[newCountTowns].t.name);
    printf("Конечная длина: %lf\n", start_len);

    printf("END SUBB: ");
    for(int i = 0; i < newCountTowns; i++) {
        printf("%d ", subb[i].t.name);
    }
    printf("\n");
}

int main(){
    testLKH();
}