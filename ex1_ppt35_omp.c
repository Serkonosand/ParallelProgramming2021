#include "omp.h"
#include <stdio.h>
#include <math.h>

N = 8;

int main(){
    int step,logN;
    int num[N];
    for (int i = 0; i < N; i++)
    {
        num[i] = i+1;
    }
    logN = (int)log2(N);
    int pid;
    printf("numbers:");
    for (int i = 0; i < N; i++)
    {
        printf("%d", num[i]);
        if(i == N-1)
            printf("\n");
    }
    for (int i = logN; i > 0; i--)
    {
        step = (int)pow(2, logN-i+1);
#pragma omp parallel private(pid)
        {
            pid = omp_get_thread_num();
            if (!(pid % step))
            {
                num[pid] = num[pid]+num[pid+step/2];
            }
#pragma omp barrier
        }
    }
    for (int i = 1; i <= logN; i++)
    {
        step = (int)pow(2, logN-i+1);
#pragma omp parallel private(pid)
        {
            pid = omp_get_thread_num();
            if (!(pid % step))
            {
                num[pid+step/2] = num[pid];
            }
#pragma omp barrier
        }
    }
    printf("after sum:");
    for (int i = 0; i < N; i++)
    {
        printf("%d", num[i]);
        if(i == N-1)
            printf("\n");
    }
    return 0;
}
