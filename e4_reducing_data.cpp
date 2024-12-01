#include <mpi.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int local_data=rank+1;

    int global_sum;


    MPI_Reduce(&local_data, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank==0){
         std::cout << "The sum of all ranks is: " << global_sum << std::endl;
    }
    MPI_Finalize();
return 0;
}