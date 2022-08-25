#include <stdio.h>
#include <mpi.h>
#define to_right 201
void my_bcast(void *data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator)
{
    int world_rank;
    MPI_Comm_rank(communicator, &world_rank);
    int world_size;
    MPI_Comm_size(communicator, &world_size);

    if (world_rank == root)
    {
        // If we are the root process, send our data to everyone
        int i;
        for (i = 0; i < world_size; i++)
        {
            if (i != world_rank)
            {
                //       MPI_Win_fence(0, win);
                // MPI_Put(&data, count, datatype, i, (MPI_Aint)0, 1, datatype, win);
                // MPI_Win_fence(0, win);
                MPI_Send(data, count, datatype, i, 0, communicator);
            }
        }
    }
    else
    {
        // If we are a receiver process, receive the data from the root
        MPI_Recv(data, count, datatype, root, 0, communicator,
                 MPI_STATUS_IGNORE);
    }
}
int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int data;
    if (world_rank == 0)
    {
        data = 100;
        printf("Process 0 broadcasting data %d\n", data);
        my_bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        my_bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Process %d received data %d from root process\n", world_rank, data);
    }

    MPI_Finalize();
}
// int main(int argc, char *argv[])
// {
//     MPI_Win win;
//     int snd_buf, rcv_buf;
//     int comp_data = 0;

//     int my_rank, size;
//     int right, left;
//     int i = 0;
//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     MPI_Win_create(&snd_buf, (MPI_Aint)sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
//     if (my_rank == 0)
//     {
//         MPI_Win_fence(0, win);
//         MPI_Put(&snd_buf, 1, MPI_INT, 1, (MPI_Aint)0, 1, MPI_INT, win);
//         MPI_Win_fence(0, win);
//         MPI_Win_fence(1, win);
//         MPI_Put(&snd_buf, 1, MPI_INT, 2, (MPI_Aint)0, 1, MPI_INT, win);
//         MPI_Win_fence(1, win);
//         MPI_Win_fence(2, win);
//         MPI_Put(&snd_buf, 1, MPI_INT, 3, (MPI_Aint)0, 1, MPI_INT, win);
//         MPI_Win_fence(2, win);
//         MPI_Win_fence(3, win);
//         MPI_Put(&snd_buf, 1, MPI_INT, 4, (MPI_Aint)0, 1, MPI_INT, win);
//         MPI_Win_fence(3, win);
//     }
//     else
//     {

//         for (i = 1; i < size; i++)
//         {
//             if (my_rank == i)
//             {
//                 if (i + 4 > size)
//                 {
//                     break;
//                 }

//                 MPI_Win_fence(0, win);
//                 MPI_Put(&snd_buf, 1, MPI_INT, i + 4, (MPI_Aint)0, 1, MPI_INT, win);
//                 MPI_Win_fence(0, win);
//                 break;
//             }
//         }
//     }
//     for (int j = 1; j < size; i++)
//     {
//         if (my_rank == i)
//         {
//             if (i + 4 > size)
//             {
//                 break;
//             }

//             MPI_Win_fence(0, win);
//             MPI_Get(&rcv_buf, 1, MPI_INT, 0, (MPI_Aint)0, 1, MPI_INT, win);
//             MPI_Win_fence(0, win);
//             printf("rcv from rank 0 to rank %d value%d\n", i, rcv_buf);
//         }
//     }
//     MPI_Win_free(&win);
//     MPI_Finalize();
//     // MPI_Init(&argc, &argv);
//     //  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank_world);
//     //  MPI_Comm_size(MPI_COMM_WORLD, &size_world);
//     //  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &comm_sm);
//     //  MPI_Comm_rank(comm_sm, &my_rank_sm);
//     //  MPI_Comm_size(comm_sm, &size_sm);

//     // MPI_Win_free(&win);
//     // MPI_Finalize();
// }