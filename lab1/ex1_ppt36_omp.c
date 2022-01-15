#include "omp.h"
#include <stdio.h>
#include <math.h>

int N = 8;

int  main(){
    int step,logN;
    int num[N];
    for (int i = 0; i < N; i++)
    {
        num[i] = i+1;
    }
    logN = (int)log2(N);
    int pid, tmp, dest;
    printf("numbers:");
    for (int i = 0; i < N; i++)
    {
        printf("%d ", num[i]);
        if(i == N-1)
            printf("\n");
    }
    for (int i = 0; i < logN; i++)
    {
        step = (int)pow(2, i);
#pragma omp parallel private(pid, tmp, dest)
        {
            pid = omp_get_thread_num();
            tmp = num[pid];
            dest = pid^step;
            tmp = num[pid] + num[dest];
#pragma omp barrier
            num[pid] = tmp;
#pragma omp barrier
        }
    }
    printf("after sum:");
    for (int i = 0; i < N; i++)
    {
        printf("%d ", num[i]);
        if(i == N-1)
            printf("\n");
    }
}