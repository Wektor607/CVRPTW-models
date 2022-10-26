#include "createObjects.h"
#ifndef TOWNACTIONS_H
#define TOWNACTIONS_H

int gettownindex(const twtown* sub, const twtown city, int lensub);

twtown getTwTownByName(int name, int counttown, const twtown* towns);

void printtwtown(const twtown t);

void swapTw(twtown *town1, twtown *town2);

void doShuffleTw(int counttown, twtown *towns);

double subtourdistanceTw(twtown *sub, int lenSub, halfmatrix *m, const double timer, const double endTime);

void printTwTownList(const twtown* towns, int counttowns);

void reverseTownTw(twtown *sub, int i, int j);

int moveElemsTw(twtown *sub, int start1, int end1, int start2, int end2);

#endif