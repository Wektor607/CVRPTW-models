#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "townActions.h"


int gettownindex(const twtown* sub, const twtown city, int lensub)
{
    for(int i = 0; i < lensub; i++)
    {
        if(sub[i].t.name == city.t.name)
        {
            return i;
        }
    }
    return -1;
}

twtown getTwTownByName(int name, int counttown, const twtown* towns)
{
    for(int i = 0; i < counttown; i++)
    {
        if(towns[i].t.name == name) {
            return towns[i];
        }
    }
    return errortwtown;
}

void printtwtown(const twtown t) {
    printf("\n--*--\n");
    printtown(t.t);
    printf("%lf-%lf service:%lf\n", t.mTimeStart, t.mTimeEnd, t.mTimeService);
}

void swapTw(twtown *town1, twtown *town2)
{
    twtown m = *town1;
    *town1 = *town2;
    *town2 = m;
}

void doShuffleTw(int counttown, twtown *towns)
{
    int j, k;
    for(int i = 0; i < counttown; i++) {
        k = rand() % counttown;
        j = rand() % counttown;
        swapTw(&towns[j], &towns[k]);
    }
}

double subtourdistanceTw(twtown *sub, int lenSub, halfmatrix *m, const double timer, const double endTime)
{
    if (lenSub == 0)
    {
        return 0;
    }

    double localtimer = timer;
    double dop_time = 0, full_service_time = 0;

    for (int i = 0; i < lenSub - 1; i++)
    {
        localtimer += getByTown(m, sub[i].t.name, sub[i + 1].t.name) + sub[i + 1].mTimeService;
        full_service_time += sub[i + 1].mTimeService;
        if (!(localtimer >= sub[i + 1].mTimeStart))
        {
            dop_time += sub[i + 1].mTimeStart - localtimer;
            localtimer = sub[i + 1].mTimeStart;
        }
        // printf("%d %d: %lf\n", sub[i].t.name, sub[i + 1].t.name, getByTown(m, sub[i].t.name, sub[i + 1].t.name));
        if (!(localtimer >= sub[i + 1].mTimeStart && localtimer <= sub[i + 1].mTimeEnd && localtimer <= endTime))
        {
            // printf("localtimer <= sub[i].mTimeEnd: %lf %lf %lf %d\n", localtimer, sub[0].mTimeStart, sub[0].mTimeEnd, localtimer <= sub[0].mTimeEnd);
            return -1;
        }
    }

    localtimer += getByTown(m, sub[0].t.name, sub[lenSub - 1].t.name);
    if (!(localtimer <= endTime))
    {
        return -1;
    }

    // Если нужно вычислить расстояние, то вычитаем суммарное время ожидания
    // Если же нужно получить общее время, затраченное на всю поездку, то не отнимаем это доп. время, но переводить в метры это время
    // не корректно
    return localtimer - timer - dop_time - full_service_time;
}


void printTwTownList(const twtown* towns, int counttowns) {
    printf("[");
    for(int i = 0; i < counttowns; i++) {
        printf("%d ", towns[i].t.name);
    }
    printf("]");
}

void depoShift(int lenSub, twtown *sub)
{
    twtown *subcopy = (twtown *) calloc(lenSub, sizeof(twtown));
    int start = 0;
    for (int e = 0; e < lenSub; ++e)
    {
        if (sub[e].t.name == 0)
        {
            start = e;
            break;
        }
    }

    if(start != 0)
    {
        for (int i = 0; i < lenSub; ++i)
        {
            if (i < start)
                subcopy[lenSub - start + i] = sub[i];
            else
                subcopy[i - start] = sub[i];
        }

        for(int i = 0; i < lenSub; ++i)
        {
            sub[i] = subcopy[i];
        }
    }

    free(subcopy);
}


void reverseTownTw(twtown *sub, int i, int j)
{
    int s = (j - i) / 2;
    for (int k = 1; k <= s; ++k)
    {
        swapTw(&sub[i + k], &sub[j - k + 1]);
    }
}

void moveElemsTw(twtown *sub, int i, int j, int k)
{
    twtown *part1 = calloc(sizeof(twtown), j - i);
    twtown *part2 = calloc(sizeof(twtown), k - j);
    for (int start = i + 1, idx = 0; start <= j; ++start, ++idx)
    {
        part1[idx] = sub[start];
    }
    
    for (int start = j + 1, idx = 0; start <= k; ++start, ++idx)
    {
        part2[idx] = sub[start];
    }

    for (int idx = i + 1, part_idx = 0; idx < i + k - j + 1; ++idx, ++part_idx)
    {
        sub[idx] = part2[part_idx];
    }

    for (int idx = i + k - j + 1, part_idx = 0; idx <= k; ++idx, ++part_idx)
    {
        sub[idx] = part1[part_idx];
    }

    free(part1);
    free(part2);
}


void reverse_segment_if_better(halfmatrix *m, twtown *tour, int i, int j, int k, int len)
{
    // Given tour [...A-B...C-D...E-F...]
    int A = tour[i].t.name, B = tour[i+1].t.name, C = tour[j].t.name, D = tour[j+1].t.name, E = tour[k].t.name, F = tour[(k+1) % len].t.name;
    double d0 = getByTown(m, A, B) + getByTown(m, C, D) + getByTown(m, E, F);
    double d1 = getByTown(m, A, C) + getByTown(m, B, D) + getByTown(m, E, F);
    double d2 = getByTown(m, A, B) + getByTown(m, C, E) + getByTown(m, D, F);
    double d3 = getByTown(m, A, D) + getByTown(m, E, B) + getByTown(m, C, F);
    double d4 = getByTown(m, F, B) + getByTown(m, C, D) + getByTown(m, E, A);
    
    if (d0 > d1)
    {
        reverseTownTw(tour, i, j);
    }
    else if (d0 > d2)
    {
        reverseTownTw(tour, j, k);
    }
    else if (d0 > d4)
    {
        reverseTownTw(tour, i, k);
    }
    else if (d0 > d3)
    {
        moveElemsTw(tour, i, j, k);
    }
}