#include "createObjects.h"
#ifndef TOWNACTIONS_H
#define TOWNACTIONS_H

struct twoResults
{
    double localtimer;
    double only_distance;
    int count_err_towns;
};

int gettownindex(const twtown* sub, const twtown city, int lensub);

twtown getTwTownByName(int name, int counttown, const twtown* towns);

void printtwtown(const twtown t);

void swapTw(twtown *town1, twtown *town2);

void doShuffleTw(int counttown, twtown *towns);

struct twoResults subtourdistanceTw(twtown *sub, int lenSub, halfmatrix *m, const double timer, const double endTime);

void printTwTownList(const twtown* towns, int counttowns);

void reverseTownTw(twtown *sub, int i, int j);

void depoShift(int lenSub, twtown *sub);

void reverse_segment_if_better(halfmatrix *m, twtown *tour, int i, int j, int k, int len);

void moveElemsTw(twtown *sub, int i, int j, int k);

#endif