#include "mpi.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int id_procs, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_procs);
    int data = id_procs+1;
    int recvdata;
    MPI_Status status;
    printf("process id %d data = %d\n",id_procs, data);
    int logN = (int)log2(num_procs);
    for(int i = 0; i < logN; i++) {
        int tag = i+1;
        int step = (int)pow(2,i);
        int dest = id_procs ^ step;
        MPI_Send(&data, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        MPI_Recv(&recvdata, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
        data += recvdata;
    }


    printf("process id %d sum is = %d\n",id_procs, data);

    MPI_Finalize();
    return 0;
}