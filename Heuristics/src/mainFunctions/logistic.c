#include <stdlib.h>
#include <stdio.h>
#include "logistic.h"

town maketown(int name, double x, double y, int weight)
{
	town t;
	t.name = name;
	t.x = x;
	t.y = y;
	t.weight = weight;
	return t;
}