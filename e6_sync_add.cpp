#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
int main(int argc, char** argv) {
MPI_Init(&argc, &argv);

int rank, size;

MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

std::vector<int> numbers(size);
int local_sum = 0;
// Only the master process initializes the data

if (rank == 0) {
   
     printf("Process %d: Initializing data...\n", rank);
    for (int i = 0; i < size; ++i) {
        numbers[i] =  i + 1;  // Fill the vector with numbers 1, 2, ..., size
         printf("Process %d: numbers[%d] = %d\n", rank, i, numbers[i]);
    }
}

// Distribute the data among processes

int local_number =0;
printf("Process %d:  Before MPI_Scatter data...\n", rank);
MPI_Scatter(numbers.data(), 1, MPI_INT, &local_number, 1, MPI_INT, 0, MPI_COMM_WORLD);

// Each process calculates the local sum
 printf("Process %d: Received local number %d from process 0\n", rank, local_number);
local_sum = local_number;

// Reduce the local sums to the global sum in the master process
int global_sum = 0;
 printf("Process %d: Before MPI_Reduce, local_sum = %d\n", rank, local_sum);
MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// Print the global sum in the master process
if(rank == 0) {
      printf("Process %d: The global sum is %d\n", rank, global_sum);
}
MPI_Finalize();

return 0;
}