#include "omp.h"
#include <stdio.h>
#include <math.h>

N = 8;

int  main(){
    int step,logN;
    int num[N];
    for (int i = 0; i < N; i++)
    {
        num[i] = i+1;
    }
    logN = (int)log2(N);
    int pid, tmp;
    printf("numbers:");
    for (int i = 0; i < N; i++)
    {
        printf("%d", &num[i]);
        if(i == N-1)
            printf("\n");
    }
    for (int i = 0; i < logN; i++)
    {
        step = (int)pow(2, i);
#pragma omp parallel private(pid, tmp)
        {
            pid = omp_get_thread_num();
            tmp = num[pid];
            if (pid + step < step*2)
            {
                tmp = num[pid] + num[pid+step];
            }
            else{
                tmp = num[pid] + num[pid-step];
            }
            num[pid] = tmp;
#pragma omp barrier
        }
    }
}