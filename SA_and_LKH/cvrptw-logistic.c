#include <stdio.h>
#include "logistic.h"


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

double subtourdistanceTw(twtown *sub, int lenSub, halfmatrix* m, const double timer, const double endTime)
{
    if(lenSub == 0) 
    {
        return 0;
    }

    double localtimer = timer;
    double dop_time = 0;

    localtimer += getByTown(m, 0, sub[0].t.name) + sub[0].mTimeService;
    // printf("sub[0].mTimeService: %lf\n", sub[0].mTimeService);
    if(!(localtimer >= sub[0].mTimeStart))
    {
        // printf("FUCK: %lf %lf\n", localtimer, sub[0].mTimeStart);
        dop_time += sub[0].mTimeStart - localtimer;
        localtimer = sub[0].mTimeStart + sub[0].mTimeService; 
    }
    if(!(localtimer >= sub[0].mTimeStart && localtimer <= sub[0].mTimeEnd && localtimer <= endTime)) 
    {
        // printf("localtimer <= sub[0].mTimeEnd: %lf %lf %lf %d\n", localtimer, sub[0].mTimeStart, sub[0].mTimeEnd, localtimer <= sub[0].mTimeEnd);
        return -1;
    }

    for(int i = 0; i < lenSub-1; i++)
    {
        localtimer += getByTown(m, sub[i].t.name, sub[i+1].t.name) + sub[i+1].mTimeService;
        if(!(localtimer >= sub[i+1].mTimeStart))
        {
            dop_time += sub[i+1].mTimeStart - localtimer;
            localtimer = sub[i+1].mTimeStart + sub[i+1].mTimeService;
        }
        if(!(localtimer >= sub[i+1].mTimeStart && localtimer <= sub[i+1].mTimeEnd && localtimer <= endTime)) {
            return -1;
        }
    }

    localtimer += getByTown(m, 0, sub[lenSub-1].t.name);
    if(!(localtimer <= endTime)) 
    {
        return -1;
    }
    
    // Если нужно вычислить расстояние, то вычитаем суммарное время ожидания
    // Если же нужно получить общее время, затраченное на всю поездку, то не отнимаем это доп. время, но переводить в метры это время
    // не корректно
    return localtimer - timer - dop_time;
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

double lkhTw(twtown *sub, int lenSub, halfmatrix *m, double *timer, const double endTime){
    twtown *subcopy = (twtown*)malloc(lenSub * sizeof(twtown));
    int *A = (int*)malloc((lenSub  - 1)/ 2 * sizeof(int));
    int *B = (int*)malloc(((lenSub + 1)/ 2) * sizeof(int));
    int *D = (int*)malloc(lenSub * sizeof(int));
    int A_size0 = (lenSub  - 1) / 2, B_size0 = ((lenSub + 1)/ 2);
    int A_size = A_size0, B_size = B_size0;
    //цикл копирования sub -> subcopy
    
    for(int i = 0; i < lenSub; i++)
    {
        subcopy[i] = sub[i];
        if (i < A_size) {A[i] = i}
        else {B[i - A_size] = i}
    }
    while(1){
        for(int a = 0; a < A_size; a++){
            int I = 0, E = 0;
            for(int y = 0; y < B_size; y++)
            {
                E += getByTown(m, subcopy[A[a]].t.name, subcopy[B[y]].t.name);
            }
            for(int x = 0; x < A_size; x++)
            {
                I += getByTown(m, subcopy[A[a]].t.name, subcopy[A[x]].t.name);
            }
            D[A[a]] = E - I;
        }

        for(int b = 0; b < B_size; b++){
            int I = 0, E = 0;
            for(int y = 0; y < A_size; y++)
            {
                E += getByTown(m, subcopy[B[b]].t.name, subcopy[A[y]].t.name);
            }
            for(int x = 0; x < B_size; x++)
            {
                I += getByTown(m, subcopy[B[b]].t.name, subcopy[B[x]].t.name);
            }
            D[B[b]] = E - I;
        }

        int G = 0;
        int n = min(A_size0, B_size0);
        int *g = (int*)malloc(n * sizeof(int));
        for(int p = 0; p < n; p++){
            int g_max = 0, a_max = 0, b_max = 0;
            for(int a = 0; a < A_size; i++){
                for(int b = 0; b < B_size; b++){
                    int g_tmp = D[A[a]] + D[B[b]] - 2 * getByTown(m, subcopy[B[b]].t.name, subcopy[A[a]].t.name);
                    if (g_max < g_tmp)
                    {
                        g_max = g_tmp;
                        a_max = a;
                        b_max = b;
                    }
                }
            }
            g[p] = g_max;

            int tmp = A[a_max];
            A[a_max] = A[A_size - 1];
            A[A_size - 1] = tmp; 

            tmp = B[b_max];
            B[b_max] = B[B_size - 1];
            B[B_size - 1] = tmp; 

            A_size--;
            B_size--;

            for(int x = 0; x < A_size; x++){
                D[A[x]] = D[A[x]] + 2 * getByTown(m, subcopy[A[x]].t.name, subcopy[A[A_size]].t.name) - 2 * getByTown(m, subcopy[A[x]].t.name, subcopy[B[B_size]].t.name);
            }

            for(int y = 0; y < B_size; y++){
                D[B[y]] = D[B[y]] + 2 * getByTown(m, subcopy[B[y]].t.name, subcopy[B[B_size]].t.name) - 2 * getByTown(m, subcopy[B[y]].t.name, subcopy[A[A_size]].t.name);
            }
        }

        int k = -1, G_max = 0;
        for(int p = 0; p < n; p++){
            G += g[p];
            if (G_max < G)
            {
                G_max = G;
                k = p;
            }
        }

        if (G_max > 0) 
        {
            for (int i = 0; i < k; i++)
            {
                int tmp = A[A_size0 - i - 1];
                A[A_size0 - i - 1] = B[B_size0 - i - 1];
                B[B_size0 - i - 1] = tmp;
            }
        }
        else
        {
            break;
        }
    }


    
    


//     double best_subtour = 0;
//     double current_subtour = 0;
//     // if(best_subtour == 0) {
//     //     return -1;
//     // }

//    int k_set[2] = {2, 3};
//    int size_k_set = sizeof k_set / sizeof *k_set;

//     for(int k = 0; k < size_k_set; k++)
//     {
//         if(k_set[k] == 2)
//         {
//             // best_subtour = lkh2optTw(subcopy, lenSub, m, timer, endTime);  
//         }
//         else if(k_set[k] == 3)
//         {
//             // current_subtour = lkh3optTw(subcopy, lenSub, m, timer, endTime);
//             // if(current_subtour < best_subtour){best_subtour = current_subtour;}
//         }
//     }
//     free(subcopy);
    
//     // printf("New total time: %lf\n", best);
//     // printf("New list: "); printTwTownList(sub, lenSub);
//     if(best_subtour != -1) {
//         *timer += best_subtour;  
//     }
//     return best_subtour;
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
    
    if(best != -1) {
        *timer += best;
    }
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