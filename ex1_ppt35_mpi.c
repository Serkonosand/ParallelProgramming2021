#include "mpi.h"
#include <stdio.h>
#include <math.h>


int main(int argc, char *argv[])
{
    int pid, pnums;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &pnums);
    int data, recvdata, logN;
    MPI_Status status;
    data = pid+1;
    recvdata = 0;
    logN = (int)log2(pnums);
    printf("process id: %d,data:%d\n", pid,data);
    //sum
    for (int i = 1; i <= logN; i++)
    {
        int tag = i;
        int step = (int)pow(2, i);
        if (pid % step == 0)
        {
            MPI_Recv(&recvdata, 1, MPI_INT, pid+step/2, tag, MPI_COMM_WORLD, &status);
            data += recvdata;
        }
        else
         if(pid % step == step/2)
        {
            MPI_Send(&data, 1, MPI_INT, pid-step/2, tag, MPI_COMM_WORLD);
        }
    }
    //spread
    for (int i = logN; i >0; i--)
    {
        int tag = i;
        int step = (int)pow(2, i);
        if (pid % step == 0)
        {
            MPI_Send(&data, 1, MPI_INT, pid+step/2, tag, MPI_COMM_WORLD);
        }
        else
        if(pid % step == step/2)
        {
            MPI_Recv(&recvdata, 1, MPI_INT, pid-step/2, tag, MPI_COMM_WORLD, &status);
            data = recvdata;
        }
        
    }
    printf("%d sum is %d\n", pid, data);
    MPI_Finalize();
    return 0;
}