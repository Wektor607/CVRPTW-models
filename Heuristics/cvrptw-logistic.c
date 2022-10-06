#include <stdio.h>
#include "logistic.h"
#include <stdlib.h>
#include <limits.h>


typedef struct timeWindowTown {
    town t;
    double mTimeStart, mTimeEnd;
    double mTimeService;
} twtown;

twtown maketwtown(town t, double mTimeStart, double mTimeEnd, double mTimeService) {
    twtown tw;
    tw.t = t;
    tw.mTimeService = mTimeService;
    tw.mTimeStart = mTimeStart;
    tw.mTimeEnd = mTimeEnd;
    return tw;
}

#define errortwtown maketwtown(errortown, -1, -1, -1)
#define MIN(a, b) ((a < b) ? (a) : (b))
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
    double dop_time = 0;

    for (int i = 0; i < lenSub - 1; i++)
    {
        localtimer += getByTown(m, sub[i].t.name, sub[i + 1].t.name) + sub[i + 1].mTimeService;
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
    return localtimer - timer;
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

double lkh2optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime)
{
    twtown *subcopy = (twtown*)malloc((lenSub) * sizeof(twtown));
    //цикл копирования sub -> subcopy

    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd;
    if(best == 0) {
        return -1;
    }

    // printf("\n--*--\nOld total time: %lf\n", best);
    // printf("Old list: "); printTwTownList(subcopy, lenSub); putchar('\n');

	for(int a = 0; a < lenSub; a++)
	{
		for(int b = a + 1; b < lenSub; b++)
		{
		
			reverseTownTw(subcopy, my_min(a, b), my_max(a, b));
			newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
            if(best == -1 && newd != -1) 
            {
                best = newd;
                //цикл копирования subcopy -> sub
                for(int j = 0; j < lenSub; j++)
                {
                    sub[j] = subcopy[j];
                }
            }

            if(newd != -1 && newd < best) 
            {
                best = newd;
                //цикл копирования subcopy -> sub
                for(int j = 0; j < lenSub; j++)
                {
                    sub[j] = subcopy[j];
                }
            } 
            else 
            {
                for(int j = 0; j < lenSub; j++)
                {
                    subcopy[j] = sub[j];
                }
            }
		}
	}
    free(subcopy);

	// printf("New distance: %lf\n", best);
	// printf("New list: "); printTwTownList(sub, lenSub);
    if(best != -1) {
        *timer += best;  
    }
	return best;
}

double lkh3optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime) // timer - is a now time. Global time on the world.
{

    /*
    2-opt
    0: Or O  O
    1: O  Or O
    2: O  O  Or
    3-opt
    3: O  [O  O ]
    4: Or [O  O ]
    5: O  [Or O ]
    6: O  [O  Or]
    */
    twtown *subcopy = (twtown*)malloc(lenSub * sizeof(twtown));
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd;
    if(best == 0) {
        return -1;
    }

    // printf("\n--*--\nOld total time: %lf\n", best);
    // printf("Old list: "); printTwTownList(subcopy, lenSub); putchar('\n');

    int mode;

    for(int a = 0; a < lenSub; a++) 
    {
        for(int b = a + 1; b < lenSub; b++) 
        {
            for(mode = 0; mode < 7; mode++) 
            {
                switch(mode){
                    case(0): {reverseTownTw(subcopy, 0, a);break;}
                    case(1): {reverseTownTw(subcopy, a+1, b);break;}
                    case(2): {reverseTownTw(subcopy, b+1, lenSub-1);break;}
                    case(3): {moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);break;}
                    case(4): {
                        
                        reverseTownTw(subcopy, 0, a);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                        
                        break;
                    }
                    case(5): {
                        
                        reverseTownTw(subcopy, a+1, b);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                    
                        break;
                    }
                    case(6): {
                        reverseTownTw(subcopy, b+1, lenSub-1);
                        moveElemsTw(subcopy, a+1, b, b+1,lenSub-1);
                        break;
                    }
                }

                newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
                if(best == -1 && newd != -1) {
                    best = newd;
                    //цикл копирования subcopy -> sub
                    for(int j = 0; j < lenSub; j++)
                    {
                        sub[j] = subcopy[j];
                    }
                }

                if(newd != -1 && newd < best) {
                    best = newd;
                    //цикл копирования subcopy -> sub
                    for(int j = 0; j < lenSub; j++)
                    {
                        sub[j] = subcopy[j];
                    }
                } else {
                    for(int j = 0; j < lenSub; j++)
                    {
                        subcopy[j] = sub[j];
                    }
                }
            }
        }
    }
    free(subcopy);
    
    // printf("New total time: %lf\n", best);
    // printf("New list: "); printTwTownList(sub, lenSub);
    if(best != -1) {
        *timer += best;  
    }
    
    return best;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Edge
{
    int node1;
    int node2;
} Edge;

bool edge_equal(Edge x, Edge y)
{
    return (x.node1 == y.node1 && x.node2 == y.node2) ||
           (x.node1 == y.node2 && x.node2 == y.node1);
}

Edge edge_init(int nd1, int nd2)
{
    Edge edge;
    edge.node1 = nd1;
    edge.node2 = nd2;
    return edge;
}

double gain(halfmatrix *m, Edge x, Edge y)
{
    return getByTown(m, x.node1, x.node2) - getByTown(m, y.node1, y.node2);
}

double lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime)
{
    /* 1 Create starting tour T */
    Edge *T = (Edge *) calloc(lenSub, sizeof(Edge));
    Edge *T1 = (Edge *) calloc(lenSub, sizeof(Edge));
    double FT = 0, FT1 = 0;
    for (int i = 0; i < lenSub - 1; ++i)
    {
        T[i] = edge_init(sub[i].t.name, sub[i + 1].t.name);
        FT += getByTown(m, sub[i].t.name, sub[i + 1].t.name);
    }
    T[lenSub - 1] = edge_init(sub[lenSub - 1].t.name, sub[0].t.name);
    FT += getByTown(m, sub[lenSub - 1].t.name, sub[0].t.name);

    /* 2 */
    for (int k = 0; k < lenSub; ++k)
    {
        bool flag6to2 = 0;
        Edge *X = (Edge *) calloc(lenSub, sizeof(Edge));
        Edge *Y = (Edge *) calloc(lenSub, sizeof(Edge));
        int t1 = T[k].node1;

        /* 3 choose X[0] */
        int tmp = (k == 0 ? T[lenSub - 1].node1 : T[k-1].node1);
        int t2[2] = {tmp, T[k].node2};
        for (int t2_var = 0; t2_var < 2; t2_var++)
        {
            X[0] = edge_init(t1, t2[t2_var]);

            /* 4  Choose Y[0] */
            int t3 = -1;
            for (int j4 = 0; j4 < lenSub; ++j4)
            {
                t3 = sub[j4].t.name;
                int flag4_1 = 0;
                for (int l = 0; l < lenSub; ++l)
                {
                    if (edge_equal(T[l], edge_init(t3, t2[t2_var])))
                    {
                        flag4_1 = 1;
                        break;
                    }
                }
                if (flag4_1)
                    continue;
                double g0 = getByTown(m, t1, t2[t2_var]) - getByTown(m, t2[t2_var], t3); /* g0 = |X[0]| - |Y[0]| */
                if (g0 > 0)
                {
                    Y[0] = edge_init(t2[t2_var], t3);
                    break; /* 12 */
                }

                /* 5 */
                for (int i = 1; i < lenSub; ++i)
                {
                    /* form var for step 6 */
                    int t2i[2] = {-1, -1};
                    int xi_node1 = Y[i - 1].node2; /* t_(2i-1) */
                    for (int j6 = 0; j6 < lenSub; ++j6)
                    {
                        int tmp = (j6 == 0 ? T[lenSub - 1].node1 : T[j6-1].node1);
                        if (T[j6].node1 == xi_node1 && tmp != t1)
                        {
                            t2i[0] = tmp;
                            t2i[1] = T[j6].node2;
                            break;
                        }
                    }
                    int t2i_var;
                    /* 6 Choose X[i] */
                    for (t2i_var = 0; t2i_var < 2; ++t2i_var)
                    {
                        int v = 0, w = 0;
                        X[i] = edge_init(xi_node1, t2i[t2i_var]);
                        Y[i] = edge_init(t2i[t2i_var], t1); /* join t2i t1 */
                        /* 6a create T1 and check T1 is tour */
                        T1 = (Edge *) calloc(lenSub, sizeof(Edge));
                        for (; v < lenSub; ++v)
                        {
                            bool flag6a = 1;
                            int x;
                            for (x = 0; x <= i; ++x)
                            {
                                if (edge_equal(T[w], X[x]))
                                {
                                    flag6a = 0;
                                    break;
                                }
                            }
                            if (flag6a) /* не меняем ребро */
                            {
                                T1[v] = T[w];
                                w++;
                            }
                            else /* меняем ребро */
                            {
                                T1[v] = Y[x];
                                int l;
                                for (l = 0; l < lenSub; ++l)
                                {
                                    if (T[l].node1 == Y[x].node2)
                                    {
                                        int flag1 = 1;
                                        for (int n = 0; n <= i; ++n)
                                        {
                                            if (edge_equal(T[l], X[n]))
                                            {
                                                flag1 = 0;
                                                break;
                                            }
                                        }
                                        if (!flag1)
                                            continue;
                                        else
                                        {
                                            w = l;
                                            break;
                                        }
                                    }
                                }
                                if (w != l)
                                    continue; /* не нашли следующее */
                            }
                        }
                        if (v != lenSub)
                            continue;

                        Edge T1_1 = edge_init(T1[0].node1, T1[0].node2);
                        bool flag6a_2 = 1;
                        /* Проверяем, что не встречаем второй раз первое ребро */
                        for (int v1 = 0; v1 < lenSub; ++v1)
                        {
                            if(edge_equal(T1_1, T1[v1]))
                            {
                                flag6a_2 = 0;
                                break;
                            }
                        }
                        if(!flag6a_2)
                            continue;

                        /* 6b */
                        bool flag6b = 1;
                        for (int s = 0; s < i; ++s)
                        {
                            if (edge_equal(X[i], Y[s]))
                            {
                                flag6b = 0;
                                break;
                            }
                        }
                        if (!flag6b)
                            continue;
                        else
                        {
                            flag6to2 = 1;
                            break;
                        }
                    
                        /* 6 end */
                        if (flag6to2)
                        {
                            FT1 = 0;
                            for (int e = 0; e < lenSub; ++e)
                            {
                                FT1 += getByTown(m, T1[e].node1, T1[e].node2);
                            }
                            FT1 += getByTown(m, T1[lenSub-1].node2, T1[0].node1);
                            if (FT1 < FT)
                            {
                                for (int e = 0; e < lenSub; ++e)
                                {
                                    T[e] = T1[e];
                                }
                                break; /* 6to2 break */
                            }
                            else
                            {
                                flag6to2 = 0;
                            }
                        }

                        /* 7 Choose Y[i] */
                        bool flag7 = 0;
                        for (int j = 0; j < lenSub; ++j)
                        {
                            int yinode2 = sub[j].t.name; /* t_(2i+1) */
                            Y[i] = edge_init(X[i].node2, yinode2);
                            bool flag7_1 = 1;
                            for (int q = 0; q < lenSub; ++q)
                            {
                                if (edge_equal(T[q], Y[i]))
                                {
                                    flag7_1 = 0;
                                    break;
                                }
                            }
                            if (!flag7_1)
                                continue; /* 8 */

                            /* 7a */
                            double Gi = 0;
                            for (int r = 0; r <= i; ++r)
                            {
                                Gi += gain(m, X[r], Y[r]);
                            }
                            if (Gi <= 0)
                                continue; /* 8 */

                            /* 7b */
                            bool flag7b = 1;
                            for (int s = 0; s <= i; ++s)
                            {
                                if (edge_equal(X[s], Y[i]))
                                {
                                    flag7b = 0;
                                    break;
                                }
                            }
                            if (!flag7b)
                                continue; /* 8 */

                            /* 7c */
                            bool flag7c = 0;
                            for (int h = 0; h < lenSub; ++h)
                            {
                                X[i + 1] = edge_init(yinode2, sub[h].t.name);
                                if (edge_equal(T[h], X[i + 1]))
                                {
                                    flag7c = 1;
                                    break;
                                }
                            }
                            if (flag7c)
                            {
                                flag7 = 1;
                                break;
                            }

                            /* 8 */
                        }
                        if (flag7)
                            break;

                    } /* 6 end */

                    if (flag6to2)
                        break;
                    
                    /* 9 */

                } /* 5 end */

                if (flag6to2)
                    break;

                /* 10 */

            } /* 4 end */

            if (flag6to2)
            {
                printf("change tour\n");
                break;
            }

            /* 11 */

        } /* 3 end */

        /* 12 */

    } /* 2 end */

    int start = 0;
    for (int i = 0; i < lenSub; ++i)
    {
        if (T[i].node1 == 0)
        {
            start = i;
            break;
        }
    }
    twtown *subcopy = (twtown *) calloc(lenSub, sizeof(twtown));
    for (int i = 0; i < lenSub; ++i)
    {
        for (int j = 0; j < lenSub; ++j)
        {
            if (T[i].node1 == sub[j].t.name)
            {
                if (i < start)
                    subcopy[lenSub - start + i] = sub[j];
                else
                    subcopy[i - start] = sub[j];
                break;
            }
        }
    }

    for (int i = 0; i < lenSub; ++i)
    {
        sub[i] = subcopy[i];
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
    if (best != -1)
    {
        *timer += best;
    }
    return best;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GenerateStateCandidateTw(twtown *sub, twtown *best, int lenSub) 
{
    int indexA = rand() % lenSub;
    int indexB = rand() % lenSub;
    while(indexA == indexB){
        indexB = rand() % lenSub;
    }

    for(int i = 0; i < my_min(indexA,indexB); i++) 
    {
        sub[i] = best[i];
    }

    for(int i = 0; i < my_max(indexA, indexB) - my_min(indexA, indexB) + 1; i++) 
    { 
        sub[my_min(indexA, indexB) + i] = best[my_max(indexA, indexB) - i]; 
    }

    for(int i = my_max(indexA, indexB) + 1; i < lenSub; i++) 
    {
        sub[i] = best[i];
    }
}

double saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime) {
    twtown subcopy[lenSub];
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        // printf("subcopy[%d]: %d\n", i, sub[i].t.name);
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd, p;
    double runtime = clock(); 
    int T = tmax;
    for(int k = 0; T >= tmin; T = tmax / (k + 1), k++) {
        GenerateStateCandidateTw(subcopy, sub, lenSub);

        newd = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
        if(best == -1 && newd != -1) {
            best = newd;
            //цикл копирования subcopy -> sub
            for(int j = 0; j < lenSub; j++)
            {
                sub[j] = subcopy[j];
            }
        }

        if(newd != -1 && newd < best) {
            best = newd;
            for(int i = 0; i < lenSub; i++) 
            {
                sub[i] = subcopy[i];
            }
        } else if(newd != -1) {
            p = exp((best - newd) / T);
            if(p > (rand() % 10000 / 10000.0)) 
            {
                best = newd;
                for(int i = 0; i < lenSub; i++) 
                {
                    sub[i] = subcopy[i];
                }
            }
        }
    }
    *timer += best;
    return best;
}

int read_file_tw(const char* name_file, twtown *towns, int counttowns)
{
    FILE* in;
    in = fopen(name_file, "r");
    if(in == NULL)
    {
        exit(-1);
    }
    int symbol;
    for(int i = 0; (symbol = fgetc(in)) != EOF && symbol != '\n'; i++);


    double x, y, cap, st;
    int hst, mst, het, met;
    for(int i = counttowns-1; i >= 0 ; i--)
    {
        fscanf(in, "%lf\t%lf\t%lf\t%d:%d-%d:%d\t%lf\n", &x, &y, &cap, &hst, &mst, &het, &met, &st);
        towns[i] = maketwtown(maketown(i, x, y, cap), (hst * 60 + mst) * 60, (het * 60 + met) * 60, st * 60);
    }
    fclose(in);
    return 0;
}

void parseOneTwTown(const char pathFile[], const char newFileName[], int index)
{

    halfmatrix m;
    inithalfmatrix(&m, countTowns-1);
    twtown towns[countTowns];
    


    char pathTown[2000], pathTable[2000];

    snprintf(pathTown, 2000, "%s/town%lld/%s%d.bin", pathSavingTowns, countTowns - 1, newFileName, index);
    snprintf(pathTable, 2000, "%s/table%lld/%s%d.bin", pathSavingTowns, countTowns - 1, newFileName, index);
    printf("%s\n", pathTown);
    FILE* outtown = fopen(pathTown, "wb");
    if(outtown == NULL) {
        printf("Error writing file: %s", pathTown);
        exit(-1);
    }
    FILE* outtable = fopen(pathTable, "wb");
    if(outtable == NULL) {
        fclose(outtown);
        printf("Error writing file: %s", pathTable);
        exit(-1);
    }

    read_file_tw(pathFile, towns, countTowns);

    printTwTownList(towns, countTowns);
    printf("After town printing!\n");

    for(int i = 0; i < countTowns; i++)
    {
        for(int j = 0; j < countTowns-i-1; j++)
        {
            double dist = getDistanceE(getTwTownByName(i, countTowns, towns).t, getTwTownByName(m.width-j, countTowns, towns).t) * KmToSeconds;
            pointAthalfmatrix(&m, i, j, dist);
            printf("%d %d: %lf\n", i, j, dist);
        }
    }
    printtownmatrix(&m);
    

    fwrite(&m.width, sizeof(int), 1, outtable);
    for(int i = 0; i < m.width; i++) {
        for(int j = 0; j < m.width - i; j++) {
            fwrite(&m.data[i][j], sizeof(double), 1, outtable);
        }
    }
    int tmp = countTowns;
    fwrite(&tmp, sizeof(int), 1, outtown);
    for(int i = 0; i < countTowns; i++) {
        fwrite(&towns[i], sizeof(struct timeWindowTown), 1, outtown);
    }

    finalizehalfmatrix(&m);
    fclose(outtown);
    fclose(outtable);

}

void parseOneTwTownNoIndex(const char pathFile[], const char newFileName[], int tcountTowns)
{

    halfmatrix m;
    inithalfmatrix(&m, tcountTowns-1);
    twtown towns[tcountTowns];
    


    char pathTown[2000], pathTable[2000];

    snprintf(pathTown, 2000, "%s-town.bin", newFileName);
    snprintf(pathTable, 2000, "%s-table.bin", newFileName);
    printf("%s\n", pathTown);
    FILE* outtown = fopen(pathTown, "wb");
    if(outtown == NULL) {
        printf("Error writing file: %s", pathTown);
        exit(-1);
    }
    FILE* outtable = fopen(pathTable, "wb");
    if(outtable == NULL) {
        fclose(outtown);
        printf("Error writing file: %s", pathTable);
        exit(-1);
    }

    read_file_tw(pathFile, towns, tcountTowns);

    printTwTownList(towns, tcountTowns);
    printf("After town printing!\n");
    double dist;
    for(int i = 0; i < tcountTowns; i++)
    {
        for(int j = 0; j < tcountTowns-i-1; j++)
        {
            
            dist = getDistanceE(getTwTownByName(i, tcountTowns, towns).t, getTwTownByName(m.width-j, tcountTowns, towns).t) * KmToSeconds;
            pointAthalfmatrix(&m, i, j, dist);

        }
    }
    printtownmatrix(&m);
    

    fwrite(&m.width, sizeof(int), 1, outtable);
    for(int i = 0; i < m.width; i++) {
        for(int j = 0; j < m.width - i; j++) {
            fwrite(&m.data[i][j], sizeof(double), 1, outtable);
        }
    }
    int tmp = tcountTowns;
    fwrite(&tmp, sizeof(int), 1, outtown);
    for(int i = 0; i < tcountTowns; i++) {
        fwrite(&towns[i], sizeof(struct timeWindowTown), 1, outtown);
    }

    finalizehalfmatrix(&m);
    fclose(outtown);
    fclose(outtable);

}

void readOneTwTownByBinary(twtown *towns, halfmatrix *m, const char newFileName[], int index) {
    char pathTown[2000], pathTable[2000];

    snprintf(pathTown, 2000, "%s/town%lld/%s%d.bin", pathSavingTowns, countTowns - 1, newFileName, index);
    snprintf(pathTable, 2000, "%s/table%lld/%s%d.bin", pathSavingTowns, countTowns - 1, newFileName, index);
    printf("%s\n", pathTown);
    FILE* intown = fopen(pathTown, "r");
    if(intown == NULL) {
        printf("Error writing file: %s", pathTown);
        exit(-1);
    }
    FILE* intable = fopen(pathTable, "r");
    if(intable == NULL) {
        fclose(intown);
        printf("Error writing file: %s", pathTable);
        exit(-1);
    }

    fread(&m->width, sizeof(int), 1, intable);
    inithalfmatrix(m, m->width);
    for(int i = 0; i < m->width; i++) {
        for(int j = 0; j < m->width - i; j++) {
            fread(&m->data[i][j], sizeof(double), 1, intable);
        }
    }

    int tmp;
    fread(&tmp, sizeof(int), 1, intown);
    for(int i = 0; i < tmp; i++) {
        fread(&towns[i], sizeof(struct timeWindowTown), 1, intown);
    }
}

void readOneTwTownByBinaryNoIndex(twtown *towns, halfmatrix *m, const char newFileName[]) {
    char pathTown[2000], pathTable[2000];
    snprintf(pathTown, 2000, "%s-town.bin", newFileName);
    snprintf(pathTable, 2000, "%s-table.bin", newFileName);
    printf("%s\n", pathTown);
    FILE* intown = fopen(pathTown, "r");
    if(intown == NULL) {
        printf("Error writing file: %s", pathTown);
        exit(-1);
    }

    FILE* intable = fopen(pathTable, "r");
    if(intable == NULL) {
        fclose(intown);
        printf("Error writing file: %s", pathTable);
        exit(-1);
    }

    printf("End readOneTownByBinary\n");
    fread(&m->width, sizeof(int), 1, intable);

    inithalfmatrix(m, m->width);

    for(int i = 0; i < m->width; i++) {
        for(int j = 0; j < m->width - i; j++) {
            fread(&m->data[i][j], sizeof(double), 1, intable);
        }
    }

    int tmp;
    fread(&tmp, sizeof(int), 1, intown);
    printf("%d\n", tmp);
    for(int i = 0; i < tmp; i++) {
        fread(&towns[i], sizeof(struct timeWindowTown), 1, intown);
    }
    printf("End readOneTownByBinary\n");

}