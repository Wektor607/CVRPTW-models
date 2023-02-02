#include "../extraFunctions/dataProcessing.h"

#ifndef SA_H
#define SA_H

void GenerateStateCandidateTw(twtown *sub, int lenSub);

struct twoResults saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin, int countTowns);

#endif