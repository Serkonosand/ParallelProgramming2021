#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
int MyBcastMPI(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
    return 0;
}
*/
int main(int argc, char *argv[])
{
    int id_procs, num_procs;
    char seq[16];
    int root = 3;
    MPI_Group world_group, new_group;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_procs);
    if (id_procs == root)
    {
        strcpy(seq, "hello,MPI!");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm split_comm_world;
    MPI_Status status;

    int rank;
    int size;

    // MPI split COMM_WORLD into 4 groups 
    MPI_Comm_split(MPI_COMM_WORLD, id_procs % 4, id_procs, &split_comm_world);
    MPI_Comm_rank(split_comm_world, &rank);
    MPI_Comm_size(split_comm_world, &size);
    //create new group H
    MPI_Comm h_comm_world;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    int grpsize = num_procs / 2;
    int zerolist[] = {0, 1, 2, 3};
    int zerocnt = 0;

    MPI_Group_incl(world_group, grpsize, zerolist, &new_group);
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &h_comm_world);
    // message from root to 0 proc of MPI_COMM_WORLD
    if (id_procs == root)
    {
        MPI_Send(&seq, 16, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }
    else if (id_procs == 0)
    {
        MPI_Recv(&seq, 16, MPI_CHAR, root, 1, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Broadcast within the group H
    if(h_comm_world != MPI_COMM_NULL)
        MPI_Bcast(&seq, 16, MPI_CHAR, 0, h_comm_world);
    MPI_Barrier(MPI_COMM_WORLD);
    //Broadcasr within the group N

    MPI_Bcast(&seq, 16, MPI_CHAR, 0, split_comm_world);
    MPI_Barrier(MPI_COMM_WORLD);

    printf("MPI Comm rank %d, original id %d, size %d. the new msg is %s\n", rank, id_procs, size, seq);
    MPI_Finalize();
    return 0;
}