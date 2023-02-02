#include "../extraFunctions/dataProcessing.h"


#ifndef LKH_H
#define LKH_H

typedef struct Edge
{
    int node1;
    int node2;
} Edge;

extern int two_opt; 
extern int three_opt; 
extern int k_opt;

bool edge_equal(Edge x, Edge y);

Edge edge_init(int nd1, int nd2);

double gain(halfmatrix *m, Edge x, Edge y);

void new_tour_create(halfmatrix *m, Edge *T_old, Edge *T_new, twtown *sub, int lenSub, Edge *X, Edge *Y, int i, int *indexes, int *Xindex);

void print_tour(Edge *T, int n);

struct twoResults lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns);

#endif