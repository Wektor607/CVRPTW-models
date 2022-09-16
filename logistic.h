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

int sumweights(int counttown, const town *t)
{
	int rsum = 0;
	for(int i = 0; i < counttown; i++)
	{
		rsum += t[i].weight;
	}
	return rsum;
}

typedef struct car {
	int name;
	int capacity;
} car;

car makecar(int name, int capacity)
{
	car c;
	c.name = name;
	c.capacity = capacity;
	return c;
}

town getTownByName(int name, int counttown, const town* towns)
{
	for(int i = 0; i < counttown; i++)
	{
		if(towns[i].name == name) {
			return towns[i];
		}
	}
	return errortown;
}

int read_file(const char* name_file, town *towns, int counttowns)
{
    FILE* in;
    in = fopen(name_file, "r");
    if(in == NULL)
    {
        return -1;
    }
    int symbol;
    for(int i = 0; (symbol = fgetc(in)) != EOF && symbol != '\n'; i++);


    double x, y, cap;
    for(int i = counttowns-1; i >= 0 ; i--)
    {
        fscanf(in, "%lf\t%lf\t%lf\n", &x, &y, &cap);
        towns[i] = maketown(i, x, y, cap);
    }
    fclose(in);
    return 0;
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
	return sqrt(pow(town2.x - town1.x, 2) + pow(town2.y - town1.y, 2));
}

void swap(town *town1, town *town2)
{
	town m = *town1;
	*town1 = *town2;
	*town2 = m;
}

void doShuffle(int counttown, town *towns)
{
	int j, k;
	for(int i = 0; i < counttown; i++) {
		k = rand() % counttown;
		j = rand() % counttown;
		swap(&towns[j], &towns[k]);
	}
}

void printTownList(int counttown, town *towns)
{
	putchar('[');
	for(int i = 0; i < counttown; i++) 
	{
		printf("%d ", towns[i].name);
	}
	putchar(']');putchar('\n');
}

double subtourdistance(town *sub, int lenSub, halfmatrix* m)
{

	if(lenSub == 0) {
		return 0;
	}
	
	double r = getByTown(m, 0, sub[0].name) + getByTown(m, 0, sub[lenSub-1].name);
	
	for(int i = 0; i < lenSub-1; i++)
	{

		r += getByTown(m, sub[i].name, sub[i+1].name);
	}
	return r;
}

void reverseTown(town *sub, int i, int j)
{
	int s = (j + i) / 2;
	if((j + i) % 2 == 1) {
		for(int k = 0; k < (j - i + 1) / 2; k++)
		{
			swap(&sub[s - k], &sub[s+1+k]);
		}
	} else {
		for(int k = 0; k < (j - i) / 2; k++)
		{
			swap(&sub[s-1-k], &sub[s+1+k]);
		}
	}
}

int moveElems(town *sub, int start1, int end1, int start2, int end2)
{
	int difference = (end1 - start1 - (end2 - start2));

	town tmp;

	town *mtmp = (town*)malloc(abs(difference) * sizeof(town));
	if(mtmp == NULL) {
		return -1;
	}
	for(int i = 0; i < abs(difference); i++) {
		if(difference > 0) {
			mtmp[i] = sub[end1 + 1 - difference + i];
		} 
		else if(difference < 0) 
		{
			mtmp[i] = sub[start2 + i];
		}
	}
	if(difference > 0) {

		for(int i = 0; i < end2 - end1; i++) { 
			sub[end1 + 1 - difference + i] = sub[end1 + 1 + i];
		}

		for(int i = 0; i < end2 - start2 + 1; i++) {
			tmp = sub[start1 + i];
			sub[start1 + i] = sub[start2 + i - difference];
			sub[start2 + i - difference] = tmp;
		}

		for(int i = 0; i < abs(difference); i++) {
			sub[end2 + 1 - difference +i] = mtmp[i];
		}

	} else if(difference < 0) {

		for(int i = 0; i < end1 - start1 + 1; i++) {
			tmp = sub[start1 + i];
			sub[start1 + i] = sub[start2 - difference + i];
			sub[start2 - difference + i] = tmp;
		}

		for(int i = 0; i < start2-start1; i++) {
			sub[start2 - difference - 1 - i] = sub[start2 - 1 - i];
		}
		
		for(int i = 0; i < abs(difference); i++) {
			sub[start1 + i] = mtmp[i];
		}
	}

	free(mtmp);
	return 0;
}