#include "logistic.h"
#include <stdbool.h>
#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

typedef struct halfmatrix {
	int height;
	int width;
	double **data;
} halfmatrix;

/*
i\j0 1 2 3 4 
4: 1 2 3 4 5
3: 1 2 3 4 *
2: 1 2 3 * *
1: 1 2 * * *
0: 1 * * * *
h(h+1)/2
21 - element
*/


#define my_min(x, y) (((x) > (y)) ? (y) : (x))
#define my_max(x, y) (((x) < (y)) ? (y) : (x))

void finalizehalfmatrix(halfmatrix *m);

bool checkPositionAthalfmatrix(const halfmatrix *canvas, int i, int j);

void pointAthalfmatrix(halfmatrix *canvas, int i, int j, double elem);

void fillhalfmatrix(halfmatrix *canvas, double elem);

void inithalfmatrix(halfmatrix *m, int h);

double getByTown(const halfmatrix *canvas, int townx, int towny);

void printhalfmatrix(const halfmatrix *canvas);
void printtownmatrix(const halfmatrix * canvas);

void printtown(const town t);

double getDistance(const town town1, const town town2);

double getDistanceE(const town town1, const town town2);

#endif