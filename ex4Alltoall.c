#include<stdio.h>
#include<stdlib.h>
#include"mpi.h"
#include<memory.h>

//采用MPI_Send和MPI_Recv编写代码实现的MPI_Allgather
void MPI_Alltoall_my(int* senddata, int sendcount, MPI_Datatype senddatatype, int* recvdata, int recvcount,
        MPI_Datatype recvdatatype, MPI_Comm comm)
{
    int rank, size;
    MPI_Status status;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    for (int i = 0; i < size; i++)
    {
        if (i != rank)
        {
            MPI_Send(senddata + i * sendcount, sendcount, senddatatype, i, rank , comm);
            MPI_Recv(recvdata + i * recvcount, recvcount, recvdatatype, i, i, comm, &status);
        }
        else
        {
        	//memcpy(recvdata + i * recvcount, senddata, sizeof(senddatatype)*sendcount);
        	recvdata[i] = senddata[i];
        }
    }
}
int main(int argc, char* argv[])
{
    int i, rank, size, tag = 1;
    int senddata[8], recvdata[8];

    
    double start_time, end_time, s_t, e_t;
    int count = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int j = 0; j < size; j++)
    {
        senddata[j] = j+1;
    }
    start_time = MPI_Wtime();
    //自己编写的MPI_Allgather的性能测试 
    MPI_Alltoall_my(senddata, count, MPI_INT, recvdata, count, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    MPI_Reduce(&start_time, &s_t, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&end_time, &e_t, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	for (i = 0; i < size; i++)
		printf("My rank =  %d  After myalltoall recv = %d\n", rank, recvdata[i]);

    if (rank == 0)
    {
        printf("myalltoall : count = %d total time = %f\n", count, e_t - s_t);
    }
    MPI_Barrier(MPI_COMM_WORLD);
	//MPI原有的MPI_Allgather的功能测试
    start_time = MPI_Wtime();
    MPI_Alltoall(senddata, count, MPI_INT, recvdata, count, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    MPI_Reduce(&start_time, &s_t, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&end_time, &e_t, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	for (i = 0; i < size; i++)
		printf("My rank =  %d  recv = %d\n", rank, recvdata[i]);

    if (rank == 0)
    {
        printf("alltoall : count = %d total time = %f\n", count, e_t - s_t);
    }
    MPI_Finalize();
    return 0;
}