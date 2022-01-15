#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int n = 50;
int main(){
    int a[n], b[n], c[n];
    for (int i = 0; i < n; i++)
    {
        a[i] = (int)(rand()%n)+1;
        b[i] = (int)(rand()%n)+1;
        c[i] = (int)(rand()%n)+1;
    }
    
#pragma omp simd
    for (int i = 0; i < n; i++)
    {
        a[i] = b[i] + c[i+1];
        c[i] = a[i] + b[i];
    }
    printf("a:\n");    
    for (int i = 0; i < n; i++)
    {
        printf("%d\t", a[i]);
    }
    printf("\n");
    printf("b:\n");    
    for (int i = 0; i < n; i++)
    {
        printf("%d\t", b[i]);
    }
    printf("\n");
    printf("c:\n");    
    for (int i = 0; i < n; i++)
    {
        printf("%d\t", c[i]);
    }
    printf("\n");
    return 0;
}