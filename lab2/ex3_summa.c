#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
 
void PrintMatrixForVector(int * matrix,int high,int len)
{
    int i;
    for(i=0;i<high*len;i++)
    {
        printf("%6d  ",matrix[i]);	
        if(i%len==len-1&&i!=0)
			printf("\n");
    }
}

void MatrixMultiply(int * A,int *B,int *C,unsigned m,unsigned n,unsigned p)
{   int i,j,k;
    for(i=0;i<m;i++)
       for(j=0;j<p;j++)
           
	   {
		   int result=0; 
		   for(k=0;k<n;k++)
            {
               result=A[i*n+k]*B[k*p+j]+result;
            }
           C[i*p+j]=result;
	   }
}
 

void MatrixAdd(int * A,int *B,unsigned m,unsigned n) //the result remain in A
{  int i,j;
   for(i=0;i<m;i++)
      for(j=0;j<n;j++)
        {
            A[i*n+j]=A[i*n+j]+B[i*n+j];
        }
}
 
 
void PrintMatrix(int ** matrix,int high,int len)
{
    int i,j;
    for(i=0;i<high;i++)
    {
        for(j=0;j<len;j++)
        {
            printf("%6d  ",matrix[i][j]);
        }
        printf("\n");
    }
}
 
 
 
/****随机数据以待计算****/
void RandomMatrix(int *matrix,int len)
{
   struct timeval tpstart;  //使数据更均匀
   gettimeofday(&tpstart,NULL);
   srand(tpstart.tv_usec);
   int i=0;
   for(i=0;i<len;i++)
   matrix[i]=rand()%8;
}
int main(int argc,char **argv)
{
	int rank;
    MPI_Status status;
    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
	int nodeNum;  //节点数，必须为可开平方
	
	int matrixHighA;// 矩阵A行数
	int matrixLenA;//矩阵A列数
	
	
	int matrixHighB;
    int matrixLenB;
	/****   参数检查  参数错误用默认参数*****/
	if(argc!=5&&rank==0)
	{
 
		 printf("The para is wrong!using default para\n");
		 nodeNum=4;
		 
		 matrixHighA=6;
		 matrixLenA=8;
		 
		 
		 matrixHighB=8;
		 matrixLenB=10;
	}
	else
	{
 
		nodeNum=atoi(argv[1]);
		
		matrixHighA=atoi(argv[2]);
		matrixLenA=atoi(argv[3]);
		
		
		matrixHighB=atoi(argv[3]);
        matrixLenB=atoi(argv[4]);
 
	}
    int p=sqrt(nodeNum);
 
/*************计算local A   B行列数*************************/
    int localHighA=matrixHighA/p;
    int localLenA=matrixLenA/p;
    
 
    int localHighB=matrixHighB/p;
    int localLenB=matrixLenB/p;
    
    
    int i;
    int j;
    int k;
    int l;
 
 
    int * A=(int *)malloc(localLenA*localHighA*sizeof(int));
    RandomMatrix(A,localHighA*localLenA);
 
 
 
    int * B=(int *)malloc(localLenB*localHighB*sizeof(int));
    RandomMatrix(B,localHighB*localLenB);
 
    int * C=(int *)malloc(localHighA*localLenB*sizeof(int));
 
    for(i=0;i<localHighA*localLenB;i++)C[i]=0;
	int myRow=rank/p;//计算当前节点在mesh中的行列值
	int myCol=rank%p;
 
 
    //将数据发送到0号进程收集显示
    MPI_Send(A,localHighA*localLenA,MPI_INT,0,rank+100,MPI_COMM_WORLD);
    MPI_Send(B,localHighB*localLenB,MPI_INT,0,rank+200,MPI_COMM_WORLD);
 
 
	if(rank==0)
	{
		int **matrixA=(int **)malloc(matrixHighA*sizeof(int *));
		for (i=0;i<matrixHighA;i++)
			matrixA[i]=(int *)malloc(matrixLenA*sizeof(int));
 
		int **matrixB=(int **)malloc(matrixHighB*sizeof(int *));
		for (i=0;i<matrixHighB;i++)
			matrixB[i]=(int *)malloc(matrixLenB*sizeof(int));
        for(i=0;i<nodeNum;i++)
        {
            int *receiveATemp=(int *)malloc(localLenA*localHighA*sizeof(int));
            int *receiveBTemp=(int *)malloc(localLenB*localHighB*sizeof(int));
 
            MPI_Recv(receiveATemp,localHighA*localLenA,MPI_INT,i,i+100,MPI_COMM_WORLD,&status);
            MPI_Recv(receiveBTemp,localHighB*localLenB,MPI_INT,i,i+200,MPI_COMM_WORLD,&status);
            l=0;
            for(j=0;j<localHighA;j++)
                for(k=0;k<localLenA;k++)
                {
                    matrixA[j+(int)(i/p)*localHighA][k+(int)(i%p)*localLenA]=receiveATemp[l++];
                }
            l=0;
            for(j=0;j<localHighB;j++)
                for(k=0;k<localLenB;k++)
                {
                    matrixB[j+(int)(i/p)*localHighB][k+(int)(i%p)*localLenB]=receiveBTemp[l++];
                }
            free(receiveATemp);
            free(receiveBTemp);  
        }  
        printf("A:\n");
        PrintMatrix(matrixA,matrixHighA,matrixLenA);
        printf("B:\n");
        PrintMatrix(matrixB,matrixHighB,matrixLenB);
        for (i=0;i<matrixHighA;i++)
            free(matrixA[i]);
        for (i=0;i<matrixHighB;i++)
            free(matrixB[i]);
        free(matrixA);
        free(matrixB);
	}
 
 
    for(i=0;i<p;i++)//每个节点向同行同列发送局部数据 A B
	{
	//	if(myCol!=i)
		{
			MPI_Send(A,localHighA*localLenA,MPI_INT,myRow*p+i,1,MPI_COMM_WORLD);
			MPI_Send(B,localHighB*localLenB,MPI_INT,myRow*p+i,2,MPI_COMM_WORLD);
		}
	//	if(myRow!=i)
		{
		   MPI_Send(A,localHighA*localLenA,MPI_INT,i*p+myCol,1,MPI_COMM_WORLD);
		   MPI_Send(B,localHighB*localLenB,MPI_INT,i*p+myCol,2,MPI_COMM_WORLD);		
		}	
	}
 
 
 
	int *receiveA=(int *)malloc(localLenA*localHighA*sizeof(int));
	int *receiveB=(int *)malloc(localLenB*localHighB*sizeof(int));
    int *resultC= (int *)malloc(localHighA*localLenB*sizeof(int));
	for(i=0;i<localHighA*localLenB;i++)resultC[i]=0;
 
/*********************计算矩阵的值*****************************/
	for(i=0;i<p;i++)
	{
		MPI_Recv(receiveA,localHighA*localLenA,MPI_INT,myRow*p+i,1,MPI_COMM_WORLD,&status);
		MPI_Recv(receiveB,localHighB*localLenB,MPI_INT,i*p+myCol,2,MPI_COMM_WORLD,&status);
 
	
        MatrixMultiply(receiveA,receiveB,resultC,localHighA,localLenA,localLenB);
		MatrixAdd(C,resultC,localHighA,localLenB);	
	}
 
    MPI_Send(C,localHighA*localLenB,MPI_INT,0,rank+400,MPI_COMM_WORLD);//将局部结果C发送至0号收集
    if(rank==0)//收集数据并且在后面显示
    { 
        int **matrixC=(int **)malloc(matrixHighA*sizeof(int *));
        for (i=0;i<matrixHighA;i++)
            matrixC[i]=(int *)malloc(matrixLenB*sizeof(int));

        for(i=0;i<nodeNum;i++)
        {
           int *receiveCTemp=(int *)malloc(localLenB*localHighA*sizeof(int));
           MPI_Recv(receiveCTemp,localHighA*localLenB,MPI_INT,i,i+400,MPI_COMM_WORLD,&status);
           l=0;
           for(j=0;j<localHighA;j++)
               for(k=0;k<localLenB;k++)
               {
                   matrixC[j+(int)(i/p)*localHighA][k+(int)(i%p)*localLenB]=receiveCTemp[l++];
               }
           free(receiveCTemp);
        }
        printf("C:\n");
        PrintMatrix(matrixC,matrixHighA,matrixLenB); 
    } 
    MPI_Finalize();
    return 0;
}
 