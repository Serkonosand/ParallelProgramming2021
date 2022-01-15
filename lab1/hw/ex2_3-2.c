#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int N = 50;
int main(){
    int count = 100;
    int x[100], y[200], b[100];
    int a[100][100], c[100][100];
    for (int i = 0; i < 100; i++)
    {
        x[i] = i+1;
        b[i] = i+2;
    }
    for (int i = 0; i < 200; i++)
    {
        y[i] = i+1;
    }
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            a[i][j] = i+j+2;
            c[i][j] = i+j+3;
        }
    }
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count-1; j++)
        {
            b[j] = a[j][N];
            #pragma omp parallel for
            for (int k = 0; k < count; i++)
            {
                a[j+1][k] = b[j] + c[j][k];
            }
        }
    }
    for (int i = 0; i < count; i++)
    {
        /* code */
        #pragma omp parallel for
        for (int j = 0; j < count-1; i++)
        {
            y[i+j] = a[j+1][N];
        }
    }
    #pragma omp parallel for
    for (int i = 0; i < count; i++)
    {
        /* code */
        x[i] = y[i] + 10;
    }
    return 0;
}