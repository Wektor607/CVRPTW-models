#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "LKH.h"

int two_opt = 0; 
int three_opt = 0; 
int k_opt = 0;

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

void print_tour(Edge *T, int n)
{
    for(int e = 0; e < n; ++e)
    {
        ////////printf("%d---%d, ", T[e].node1, T[e].node2);
    }
    ////////printf("\n");
}

bool compair_idx(int idx1, int idx2, int lenSub)
{
    return (idx1 == 0 && idx2 == 1) || (idx1 == lenSub - 1 && idx2 == 0) || (idx1 > 0 && idx1 < lenSub - 1 && idx1 + 1 == idx2);
}

int minus(int idx, int n)
{
    if (!idx)
        return n - 1;
    return idx - 1;
}


void new_tour_create(halfmatrix *m, Edge *T_old, Edge *T_new, twtown *sub, int lenSub, Edge *X, Edge *Y, int i, int *indexes, int *Xindex)
{
    //////printf("2Opt\n");

    if (i == 1) // 2-opt 
    {
        twtown sub_temp[lenSub];
        for (int j = 0; j < lenSub; ++j)
        {
            sub_temp[j] = sub[j];
            T_new[j] = T_old[j];
        }
        int idx1 = my_min(indexes[X[0].node1], indexes[X[0].node2]),
            idx2 = my_min(indexes[X[1].node1], indexes[X[1].node2]);

        if (abs(idx1 - idx2) > 1)
        {
            reverseTownTw(sub_temp, my_min(idx1, idx2), my_max(idx1, idx2));
            for (int j = 0; j < lenSub-1; ++j)
            {
                T_new[j] = edge_init(sub_temp[j].t.name, sub_temp[j + 1].t.name);
            }
            T_new[lenSub-1] = edge_init(sub_temp[lenSub-1].t.name, sub_temp[0].t.name);
        }

        return;
    }

    //////printf("3Opt\n");
    if(i == 2 && lenSub > 3) // 3-opt
    {
        twtown sub_temp[lenSub];
        for (int j = 0; j < lenSub; ++j)
        {
            sub_temp[j] = sub[j];
            T_new[j] = T_old[j];
        }
        //////printf("INDEXES\n");
        int min = my_min(my_min(indexes[X[0].node1], indexes[X[1].node1]), indexes[X[2].node1]);
        int max = my_max(my_max(indexes[X[0].node1], indexes[X[1].node1]), indexes[X[2].node1]);
        int central = 0;
        int flag_central = 0;
        //////printf("%d\n", lenSub);
        for(int r = 0; r < 3; ++r)
        {
            if(indexes[X[r].node1] < max && indexes[X[r].node1] > min)
            {
                central = indexes[X[r].node1];
                flag_central = 1;
                break;
            }
        }
        if(flag_central == 0)
            return;
            
        //////printf("IF\n");
        if((abs(max - min) > 1) + (abs(max - central) > 1) + (abs(central - min) > 1) > 1)
        {
            // //printf("START FUNCTION\n");
            //////printf("MIN: %d, CENTRAL:%d, MAX:%d\n", min, central, max);
            reverse_segment_if_better(m, sub_temp, min, central, max, lenSub);
            //////printf("END FUNCTION\n");
            for (int j = 0; j < lenSub-1; ++j)
            {
                //////printf("first: %d\n", sub_temp[j].t.name); 
                //////printf("second: %d\n", sub_temp[j + 1].t.name);
                T_new[j] = edge_init(sub_temp[j].t.name, sub_temp[j + 1].t.name);
            }
            //////printf("END CYCLE\n");
            T_new[lenSub-1] = edge_init(sub_temp[lenSub-1].t.name, sub_temp[0].t.name);
            //////printf("END IF\n");
        }
        //////printf("3OPT END\n");
        return;
    }

    //////printf("KOpt\n");
    bool direction = 1;
    for (int e_new = 0, e_old = 0; e_new < lenSub; ++e_new)
    {
        bool change_edge = 0;
        if (Xindex[e_old] != -1 && edge_equal(T_old[e_old], X[Xindex[e_old]]))
        {
            change_edge = 1;
        }
        
        if (!change_edge) /* не меняем ребро */
        {
            if (direction)
            {
                T_new[e_new] = edge_init(T_old[e_old].node1, T_old[e_old].node2);
                e_old = (e_old + 1) % lenSub;
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
            int y = 0;
            int cur_town = 0;
            /* Ищем Y, который будет следующим ребром в T_new и запоминаем второй его конец*/
            for (; y <= i; ++y)
            {
                if (Y[y].node1 == T_new[e_new - 1].node2)
                {
                    T_new[e_new] = Y[y];
                    cur_town = Y[y].node2;
                    break;
                }
                if (Y[y].node2 == T_new[e_new - 1].node2)
                {
                    T_new[e_new] = edge_init(Y[y].node2, Y[y].node1);
                    cur_town = Y[y].node1;
                    break;
                }
            }
            /* Цикл работающий до тех пор пока в T_new записываем подряд Y или пока не закончится тур */
            while(e_new < lenSub-1)
            {
                int flag_neighb = 0;
                int cur_idx = indexes[T_new[e_new].node2];
                int cur_neighb[2] = {T_old[minus(cur_idx, lenSub)].node1, T_old[(cur_idx + 1) % lenSub].node1};
                for (int neighb = 0; neighb < 2; ++neighb)
                {
                    /* T_tmp - два вариант ребер на старом туре, которые соединены с Y */
                    Edge T_tmp = edge_init(T_new[e_new].node2, cur_neighb[neighb]);
                    int flag = 1;
                    /* Проверяем ялвяется ли T_tmp теми ребрами, которые необходимо из тура */
                    for (int n = 0; n <= i; ++n)
                    {
                        if (edge_equal(T_tmp, X[n]))
                        {
                            flag = 0;
                            break;
                        }
                    }
                    
                    // Поиск номера следующего ребра в старом туре и проверка направления относительно старого тура
                    if (flag)
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
                            int idx1 = indexes[T_tmp.node1], idx2 = indexes[T_tmp.node2];  
                            direction = compair_idx(idx1, idx2, lenSub);
                        }
                        flag_neighb = 1;
                        break;
                    }
                }
                /* Оба соседа оказались X */
                if(!flag_neighb)
                {
                    for(int r = 0; r <= i; ++r)
                    {
                        if(Y[r].node1 == cur_town && y != r)
                        {
                            e_new++;
                            T_new[e_new] = edge_init(Y[r].node1, Y[r].node2);
                            y = r;
                            cur_town = Y[r].node2;
                            break;
                        }
                        if(Y[r].node2 == cur_town && y != r)
                        {
                            e_new++;
                            T_new[e_new] = edge_init(Y[r].node2, Y[r].node1);
                            y = r;
                            cur_town = Y[r].node1;
                            break;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
}

struct twoResults lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime, double zeroParam1, double zeroParam2, int countTowns)
{
    int opts = lenSub / 2;
    if(lenSub > 30)
    {
        opts = 15;
    }

    depoShift(lenSub, sub);
    /* 1 Create starting tour T */
    Edge T[lenSub]; // ребра тура, который является наилучшим из найденных в текущий момент
    Edge X[lenSub];
    Edge Y[lenSub];
    twtown *sub_copy = (twtown*) malloc((lenSub) * sizeof(twtown));;
    int useflag8 = 0, useflag9 = 0, ynoty = 0;
    int Xindex[lenSub];
    int indexes[countTowns]; /* Индексы городов в sub_copy. Массив размера числа всех городов, включая выкинутые */
    double FT = 0, FT1 = 0;
    for (int town = 0; town < lenSub; ++town)
    {
        sub_copy[town] = sub[town];
        Xindex[town] = -1;
        if(town < lenSub-1)
        {
            T[town] = edge_init(sub[town].t.name, sub[town + 1].t.name);   
        }
        indexes[sub[town].t.name] = town;
    }
    T[lenSub - 1] = edge_init(sub[lenSub - 1].t.name, sub[0].t.name); // замыкание тура
    
    struct twoResults tr = subtourdistanceTw(sub, lenSub, m, *timer, endTime);
    FT = tr.localtimer;
    bool new_tour = 1; // данный флаг показывает был ли найден более оптимальный тур во время одной итерации while
    while(new_tour)
    {
        new_tour = 0;
        /* 2 Choose t1*/
        for (int t1_idx = 0; t1_idx < lenSub; ++t1_idx)
        {
            int t1_var = T[t1_idx].node1;
            /* 3 choose X[0] */
            
            int t2[2] = {T[minus(t1_idx, lenSub)].node1, T[(t1_idx + 1) % lenSub].node1}; // соседи города t1_var
            for (int t2_var = 0; t2_var < 2; t2_var++)
            {
                if(t1_var == t2[t2_var])
                {
                    continue;
                }
                X[0] = edge_init(t1_var, t2[t2_var]);
                int t2_idx = indexes[t2[t2_var]];
                if(compair_idx(t1_idx, t2_idx, lenSub))
                {
                    Xindex[t1_idx] = 0;
                }
                else
                {
                    Xindex[t2_idx] = 0;
                }
                /* 4  Choose Y[0] */
                
                for (int town = 0; town < lenSub; ++town)
                {
                    int t3 = sub_copy[town].t.name;
                    // t_3 не совпадает с соседями t2_var и с самим t2_var
                    if(indexes[t3] == minus(t2_idx, lenSub) || indexes[t3] == (t2_idx + 1) % lenSub || t3 == t2[t2_var])
                    {
                        continue;
                    }
                    Y[0] = edge_init(t2[t2_var], t3);
                    double g0 = getByTown(m, X[0].node1, X[0].node2) - getByTown(m, Y[0].node1, Y[0].node2); /* g0 = |X[0]| - |Y[0]| */
                    if (g0 <= 0)
                    {
                        continue;
                    }
                    
                    bool flag8 = 0, flag9 = 0;
                    int y2_alt = -1, x2_alt = -1;
                    /* 5 После выбора X[0] и Y[0] делаем i = i + 1*/
                    
                    for (int i = 1; i < opts; ++i)
                    {
                        // //printf("HELLO\n");
                        if(flag8 || flag9)
                        {
                            i = 1;
                        }
                        if(!flag8) /* Если Y[i] существует переходим к шагу 5 в противном случае откатываемся к Y[1]*/
                        {
                            /* form var for step 6 */
                            int prev_idx = indexes[Y[i - 1].node2]; /* index of t_(2i-1) */
                            // соседи t_(2i-1) 
                            int t2i[2] = {T[minus(prev_idx, lenSub)].node1, T[(prev_idx + 1) % lenSub].node1};
                            int xi_node1 = Y[i - 1].node2; /* t_(2i-1) */
                            /* 6 Choose X[i] */
                            
                            int t2i_var = 0;
                            if(i == 1 && flag9) {
                                t2i_var = (x2_alt != -1) ? 1 : 2;
                                x2_alt = -1;
                            }
                            bool flag6 = 0;
                            for (; t2i_var < 2; ++t2i_var)
                            {
                                
                                if(xi_node1 == t2i[t2i_var])
                                {
                                    continue;
                                }
                                X[i] = edge_init(xi_node1, t2i[t2i_var]);
                                
                                int flag_was = 0;
                                for (int s = 0; s < i; ++s)
                                {
                                    if (edge_equal(X[s], X[i]))
                                    {
                                        flag_was++;
                                        break;
                                    }
                                }
                                if (flag_was) {continue;}
                                int xi_idx1 = indexes[xi_node1];
                                int t2i_idx = indexes[t2i[t2i_var]];
                                if(compair_idx(xi_idx1, t2i_idx, lenSub))
                                {
                                    Xindex[xi_idx1] = i;
                                }
                                else
                                {
                                    Xindex[t2i_idx] = i;
                                }
                                
                                if(t2i[t2i_var] != t1_var)
                                {
                                    Y[i] = edge_init(t2i[t2i_var], t1_var); /* join t2i t1 */
                                }
                                else
                                {
                                    continue;
                                }
                                
                                /* 6a create T1 and check T1 is tour */
                                Edge T1[lenSub]; // ребра тура кандитата на новый наилучший тур
                                new_tour_create(m, T, T1, sub_copy, lenSub, X, Y, i, indexes, Xindex);
                                bool flag6a = 1;
                                /* Проверяем, что всех городов по два */
                                for (int cur_town = 0; cur_town < lenSub; ++cur_town)
                                {
                                    int cntr = 0;
                                    for (int e = 0; e < lenSub; ++e)
                                    {
                                        if(T1[e].node1 == sub_copy[cur_town].t.name || T1[e].node2 == sub_copy[cur_town].t.name)
                                            cntr++;
                                        
                                        if (cntr > 2 || (e < lenSub - 1 && T1[e].node2 != T1[e+1].node1))
                                        {
                                            flag6a = 0;
                                            break;
                                        }
                                    }
                                    if(flag6a == 0)
                                        break;
                                }

                                if(!flag6a || T1[lenSub - 1].node2 != T1[0].node1)
                                {
                                    /* bad tour T1 */
                                    continue;
                                }
                                
                                FT1 = 0;
                                twtown sub_temp[lenSub];
                                for (int e = 0; e < lenSub; ++e)
                                {
                                    sub_temp[e] = sub_copy[indexes[T1[e].node1]];
                                }
                                tr = subtourdistanceTw(sub_temp, lenSub, m, *timer, endTime);
                                FT1 = tr.localtimer;
                                
                                /* Если найденный тур лучше лучшего,то заменяем*/
                                if ((FT1 < FT || FT == -1) && FT1 != -1)
                                {
                                    if(i == 1)
                                        two_opt++;
                                    else if(i == 2)
                                        three_opt++;
                                    else if(i > 2)
                                        k_opt++;
                                    for (int e = 0; e < lenSub; ++e)
                                    {
                                        Xindex[e] = -1;
                                        T[e] = T1[e];
                                        sub_copy[e] = sub_temp[e];
                                        indexes[sub_temp[e].t.name] = e;
                                    }
                                    FT = FT1;
                                    new_tour = 1;
                                    break; /* 6to2 break */
                                }
                                
                                new_tour = 0;
                                flag6 = 1;
                                break;
                            } /* 6 end */
                        
                            if(!flag6)
                            {
                                break;
                            }
                            flag9 = 0;
                            if (new_tour)
                            {
                                break;
                            }
                            /* подготовка к шагу 9 */
                            if (i == 1)
                            {
                                x2_alt = -1;
                                if(t2i_var == 0)
                                {
                                    x2_alt = t2i_var + 1;
                                }
                            }
                        }

                        /* 7 Choose Y[i] */
                        bool flag7 = 0;
                        int j = 0;
                        if(i == 1 && flag8) {
                            j = (y2_alt != -1) ? y2_alt : lenSub;
                            y2_alt = -1;
                            flag8 = 0;
                        }
                        for (; j < lenSub; ++j)
                        {
                            int yinode2 = sub_copy[j].t.name; /* t_(2i+1) */
                            int next_idx = indexes[X[i].node2];
                            if (indexes[yinode2] == minus(next_idx, lenSub) || indexes[yinode2] == (next_idx + 1) % lenSub || yinode2 == X[i].node2)
                            {
                                continue; /* 8 */ 
                            }

                            Y[i] = edge_init(X[i].node2, yinode2);
                            bool flag7_equal = 0;
                            for (int s = 0; s < i; ++s)
                            {
                                if (edge_equal(Y[i], Y[s]))
                                {
                                    flag7_equal = 1;
                                    ynoty++;
                                    break;
                                }
                            }
                            if(flag7_equal)
                            {
                                continue;
                            }

                            /* 7a */
                            double Gi = 0;
                            for (int r = 0; r <= i; ++r)
                            {
                                Gi += gain(m, X[r], Y[r]);
                            }
                            if (Gi <= 0)
                                continue; /* 8 */

                            /* 7c */
                            //printf("7C START\n");
                            // fflush(stdout);
                            int flag7c = 0;
                            // cur_neigh это соседи yinode2
                            int cur_neigh[2] = {T[minus(indexes[yinode2], lenSub)].node1, T[(indexes[yinode2] + 1) % lenSub].node1};
                            for(int neigh = 0; neigh < 2; ++neigh)
                            {
                                X[i + 1] = edge_init(yinode2, cur_neigh[neigh]);
                                for (int s = 0; s <= i; ++s)
                                {
                                    if (edge_equal(X[s], X[i+1]))
                                    {
                                        flag7c++;
                                        break;
                                    }
                                }
                                // если оба ребра, включающих yinode2 уже иксы, то мы отбрасываем текущий Y[i]
                                if(flag7c == 2)
                                {
                                    break;
                                }
                            }
                            //printf("7C END\n");
                            // fflush(stdout);
                            
                            if (Gi > 0 && flag7c < 2)
                            {
                                flag7 = 1;
                                break;
                            }
                        } /* 7 end */
                        //printf("7 END\n");
                        // fflush(stdout);

                        /* подготовка к 8 шагу заранее */
                        if(i == 1)
                        {
                            y2_alt = -1;
                            if(j < lenSub - 1 && flag7)
                            {
                                y2_alt = j + 1;
                            }
                        }
                        /* 8 */
                        if(!flag7)
                        {
                            if (y2_alt != -1)
                            {
                                flag8 = 1;
                                useflag8++;
                                for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
                                { 
                                    if(Xindex[x_idx] != 0 && Xindex[x_idx] != 1) 
                                        Xindex[x_idx] = -1;  
                                }
                                continue;
                            }

                            /* 9 */
                            if (x2_alt != -1)
                            {
                                flag9 = 1;
                                useflag9++;
                                y2_alt = -1;
                                for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
                                { 
                                    if(Xindex[x_idx] != 0) 
                                        Xindex[x_idx] = -1;  
                                }
                                continue; 
                            }
                            /* 10 */
                            break;
                        }
                        //////printf("8 END\n");
                    } /* 5 end */
                    //printf("5 END\n");

                    if (new_tour)
                        break;
                    for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
                    { 
                        if(Xindex[x_idx] != 0) 
                            Xindex[x_idx] = -1;  
                    }
                } /* 4 end */
                ////////printf("4 END\n");

                if (new_tour)
                    break;
                for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
                { 
                    Xindex[x_idx] = -1;  
                }
                    
            } /* 3 end */

            if (new_tour)
            {
                break;
            }
            for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
            { 
                Xindex[x_idx] = -1;  
            }
        } /* 2 end */
        for(int x_idx = 0; x_idx < lenSub; ++x_idx) 
        { 
            Xindex[x_idx] = -1;  
        }
    }

    tr = subtourdistanceTw(sub_copy, lenSub, m, *timer, endTime);
    double best = tr.localtimer;
    if(best != -1)
    {
        for(int p = 0; p < lenSub; ++p)
        {
            sub[p] = sub_copy[p];
        }
    }
    free(sub_copy);
    return tr;
}