#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void loop1(){
    int A[101], B[101], C[101], D[101];
    for (int i = 0; i < 101; i++)
    {
        A[i] = i;
        B[i] = i+1;
        C[i] = i+2;
        D[i] = i+3;
    }
    for (int i = 1; i <= 100; i++)
    {
        B[i] = C[i-1]*2;
        C[i] = 1.0/B[i];
    }
#pragma omp parallel for
    for (int i = 1; i <= 100; i++)
    {
        /* code */
        A[i] = A[i] + B[i-1];
        D[i] = C[i] * C[i];
    }
}

void loop2(){
    int count = 1000;
    int A[1000], B[1000], C[1000], D[1000];
    for (int i = 0; i < count; i++)
    {
        A[i] = i+1;
        B[i] = i+2;
        C[i] = i+3;
        D[i] = i+4;
    }
#pragma omp parallel for
    for (int i = 1; i <= 500; i++)
    {
        A[i] = B[i] + C[i];
        D[i] = (A[i] + A[1000-i])/2.0;
    }
#pragma omp parallel for
    for (int i = 501; i < count; i++)
    {
        /* code */
        A[i] = B[i] + C[i];
        D[i] = (A[i]+A[1000-i])/2.0;
    }
}

void loop3(){
    int count = 100;
    int A[500][200];
    int C[100][100];
    int D[100][100];
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count; j++)
        {
            C[i][j] = i+j+1;
            D[i][j] = i+j+3;
        }
    }
    for (int i = 0; i < 500; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            A[i][j] = i+j+2;
        }
    }
    
#pragma omp parallel for
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; i < count; j++)
        {
            A[3*i+2*j][2*j] = C[i][j]*2;
            D[i][j] = A[(i-j+6)>0 ? (i-j+6):-(i-j+6)][i+j];
        }   
    }
}