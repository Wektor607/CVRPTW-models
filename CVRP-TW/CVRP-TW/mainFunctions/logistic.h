#include "parameters.h"

#pragma once

#ifndef LOGISTIC_H
#define LOGISTIC_H
#define errortown maketown(-1, -1, -1, -1)
#define zerotown maketown(-3, 0, 0, 3000)

typedef struct town {
	int name;
	double x, y;
	int weight;
} town;

town maketown(int name, double x, double y, int weight);

#endif