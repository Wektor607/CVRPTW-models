#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#pragma once

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

void finalizehalfmatrix(halfmatrix *m)
{
	for(int i = 0; i < m->height; i++)
	    free(m->data[i]);

	free(m->data);
}

//Проверяет вышел ли элемент за пределы половинчетой матрицы TODO: check
bool checkPositionAthalfmatrix(const halfmatrix *canvas, int i, int j) 
{
    return i>=0 && j>=0 && (canvas->width - 1 - i) >= j && i < canvas->height;//(x >= 0 && x < canvas->width && y >= x && y < canvas->height);
}

void pointAthalfmatrix(halfmatrix *canvas, int i, int j, double elem) {
    if (!checkPositionAthalfmatrix(canvas, i, j))
        return;

    canvas->data[i][j] = elem;
}

//Если не будет работать нужно дописать... TODO
void fillhalfmatrix(halfmatrix *canvas, double elem) {
    for (int i = 0; i < canvas->height; i++)
    {
        for (int j = i; j < canvas->width; j++)
        {
            pointAthalfmatrix(canvas, i, j-i, elem);
        }
    }
}

void inithalfmatrix(halfmatrix *m, int h)
{
	m->height = h;
	m->width = h;
	m->data = (double**)malloc(h * sizeof(double*)); //(h*(h+1)/2)
	if(m->data == NULL) 
    {
        printf("Error malloc\n");
        exit(-1);
    }
	for(int i = 0; i < h; i++) {
		m->data[i] = (double*)malloc((h-i)*sizeof(double));
		if(m->data[i] == NULL) 
        {
            printf("Error malloc\n");
            finalizehalfmatrix(m);
            exit(-1);
        }
	}
}

double getByTown(const halfmatrix *canvas, int townx, int towny)
{
    //printf("i: %d, j: %d\n", my_min(townx, towny), canvas->width - my_max(townx, towny));
    //printf("hlf: %d, townx: %d, towny: %d\n", canvas->width, townx, towny);
    return canvas->data[my_min(townx, towny)][canvas->width - my_max(townx, towny)];
}

//Печатает половинную матрицу: TODO
void printhalfmatrix(const halfmatrix *canvas) {
    int i;
    for(i = 0; i < canvas->height; i++) {
        int j;
        for (j = i; j < canvas->width; j++) 
        {
            // putchar(canvas->data[i][j]);
            // printf("%d %d\n", i, j-i);
            printf("%lf  ", canvas->data[i][j-i]);
        }
        putchar('\n');
    }
}

void printtownmatrix(const halfmatrix * canvas) {
    printf("  :");
    for(int i = 0; i < canvas->width;i++)
    {
        printf(" %-8c  ", '0' + canvas->width - i);
    }
    putchar('\n');
    for(int i = 0; i < canvas->width; i++)
    {
        printf("%-2c:", '0' + i);
        for(int j = i; j < canvas->width;j++)
        {
            if(canvas->data[i][j-i] < 0){
                printf("%lf  ", canvas->data[i][j-i]);
            } else {
                printf(" %lf  ", canvas->data[i][j-i]);
            }
        }
        putchar('\n');
    }
}
