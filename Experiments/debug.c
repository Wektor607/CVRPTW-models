#include <stdio.h>
#define my_min(x, y) (((x) > (y)) ? (y) : (x))
#define my_max(x, y) (((x) < (y)) ? (y) : (x))
#include <bits/stdc++.h>

int main()
{
    
    int sub[7] = {0, 1, 2, 3, 4, 5, 6};  
    int sub_copy[7];
    int lenSub = 7;
    //цикл копирования sub -> subcopy
    printf("sub_old: \n");
    for(int i = 0; i < lenSub; i++)
    {
        sub_copy[i] = sub[i];
        printf("%d ", sub[i]);
    }
    
    printf("\n");
    // int indexA = 1;//rand() % lenSub;
    // int indexB = 6;//rand() % lenSub;
    // // while(indexA == indexB){
    // //     indexB = rand() % lenSub;
    // // }

    // for(int i = 0; i < my_min(indexA,indexB); i++) 
    // {
    //     sub[i] = sub_copy[i];
    // }

    // for(int i = 0; i < my_max(indexA, indexB) - my_min(indexA, indexB) + 1; i++) 
    // { 
    //     sub[my_min(indexA, indexB) + i] = sub_copy[my_max(indexA, indexB) - i]; 
    // }

    // for(int i = my_max(indexA, indexB) + 1; i < lenSub; i++) 
    // {
    //     sub[i] = sub_copy[i];
    // }
    
    printf("sub_new: \n");
    for(int i = 0; i < lenSub; i++)
    {
        printf("%d ", sub[i]);
    }
    
    printf("\n");
    // free(sub_copy);
    return 0;
}