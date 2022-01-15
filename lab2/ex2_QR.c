#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int N;
#define A(x, y) A[x * N + y]
#define Q(x, y) Q[x * N + y]
#define R(x, y) R[x * N + y]
#define tmp(x, y) tmp[x * N + y]

void Env_Fin(float *a, float *q, float *r, float *tmp, \
                float *aj, float *ai, float *qi, float *qj)
{
    free(a);
    free(q);
    free(r);
    free(tmp);
    free(qi);
    free(qj);
    free(ai);
    free(aj);
}

int main(){
    printf("input matrix rows :");
    scanf("%d", &N);
    float *A, *Q, *R, *tmp;
    A =(float *)malloc(N * N * sizeof(float));
    Q =(float *)malloc(N * N * sizeof(float));
    R =(float *)malloc(N * N * sizeof(float));
    tmp = (float *)malloc(N * N * sizeof(float));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A(i,j) = (float)(rand() % 10) + 1;
            if(i == j)
                Q(i,j) = 1;
            R(i,j) = 0;
        }
    }
    printf("matrix A:\n");
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            printf("%f\t",A(i,j));
        printf("\n");
    }
    float *Ai, *Aj, *Qi, *Qj;
    Aj =(float *)malloc(N * sizeof(float));
    Ai =(float *)malloc(N * sizeof(float));
    Qi =(float *)malloc(N * sizeof(float));
    Qj =(float *)malloc(N * sizeof(float));
    for (int i = 0; i < N; i++)
    {
        Ai[i] = 0;
        Aj[i] = 0;
        Qi[i] = 0;
        Qj[i] = 0;
    }
    float c, s, sq;
    double start = omp_get_wtime();
    for(int j = 0; j < N; j++){
        for (int i = j+1; i < N; i++)
        {
            sq = sqrt(A(j, j) * A(j, j) + A(i, j) * A(i, j));
            c = A(j, j)/sq;
            s = A(i, j)/sq;
            int k ;
#pragma omp parallel for private(k)
            for (k = 0; k < N; k++)
            {
                Aj[k] = c * A(j, k) + s * A(i, k);
                Qj[k] = c * Q(j, k) + s * Q(i, k); 
                Ai[k] = - s * A(j, k) + c * A(i, k);
                Qi[k] = - s * Q(j, k) + c * Q(i, k);
            }
#pragma omp barrier
#pragma omp parallel for private(k)
            for (k = 0; k < N; k++)
            {
                A(j, k) = Aj[k];
                A(i, k) = Ai[k];
                Q(i, k) = Qi[k];
                Q(j, k) = Qj[k];
            }
#pragma omp barrier            
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            R(i, j) = A(i, j);
            tmp(j, i) = Q(i, j);
        }
    }
    double end = omp_get_wtime();
    double time = end - start;
    printf("matrix Q:\n");
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            printf("%f\t",tmp(i,j));
        printf("\n");
    }
    printf("matrix R:\n");
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            printf("%f\t",R(i,j));
        printf("\n");
    }   
    printf("total time: %f\n", time);
    Env_Fin(A, Q, R, tmp, Aj, Ai, Qi, Qj);     
    return 0;
}
