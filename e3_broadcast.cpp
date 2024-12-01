#include <mpi.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
     int data;
    if (rank == 0){

     data = 100;
    }

    MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << "Processor " << rank << " received data: " << data << std::endl;
    
    MPI_Finalize();
    return 0;

}