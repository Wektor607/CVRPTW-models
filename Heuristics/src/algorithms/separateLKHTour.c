#include <stdio.h>
#include "logistic.h"
#include <stdlib.h>
#include <limits.h>

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


