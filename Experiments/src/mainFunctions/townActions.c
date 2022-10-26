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
    //смещаем депо в начало
    depoShift(lenSub, sub);

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

    // Если нужно вычислить расстояние, то вычитаем суммарное время ожидания и время обслуживания
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


void reverseTownTw(twtown *sub, int i, int j)
{
    // int s = (j - i) / 2;
    // for (int k = 1; k <= s; ++k)
    // {
    //     swapTw(&sub[i + k], &sub[j - k + 1]);
    // }
    int s = (j + i) / 2;
    if((j + i) % 2 == 1) {
        for(int k = 0; k < (j - i + 1) / 2; k++)
        {
            swapTw(&sub[s - k], &sub[s+1+k]);
        }
    } else {
        for(int k = 0; k < (j - i) / 2; k++)
        {
            swapTw(&sub[s-1-k], &sub[s+1+k]);
        }
    }
}

int moveElemsTw(twtown *sub, int start1, int end1, int start2, int end2)
{
    int difference = (end1 - start1 - (end2 - start2));

    twtown tmp;

    twtown *mtmp = (twtown*)malloc(abs(difference) * sizeof(twtown));
    if(mtmp == NULL) 
    {
        return -1;
    }
    for(int i = 0; i < abs(difference); i++) 
    {
        if(difference > 0) 
        {
            mtmp[i] = sub[end1 + 1 - difference + i];
        } 
        else if(difference < 0) 
        {
            mtmp[i] = sub[start2 + i];
        }
    }

    if(difference > 0) 
    {

        for(int i = 0; i < end2 - end1; i++) 
        {
            sub[end1 + 1 - difference + i] = sub[end1 + 1 + i];
        }

        for(int i = 0; i < end2 - start2 + 1; i++) 
        {
            tmp = sub[start1 + i];
            sub[start1 + i] = sub[start2 + i - difference];
            sub[start2 + i - difference] = tmp;
        }

        for(int i = 0; i < abs(difference); i++) 
        {
            sub[end2 + 1 - difference +i] = mtmp[i];
        }

    } 
    else if(difference < 0) 
    {

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
