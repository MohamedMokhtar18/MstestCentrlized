#include <stdio.h>
#include <mpi.h>
#define to_right 201

int main(int argc, char *argv[])
{
    int size_world;
    int rank_world;
    int my_rank_sm, size_sm;
    int rcv_buf, sum, i;
    int *rcv_buf_ptr;
    int snd_buf, provided;

    MPI_Comm comm_sm;
    MPI_Win win;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &size_world);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_world);
    // MPI_Init(&argc, &argv);
    // splitting memory into islands
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &comm_sm);
    MPI_Comm_rank(comm_sm, &my_rank_sm);
    MPI_Comm_size(comm_sm, &size_sm);
    if (my_rank_sm == 0)
    {
        if (size_sm == size_world)
        {
            printf("MPI_COMM_WORLD consists of only one shared memory region\n");
            printf("MPI_COMM_WORLD is split into %d \n", size_sm);
        }
        else
        {
            printf("MPI_COMM_WORLD is split into 2 or more shared memory islands\n");
            printf("MPI_COMM_WORLD is split into %d \n", size_sm);
        }
    }
    MPI_Win_allocate_shared(sizeof(int), sizeof(int), MPI_INFO_NULL, comm_sm, &rcv_buf_ptr, &win);
    double start = MPI_Wtime();

    sum = 100000000;
    snd_buf = sum;
    MPI_Win_lock_all(0, win);

    for (i = 0; i < size_world; i++)
    {
        if (i == my_rank_sm)
        {
            *(rcv_buf_ptr) = snd_buf; // to store into right neighbor's rcv_buf

            snd_buf = *rcv_buf_ptr;

            sum = *rcv_buf_ptr;
        }
        else if (my_rank_sm >= 4)
        {

            if (i == my_rank_sm)
            {
                // MPI_Rget(rcv_buf_ptr, 1, MPI_INT, (1 - 4), (MPI_Aint)0, 1, MPI_INT, win, &req);
                // MPI_Wait(&req, &status);
                MPI_Get(rcv_buf_ptr, 1, MPI_INT, (i - 4), (MPI_Aint)0, 1, MPI_INT, win);
                // MPI_Put(&snd_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, win);

                sum = *rcv_buf_ptr;
            }
        }
    }
    MPI_Win_unlock_all(win);

    MPI_Win_sync(win);
    printf("PE%i:\t Data = %i\n", my_rank_sm, sum);
    double end = MPI_Wtime();
    printf("The process took %fseconds to run\n", end - start);
    MPI_Finalize();
}