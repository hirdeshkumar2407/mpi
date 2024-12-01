#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Simple ping pong program to exemplify MPI_Send and MPI_Recs
// Assume only two processes
int main(int argc, char** argv) {
  const int tot_msgs = 100;
  
  MPI_Init(NULL, NULL);

  int my_rank,comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   
    int next_rank = (my_rank + 1) % comm_size; // Next process
    int prev_rank = (my_rank - 1 + comm_size) % comm_size; // Previous process

  int num_msgs = 0;
  while (num_msgs < tot_msgs) {
    if (num_msgs % comm_size == my_rank) {
      num_msgs++;
       MPI_Send(&num_msgs, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
      printf("Process %d sent message %d to process %d\n", my_rank, num_msgs, next_rank);
    } else {
            MPI_Recv(&num_msgs, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d received message %d from process %d\n", my_rank, num_msgs, prev_rank);
    }
  }

  MPI_Finalize();
}
