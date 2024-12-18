#include <stdlib.h>
#include <stdio.h>

int main(){
    int tabDistance[5] = {10, 15, 5, 20, 8};
    int distMin = 0;
    int min = tabDistance[0];
    for (int i = 1 ; i < 5 ; i++)
    {
        if (min >= tabDistance[i])
        {
            min = tabDistance[i];
            distMin = i;
        }
    }
    printf("min : %d", distMin);
}
