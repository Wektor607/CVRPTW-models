#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#pragma once

//----------------------------------------------------------------------------
// IntVector2d

struct IntVector2d {
    int x, y;
};

// typedef int weight_t; // example: it defines type 'weight_t' as a synonym for 'int'

typedef struct IntVector2d IntVector2d; // now we can use 'IntVector2d' instead of 'struct IntVector2d'

IntVector2d makeVector2d(int x, int y) {
    IntVector2d v;
    v.x = x;
    v.y = y;
    return v;
}

void printIntVector2d(IntVector2d v) {
	printf("(%d, %d)\n", v.x, v.y);
}

IntVector2d askAndScanIntVector2d(void) {
	int x, y;
	printf("Pleace, enter your coordinates:");
	scanf("%d%d", &x, &y);
	return makeVector2d(x, y);
}
