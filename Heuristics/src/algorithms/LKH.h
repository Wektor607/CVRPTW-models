#include "../mainFunctions/logistic.h"
#include "../mainFunctions/townActions.c"
#include "../mainFunctions/dataProcessing.c"

#ifndef LKH_H
#define LKH_H

typedef struct Edge
{
    int node1;
    int node2;
} Edge;

bool edge_equal(Edge x, Edge y);

Edge edge_init(int nd1, int nd2);

double gain(halfmatrix *m, Edge x, Edge y);

Edge *new_tour_create(Edge *T_old, int lenSub, Edge *X, Edge *Y, int i, int (*neighbours)[2], int *indexes);

void print_tour(Edge *T, int n);

double lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns);

#endif