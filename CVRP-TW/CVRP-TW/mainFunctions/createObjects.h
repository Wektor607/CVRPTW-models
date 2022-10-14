#include "distanceMatrix.h"

#ifndef CREATEOBJECTS_H
#define CREATEOBJECTS_H

typedef struct timeWindowTown {
    town t;
    double mTimeStart, mTimeEnd;
    double mTimeService;
} twtown;

twtown maketwtown(town t, double mTimeStart, double mTimeEnd, double mTimeService);

#endif