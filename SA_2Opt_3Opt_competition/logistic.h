#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "distanceMatrix.h"
#include "parameters.h"
#include <limits.h>


#pragma once

typedef struct town {
	int name;
	double x, y;
	int weight;
} town;

#define errortown maketown(-1, -1, -1, -1)
#define zerotown maketown(-3, 0, 0, 3000)

town maketown(int name, double x, double y, int weight)
{
	town t;
	t.name = name;
	t.x = x;
	t.y = y;
	t.weight = weight;
	return t;
}

void printtown(const town t)
{
	printf("%d %lf %lf %d\n", t.name, t.x, t.y, t.weight);
}

double getDistance(const town town1, const town town2)
{
	char path[PATH_MAX];
	char output[120];
	FILE *fp; int status;

	char *req = "curl -s 'http://router.project-osrm.org/route/v1/driving/%lf,%lf;%lf,%lf?overview=false'";
	snprintf(output, 120, req, town1.y, town1.x, town2.y, town2.x);
	fp = popen(output, "r");
	if(fp == NULL) {
		return -2.0;
	}

	char num[12]; int trigger = 0;
	char *eptr;

	for(int c = 3; (c = fgetc(fp)) != EOF; ) {
		if(trigger <= 3 && c == 'd' && (c = fgetc(fp)) == 'i' && (c = fgetc(fp)) == 's' &&
		  (c = fgetc(fp)) == 't' && (c = fgetc(fp)) =='a' && (c = fgetc(fp)) == 'n' &&
		  (c = fgetc(fp)) == 'c' && (c = fgetc(fp)) == 'e' && (c = fgetc(fp)) == '"' &&
		  (c = fgetc(fp)) == ':') {
		  	if(trigger < 2) {
		  		trigger++;
		  	}
		  	int h = 0;
			while((c = fgetc(fp)) != ',') {
				num[h] = c;
				h++;
			}
			num[h] = '\0';
			trigger++;
		}
	}
	double result = strtod(num, &eptr);
	pclose(fp);	
	return result;
}

double getDistanceE(const town town1, const town town2)
{
	return sqrt(pow((town2.x - town1.x), 2) + pow((town2.y - town1.y), 2));
}