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

double lkh2optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2)
{
    twtown *subcopy = (twtown*)malloc((lenSub) * sizeof(twtown));
    //цикл копирования sub -> subcopy

    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd;
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
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

double lkh3optTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2) // timer - is a now time. Global time on the world.
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
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
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

double separateTour(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime){
    twtown *subcopy = (twtown*)malloc(lenSub * sizeof(twtown));
    int A_size0 = lenSub / 2, B_size0 = lenSub - (lenSub / 2);
    int *A0 = (int*)malloc(A_size0 * sizeof(int));
    int *B0 = (int*)malloc(B_size0 * sizeof(int));
    //цикл копирования sub -> subcopy
    
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        if (i < A_size0) {A0[i] = i;}
        else {B0[i - A_size0] = i;}
    }
    // printf("town %d\n", lenSub);

    // for(int i = 0; i < lenSub; i++)
    // {
    //     for(int j = 0; i < lenSub; i++)
    //     {
    //         if(i != j)
    //         {
    //             printf("%d %d: %lf\n", i, j, getByTown(m, subcopy[i].t.name, subcopy[j].t.name));
    //         }
    //     }
    // }
    while(1)
    {
        /* инициализация D */
        int A_size = A_size0, B_size = B_size0;
        int *D = (int*)malloc(lenSub * sizeof(int));
        for(int a = 0; a < A_size0; a++){
            int I = 0, E = 0;
            /* external cost for a in A0 */
            for(int y = 0; y < B_size0; y++)
            {
                E += getByTown(m, subcopy[A0[a]].t.name, subcopy[B0[y]].t.name);
            }
            /* internal cost for a in A0 */
            for(int x = 0; x < A_size0; x++)
            {
                if(a != x)
                {
                    I += getByTown(m, subcopy[A0[a]].t.name, subcopy[A0[x]].t.name);
                }
            }
            D[A0[a]] = E - I;
        }

        for(int b = 0; b < B_size0; b++){
            int I = 0, E = 0;
            /* external cost for b in B0 */
            for(int y = 0; y < A_size0; y++)
            {
                E += getByTown(m, subcopy[B0[b]].t.name, subcopy[A0[y]].t.name);
            }
            /* internal cost for b in B0 */
            for(int x = 0; x < B_size0; x++)
            {
                if(b != x)
                {
                    I += getByTown(m, subcopy[B0[b]].t.name, subcopy[B0[x]].t.name);
                }
            }
            D[B0[b]] = E - I;
            // printf("b_%d: %d\n", b, D[B0[b]]);
        }

        int n;
        if(A_size0 < B_size0) {n = A_size0;}else{n = B_size0;}
        int *g = (int*) malloc(n * sizeof(int));
        /* массивы с номерами элементов, которые переставляются */
        int *swap_a = (int*) malloc(n * sizeof(int));
        int *swap_b = (int*) malloc(n * sizeof(int));
        for(int p = 0; p < n; p++)
        {
            int *Ap = (int*) malloc(A_size * sizeof(int));
            int *Bp = (int*) malloc(B_size * sizeof(int));
            for (int i = 0, j = 0; i < A_size0; i++)
            {
                int flag = 1;
                /* заполняем Ap элементами, которые не попали в перестановку */
                for (int k = 0; k < p; k++)
                {
                    if (A0[i] == swap_a[k])
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    Ap[j] = A0[i];
                    j++;
                }
            }
            for (int i = 0, j = 0; i < B_size0; i++)
            {
                int flag = 1;
                /* заполняем Bp элементами, которые не попали в перестановку */
                for (int k = 0; k < p; k++)
                {
                    if (B0[i] == swap_b[k])
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    Bp[j] = B0[i];
                    j++;
                }
            }
            int g_max = INT_MIN, a_max = 0, b_max = 0;
            // printf("SIZES: %d %d\n", A_size, B_size);
            for(int a = 0; a < A_size; a++){
                for(int b = 0; b < B_size; b++){
                    int g_tmp = D[Ap[a]] + D[Bp[b]] - 2 * getByTown(m, subcopy[Bp[b]].t.name, subcopy[Ap[a]].t.name);
                    if (g_max < g_tmp)
                    {
                        g_max = g_tmp;
                        a_max = a;
                        b_max = b;
                    }
                    // printf("%d %d: \n", D[Ap[a]], D[Bp[b]]);
                }
            }
            g[p] = g_max;
            // printf("JOPA %d %d\n", p, g[p]);
            swap_a[p] = Ap[a_max];
            swap_b[p] = Bp[b_max];
            // printf("Droped Towns: %d %d\n", subcopy[Ap[a_max]].t.name, subcopy[Bp[b_max]].t.name);

            for(int x = 0; x < A_size; x++){
                if (x != a_max)
                {
                    D[Ap[x]] = D[Ap[x]] + 2 * getByTown(m, subcopy[Ap[x]].t.name, subcopy[Ap[a_max]].t.name) - 2 * getByTown(m, subcopy[Ap[x]].t.name, subcopy[Bp[b_max]].t.name);
                    // printf("%d \n", D[Ap[x]]);
                }

            }

            for(int y = 0; y < B_size; y++){
                if (y != b_max)
                {
                    D[Bp[y]] = D[Bp[y]] + 2 * getByTown(m, subcopy[Bp[y]].t.name, subcopy[Bp[b_max]].t.name) - 2 * getByTown(m, subcopy[Bp[y]].t.name, subcopy[Ap[a_max]].t.name);
                    // printf("%d \n", D[Bp[y]]);
                }
            }
            A_size--;
            B_size--;
        }
        int k = -1, G_max = 0, G = 0;
        for(int p = 0; p < n; p++){
            G += g[p];
            if (G_max < G)
            {
                G_max = G;
                k = p + 1;
            }
        }
        // free(g);
        if (G_max > 0) 
        {
            // printf("G_MAX: %d\n", G_max);
            for (int i = 0; i < k; i++)
            {
                int j, t;
                for(j = 0; j < A_size0; j++){
                    if(A0[j] == swap_a[i]){
                        // printf("NICEEEEE\n");
                        break;
                    }
                }
                for(t = 0; t < B_size0; t++){
                    if(B0[t] == swap_b[i]){
                        // printf("BAAAADDDDD\n");
                        break;
                    }
                }
                int tmp = A0[j];
                A0[j] = B0[t];
                B0[t] = tmp;
            }
        }
        else
        {
            // printf("TOUR FINISHED\n");
            break;
        }
        // printf("GACHIII!\n");
    }

    for (int i = 0; i < lenSub; i++)
    {
        if (i < A_size0)
        {
            sub[i] = subcopy[A0[i]];
        }
        else
        {
            sub[i] = subcopy[B0[i - A_size0]];
        }
    }

    double best = subtourdistanceTw(sub, lenSub, m, *timer, endTime);
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

double lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2)
{
    /* 1 Create starting tour T */
    //printf("LENSUB: %d\n", lenSub );
    Edge *T = (Edge *) calloc(lenSub, sizeof(Edge));
    Edge *T1 = (Edge *) calloc(lenSub, sizeof(Edge));
    int *indexes = (int *) calloc(20, sizeof(int)); /* число всех городов */
    double FT = 0, FT1 = 0;
    int neighbours[lenSub][2];
    for (int town = 0; town < lenSub - 1; ++town)
    {
        neighbours[town][1] = sub[town + 1].t.name;
        neighbours[town + 1][0] = sub[town].t.name; 
        T[town] = edge_init(sub[town].t.name, sub[town + 1].t.name);
        FT += getByTown(m, sub[town].t.name, sub[town + 1].t.name);
        indexes[sub[town].t.name] = town;
    }
    T[lenSub - 1] = edge_init(sub[lenSub - 1].t.name, sub[0].t.name);
    neighbours[0][0] = sub[lenSub - 1].t.name;
    neighbours[lenSub - 1][1] = sub[0].t.name;
    /* for (int i = 0; i < lenSub; ++i)
    {
        printf("%d %d %d\n", sub[i].t.name, neighbours[i][0], neighbours[i][1]);
    } */
    FT += getByTown(m, sub[lenSub - 1].t.name, sub[0].t.name);
    //FT = subtourdistanceTw(sub, lenSub, m, *timer, endTime);

    
    /* 2 Choose t1*/
    for (int t1 = 0; t1 < lenSub; ++t1)
    {
        bool flag6to2 = 0;
        Edge *X = (Edge *) calloc(lenSub, sizeof(Edge));
        Edge *Y = (Edge *) calloc(lenSub, sizeof(Edge));
        int t1_var = T[t1].node1;

        /* 3 choose X[0] */
        int tmp = (t1 == 0 ? T[lenSub - 1].node1 : T[t1-1].node1);
        int t2[2] = {tmp, T[t1].node2};
        for (int t2_var = 0; t2_var < 2; t2_var++)
        {
            X[0] = edge_init(t1_var, t2[t2_var]);

            /* 4  Choose Y[0] */
            int t3 = -1;
            int flag4 = 0;
            for (int town = 0; town < lenSub; ++town)
            {
                t3 = sub[town].t.name;
                if(t3 == neighbours[indexes[t2[t2_var]]][0] || t3 == neighbours[indexes[t2[t2_var]]][1] || t3 == t2[t2_var])
                {
                    continue;
                }
                Y[0] = edge_init(t2[t2_var], t3);
                double g0 = getByTown(m, X[0].node1, X[0].node2) - getByTown(m, Y[0].node1, Y[0].node2); /* g0 = |X[0]| - |Y[0]| */
                if (g0 > 0)
                {
                    flag4 = 1;
                }
                else
                {
                    continue;
                }

                /* 5 */
                for (int i = 1; i < lenSub; ++i)
                {
                    //printf("\n%d %d %d %d %d\n\n", i, X[i-1].node1, X[i-1].node2, Y[i-1].node1, Y[i-1].node2);
                    /* form var for step 6 */
                    int t2i[2] = {-1, -1};
                    if (neighbours[indexes[Y[i - 1].node2]][0] == t1_var)
                        t2i[0] = neighbours[indexes[Y[i - 1].node2]][1];
                    else if (neighbours[indexes[Y[i - 1].node2]][1] == t1_var)
                        t2i[0] = neighbours[indexes[Y[i - 1].node2]][0];
                    else
                    {
                        t2i[0] = neighbours[indexes[Y[i - 1].node2]][0];
                        t2i[1] = neighbours[indexes[Y[i - 1].node2]][1];
                    }
                    int xi_node1 = Y[i - 1].node2; /* t_(2i-1) */
                    /* 6 Choose X[i] */
                    //printf("%d %d %d %d\n", i, Y[i - 1].node2, neighbours[indexes[Y[i - 1].node2]][0], neighbours[indexes[Y[i - 1].node2]][1]);
                    for (int t2i_var = 0; t2i_var < 2 && t2i[t2i_var] != -1; ++t2i_var)
                    {
                        X[i] = edge_init(xi_node1, t2i[t2i_var]);
                        Y[i] = edge_init(t2i[t2i_var], t1_var); /* join t2i t1 */
                        if(xi_node1 == t1_var)
                            continue;
                        
                        /* 6a create T1 and check T1 is tour */
                        T1 = (Edge *) calloc(lenSub, sizeof(Edge));
                        bool direction = 1;
                        for (int e_new = 0, e_old = 0; e_new < lenSub; ++e_new)
                        {
                            bool flag6a = 1;
                            int x;
                            for (x = 0; x <= i; ++x)
                            {
                                if (edge_equal(T[e_old], X[x]))
                                {
                                    flag6a = 0;
                                    break;
                                }
                            }
                            if (flag6a) /* не меняем ребро */
                            {
                                T1[e_new] = T[e_old];
                                if (direction)
                                {
                                    e_old = e_old == lenSub - 1 ? 0 : e_old + 1;
                                }
                                else
                                {
                                    e_old = e_old == 0 ? lenSub - 1 : e_old - 1;
                                }
                            }
                            else /* меняем ребро */
                            {
                                T1[e_new] = Y[x];
                                for (int neighb = 0; neighb < 2; ++neighb)
                                {
                                    Edge T_new = edge_init(Y[x].node2, neighbours[indexes[Y[x].node2]][neighb]);
                                    int flag6a2 = 1;
                                    for (int n = 0; n <= i; ++n)
                                    {
                                        if (edge_equal(T_new, X[n]))
                                        {
                                            flag6a2 = 0;
                                            break;
                                        }
                                    }
                                    if (flag6a2)
                                    {
                                        int edge = 0;
                                        for(; edge < lenSub; ++edge)
                                        {
                                            if(edge_equal(T_new, T[edge]))
                                            {
                                                e_old = edge;
                                                break;
                                            }
                                        }
                                        if (e_old == edge)
                                        {
                                            direction = indexes[T_new.node2] > indexes[T_new.node1];
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        //printf("here 6a\n");
                        
                        bool flag6a_2 = 1;
                        /* Проверяем, что всех городов по два */
                        for (int town = 0; town < lenSub; ++town)
                        {
                            int cntr = 0;
                            for (int e = 0; e < lenSub; ++e)
                            {
                                if(T1[e].node1 == sub[town].t.name || T1[e].node2 == sub[town].t.name)
                                    cntr++;
                            }
                            if (cntr > 2)
                            {
                                flag6a_2 = 0;
                                break;
                            }
                        }
                        if(!flag6a_2)
                        {
                            printf("BAD TOUR\n");
                            continue;
                        }
                        //printf("6a good\n");
                        //printf("\nNEW %d %d %d %d %d\n\n", i, X[i].node1, X[i].node2, Y[i].node1, Y[i].node2);
                        /* 6b X[i] not in Y*/
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
                        
                        //printf("here 6b\n");
                    
                        /* 6 end */
                        FT1 = 0;
                        twtown* sub_temp = calloc(lenSub, sizeof(twtown));
                        /* if (flag6b)
                        {
                            for (int i = 0; i < lenSub; ++i)
                            {
                                printf("(%d, %d) ", T1[i].node1, T1[i].node2);
                            }
                            printf("\n");
                        } */
                        for (int e = 0; e < lenSub; ++e)
                        {
                            for(int t = 0; t < lenSub; ++t)
                            {
                                if(sub[t].t.name == T1[e].node1)
                                {
                                    sub_temp[e] = sub[t];
                                    break;
                                }
                            }
                            FT1 += getByTown(m, T1[e].node1, T1[e].node2);
                            //FT1 = subtourdistanceTw(sub_temp, lenSub, m, *timer, endTime);
                        }
                        FT1 += getByTown(m, T1[lenSub-1].node2, T1[0].node1);
                        //printf("%lf %lf", FT1, FT);
                        if (FT1 < FT)
                        {
                            for (int e = 0; e < lenSub; ++e)
                            {
                                T[e] = T1[e];
                                sub[e] = sub_temp[e];
                                neighbours[indexes[e]][1] = T1[e].node2;
                                neighbours[indexes[e + 1]][0] = T1[e].node1;
                            }
                            neighbours[indexes[lenSub - 1]][1] = T1[lenSub].node2;
                            neighbours[indexes[0]][0] = T1[lenSub - 1].node1;
                            flag6to2 = 1;
                            //printf("6to2\n");
                            break; /* 6to2 break */
                        }
                        else
                        {
                            flag6to2 = 0;
                        }
                        //printf("6 FINISH\n");
                        //printf("7 START\n");
                        /* 7 Choose Y[i] */
                        bool flag7 = 0;
                        for (int j = 0; j < lenSub; ++j)
                        {
                            printf("%d %d\n", sub[j].t.name, X[i].node2);
                            int yinode2 = sub[j].t.name; /* t_(2i+1) */
                            Y[i] = edge_init(X[i].node2, yinode2);
                            if(yinode2 == neighbours[X[i].node2][0] || yinode2 == neighbours[X[i].node2][1] || yinode2 == X[i].node2){
                                continue; /* 8 */ 
                            }

                            /* 7a */
                            double Gi = 0;
                            for (int r = 0; r <= i; ++r)
                            {
                                Gi += gain(m, X[r], Y[r]);
                            }
                            if (Gi <= 0)
                                continue; /* 8 */

                            /* 7b Y[i] not in X */
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
                            for(int neigh = 0; neigh < 2; ++neigh)
                            {
                                X[i + 1] = edge_init(yinode2, neighbours[yinode2][neigh]);
                                for (int h = 0; h < lenSub; ++h)
                                {
                                    if (edge_equal(T[h], X[i + 1]))
                                    {
                                        flag7c = 1;
                                        break;
                                    }
                                }
                                if(flag7c)
                                {
                                    for (int s = 0; s <= i; ++s)
                                    {
                                        if (edge_equal(X[s], X[i+1]))
                                        {
                                            flag7c = 0;
                                            break;
                                        }
                                    }       
                                    if(flag7c)
                                    {
                                        break;  
                                    }
                                }
                            }
                            if (Gi > 0 &&  flag7b && flag7c)
                            {
                                flag7 = 1;
                                break;
                            }

                            /* 8 */
                        }
                        //printf("7 FINISH\n");
                        if (flag7)
                            break;

                    } /* 6 end */
                    //printf("6 END\n");
                    if (flag6to2)
                        break;
                    
                    /* 9 */

                } /* 5 end */
                //printf("5 END\n");
                if (flag6to2)
                    break;

                /* 10 */

            } /* 4 end */
            //printf("4 END\n");
            if(flag4 == 0)
            {
                break;
            }
            if (flag6to2)
            {
                //printf("change tour\n");
                break;
            }

            /* 11 */

        } /* 3 end */

        /* 12 */

    } /* 2 end */

    //printf("HELLO\n");
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

double saTw(twtown *sub, int lenSub, halfmatrix *m, double* timer, const double endTime, double tmax, double tmin) {
    twtown subcopy[lenSub];
    //цикл копирования sub -> subcopy
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        // printf("subcopy[%d]: %d\n", i, sub[i].t.name);
    }

    double best = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime), newd, p;
    // if(best != 0 && best != -1)
    //     printf("\nSTART %lf\t%lf\n", best, 0.0);
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