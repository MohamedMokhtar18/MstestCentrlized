#include <stdio.h>
#include <mpi.h>
#define to_right 201

int main(int argc, char *argv[])
{
    int right, left;
    int my_rank_world, size_world;
    int my_rank_sm, size_sm;
    int rcv_buf, sum, i;
    int *rcv_buf_ptr;
    int snd_buf;
    MPI_Comm comm_sm;

    MPI_Win win;

    MPI_Init(&argc, &argv);
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &comm_sm);
    MPI_Comm_rank(comm_sm, &my_rank_sm);
    MPI_Comm_size(comm_sm, &size_sm);
    if (my_rank_sm == 0)
    {
        if (size_sm == size_world)
        {
            printf("MPI_COMM_WORLD consists of only one shared memory region\n");
        }
        else
        {
            printf("MPI_COMM_WORLD is split into 2 or more shared memory islands\n");
            printf("MPI_COMM_WORLD is split into %d \n", size_sm);
        }
    }
    right = (my_rank_sm + 1) % size_sm;
    left = (my_rank_sm - 1 + size_sm) % size_sm;

    MPI_Win_allocate_shared(sizeof(int), sizeof(int), MPI_INFO_NULL, comm_sm, &rcv_buf_ptr, &win);

    sum = 100;
    snd_buf = sum;

    for (i = 0; i < size_sm; i++)
    {

        /* *** The following 3 lines shall be substituted by
         *** 1-sided communication and synchronization    */
        MPI_Win_fence(0, win);
        // MPI_Put(&snd_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, win);
        *(rcv_buf_ptr) = snd_buf; // to store into right neighbor's rcv_buf
        MPI_Win_fence(0, win);

        snd_buf = *rcv_buf_ptr;

        sum = *rcv_buf_ptr;
    }
    if (my_rank_sm > 4)
    {
        for (i = 5; i > size_sm && size_sm < size_world; i++)
        {
            MPI_Win_fence(0, win);
            MPI_Get(&rcv_buf, 1, MPI_INT, (i - 4), (MPI_Aint)0, 1, MPI_INT, win);
            // MPI_Put(&snd_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, win);
            MPI_Win_fence(0, win);
            sum = *rcv_buf_ptr;
        }
    }

    printf("PE%i:\tSum = %i\n", my_rank_sm, sum);
    MPI_Win_free(&win);
    MPI_Finalize();
}