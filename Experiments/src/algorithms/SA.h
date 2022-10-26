#include "../mainFunctions/dataProcessing.h"

#ifndef SA_H
#define SA_H

void GenerateStateCandidateTw(twtown *sub, twtown *best, int lenSub);

double saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin, int countTowns, int dist_param);

#endif