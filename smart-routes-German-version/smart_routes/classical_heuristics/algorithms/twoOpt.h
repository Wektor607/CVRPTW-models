#include "../extraFunctions/dataProcessing.h"

#ifndef TWOOPT_H
#define TWOOPT_H

struct twoResults lkh2optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns);

#endif