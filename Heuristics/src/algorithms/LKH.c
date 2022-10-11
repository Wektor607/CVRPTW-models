#include <stdio.h>
#include <stdlib.h>

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

Edge *new_tour_create(Edge *T_old, int lenSub, Edge *X, Edge *Y, int i, int (*neighbours)[2], int *indexes)
{
    Edge *T_new = calloc(lenSub, sizeof(Edge));
    bool direction = 1;
    for (int e_new = 0, e_old = 0; e_new < lenSub; ++e_new)
    {
        bool change_edge = 0;
        for (int x = 0; x <= i; ++x)
        {
            if (edge_equal(T_old[e_old], X[x]))
            {
                change_edge = 1;
                break;
            }
        }
        if (!change_edge) /* не меняем ребро */
        {
            if (direction)
            {
                T_new[e_new] = edge_init(T_old[e_old].node1, T_old[e_old].node2);
                e_old = e_old == lenSub - 1 ? 0 : e_old + 1;
            }
            else
            {
                T_new[e_new] = edge_init(T_old[e_old].node2, T_old[e_old].node1);
                e_old = e_old == 0 ? lenSub - 1 : e_old - 1;
            }
        }
        else /* меняем ребро */
        {
            if (e_new == 0) /* не начинаем с измененённого ребра */
            {
                e_old++;
                e_new--;
                continue;
            }
            for (int y = 0; y <= i; ++y)
            {
                if (Y[y].node1 == T_new[e_new - 1].node2)
                {
                    T_new[e_new] = Y[y];
                    break;
                }
                if (Y[y].node2 == T_new[e_new - 1].node2)
                {
                    T_new[e_new] = edge_init(Y[y].node2, Y[y].node1);
                    break;
                }
            }
            for (int neighb = 0; neighb < 2; ++neighb)
            {
                Edge T_tmp = edge_init(T_new[e_new].node2, neighbours[indexes[T_new[e_new].node2]][neighb]);
                int flag6a2 = 1;
                for (int n = 0; n <= i; ++n)
                {
                    if (edge_equal(T_tmp, X[n]))
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
                        if(edge_equal(T_tmp, T_old[edge]))
                        {
                            e_old = edge;
                            break;
                        }
                    }
                    if (e_old == edge)
                    {
                        direction = indexes[T_tmp.node2] > indexes[T_tmp.node1];
                        break;
                    }
                }
            }
        }
    }
    return T_new;
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
        //FT += getByTown(m, sub[town].t.name, sub[town + 1].t.name);
        indexes[sub[town].t.name] = town;
    }
    T[lenSub - 1] = edge_init(sub[lenSub - 1].t.name, sub[0].t.name);
    neighbours[0][0] = sub[lenSub - 1].t.name;
    neighbours[lenSub - 1][1] = sub[0].t.name;
    /* for (int i = 0; i < lenSub; ++i)
    {
        printf("%d %d %d\n", sub[i].t.name, neighbours[i][0], neighbours[i][1]);
    } */
    //FT += getByTown(m, sub[lenSub - 1].t.name, sub[0].t.name);
    FT = subtourdistanceTw(sub, lenSub, m, *timer, endTime);
    // printf("FT start %lf\n", FT);

    
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
                        T1 = new_tour_create(T, lenSub, X, Y, i, neighbours, indexes);
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
                            //printf("BAD TOUR\n");
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
                            //FT1 += getByTown(m, T1[e].node1, T1[e].node2);
                        }
                        int start = 0;
                        for (int i = 0; i < lenSub; ++i)
                        {
                            if (T1[i].node1 == 0)
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
                                if (T1[i].node1 == sub_temp[j].t.name)
                                {
                                    if (i < start)
                                        subcopy[lenSub - start + i] = sub_temp[j];
                                    else
                                        subcopy[i - start] = sub_temp[j];
                                    break;
                                }
                            }
                        }

                        // for (int i = 0; i < lenSub; ++i)
                        // {
                        //     printf("%d ", subcopy[i].t.name);
                        // }
                        // printf("\n");
                        
                        FT1 = subtourdistanceTw(subcopy, lenSub, m, *timer, endTime);
                        //FT1 += getByTown(m, T1[lenSub-1].node2, T1[0].node1);
                        //printf("%lf %lf\n", FT1, FT);
                        if ((FT1 < FT || FT == -1) && FT1 != -1)
                        {
                            printf("FT1: %lf \n FT: %lf\n", FT1, FT);
                            indexes[subcopy[0].t.name] = 0;
                            for (int e = 0; e < lenSub; ++e)
                            {
                                T[e] = T1[e];
                                sub[e] = subcopy[e];
                                if (e < lenSub - 1)
                                {
                                    indexes[subcopy[e + 1].t.name] = e + 1;
                                    neighbours[indexes[e]][1] = T1[e].node2;
                                    neighbours[indexes[e + 1]][0] = T1[e].node1;
                                }
                            }
                            neighbours[indexes[lenSub - 1]][1] = T1[lenSub].node2;
                            neighbours[indexes[0]][0] = T1[lenSub - 1].node1;
                            flag6to2 = 1;
                            FT = FT1;
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
                            //printf("%d %d\n", sub[j].t.name, X[i].node2);
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
                        // printf("7 FINISH\n");
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

    double best = subtourdistanceTw(sub, lenSub, m, *timer, endTime);
    // if (best != -1)
    // {
        *timer += best;
    // }
    /* for (int i = 0; i < lenSub; ++i)
    {
        printf("%d ", sub[i].t.name);
    }
    printf("\n"); */
    // printf("FT final %lf\n", FT);
    return best;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////