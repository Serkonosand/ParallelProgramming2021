#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "omp.h"
#define a(x,y) a[x*M +y]
#define A(x,y) A[x*M+y]
#define l(x,y) l[x*M+y]
#define u(x,y) u[x*M+y]
#define floatsize sizeof(float)
#define intsize sizeof(int)

int M;
int m;
float * A;
int my_rank;
int p;
MPI_Status status;

void fatal(char * message)
{
    printf("%s\n", message);
    exit(1);
}

void Env_Fin(float *a, float *f)
{
    free(a);
    free(f);
}

int main(int argc, char * argv[])
{
    int i,j,k, my_rank, group_size;
    int i1, i2;
    int v,w;
    float *a, *f, *l, *u;
    printf("Input matrix row length:\n");
    scanf("%d", &M);
    doubel time;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &group_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    p = group_size;
    if (my_rank == 0)
    {
        A = (float *)malloc(floatsize * M * M);
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < M; j++)
            {
                A(i,j) = (float)(rand() % 25 + 1);
            }
        }
        
    }
    MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);
    m=M/p;
    if (M%p!=0) m++;
    a=(float*)malloc(floatsize*m*M);

    f=(float*)malloc(floatsize*M);

    if (my_rank==0)
    {
        l=(float*)malloc(floatsize*M*M);
        u=(float*)malloc(floatsize*M*M);
    }
 
    if (a==NULL) fatal("allocate error\n");
    time = MPI_Wtime();
    if (my_rank==0)
    {
        for(i=0;i<m;i++)
            for(j=0;j<M;j++)
                a(i,j)=A((i*p),j);
        for(i=0;i<M;i++)
            if ((i%p)!=0)
        {
            i1=i%p;
            i2=i/p+1;
            MPI_Send(&A(i,0),M,MPI_FLOAT,i1,i2,MPI_COMM_WORLD);
        }
    }
    else
    {
        for ( i = 0; i < m; i++)
        {
            MPI_Recv(&a(i,0),M,MPI_FLOAT, 0,i+1, MPI_COMM_WORLD, &status);
        }
        
    }
    for(i=0;i<m;i++)
        for(j=0;j<p;j++)
    {
        /*j号进程负责广播主行元素*/
        if (my_rank==j)
        {
            v=i*p+j;
            for (k=v;k<M;k++)
                f[k]=a(i,k);
 
            MPI_Bcast(f,M,MPI_FLOAT,my_rank,MPI_COMM_WORLD);
        }
        else
        {
            v=i*p+j;
            MPI_Bcast(f,M,MPI_FLOAT,j,MPI_COMM_WORLD);
        }
 
        /*编号小于my_rank的进程（包括my_rank本身）利用主行对其第i+1,…,m-1行数据做行变换*/
        /*********MPI 并行优化 ********/
        if (my_rank<=j)
        {
           #pragma  omp parallel shared(a,f,v,m) private(k,w)
           {
                #pragma omp for
 
                for(k=i+1;k<m;k++)
                {
 
                    a(k,v)=a(k,v)/f[v];
 
                }
 
 
                for(k=i+1;k<m;k++)
                {
                    #pragma omp for
                    for(w=v+1;w<M;w++)
                        a(k,w)=a(k,w)-f[w]*a(k,v);
                }
            }
        }
 
        /*编号大于my_rank的进程利用主行对其第i,…,m-1行数据做行变换*/
        if (my_rank>j)
        {
 
            #pragma  omp parallel shared(a,f,v,m) private(k,w)
            {
                #pragma omp for
                for(k=i;k<m;k++)
                {
                    a(k,v)=a(k,v)/f[v];
 
                }
 
                for(k=i;k<m;k++)
                {
                    #pragma omp for
                    for(w=v+1;w<M;w++)
                        a(k,w)=a(k,w)-f[w]*a(k,v);
                }
            }
        }
    }
 
    /*0号进程从其余各进程中接收子矩阵a，得到经过变换的矩阵A*/
    if (my_rank==0)
    {
        for(i=0;i<m;i++)
            for(j=0;j<M;j++)
                A(i*p,j)=a(i,j);
    }
    if (my_rank!=0)
    {
        for(i=0;i<m;i++)
            MPI_Send(&a(i,0),M,MPI_FLOAT,0,i,MPI_COMM_WORLD);
    }
    else
    {
        for(i=1;i<p;i++)
            for(j=0;j<m;j++)
        {
            MPI_Recv(&a(j,0),M,MPI_FLOAT,i,j,MPI_COMM_WORLD,&status);
            for(k=0;k<M;k++)
                A((j*p+i),k)=a(j,k);
        }
    }
    time = MPI_Wtime() - time;
    if (my_rank==0)
    {
            for(i=0;i<M;i++)
            {
                for(j=0;j<M;j++)
                {
                    if(i>j)
                    {
                        l(i,j)=A(i,j);
                        u(i,j)=0.0;
                    }
                    else if(i<j)
                    {
                        u(i,j)=A(i,j);
                        l(i,j)=0.0;
                    }
                    else
                    {
                        u(i,j)=0.0;
                        l(i,j)=1.0;
                    }
                }
            }
        for(i=0;i<M;i++)
        {
            for(j=0;j<M;j++)
            {
                if (i>j)
                    l(i,j)=A(i,j);
                else
                   u(i,j)=A(i,j);
            }
        }
        printf("Input matrix:\n");
        printf("%d\t %d\n",M, N);
        for(i=0;i<M;i++)
        {
            for(j=0;j<N;j++)
                printf("%f\t",A(i,j));
            printf("\n");
        }
        printf("\nOutput of LU operation\n");
        printf("Matrix L:\n");
        for(i=0;i<M;i++)
        {
            for(j=0;j<M;j++)
                printf("%f\t",l(i,j));
            printf("\n");
        }
        printf("Matrix U:\n");
        for(i=0;i<M;i++)
        {
            for(j=0;j<M;j++)
                printf("%f\t",u(i,j));
            printf("\n");
        }
        printf("total time: %f\n", time);
    }
    MPI_Finalize();
    Env_Fin(a,f);
    return(0);
}