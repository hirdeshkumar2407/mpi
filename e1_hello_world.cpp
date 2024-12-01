#include <mpi.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char** argv) {
using namespace std;
    MPI_Init(NULL, NULL);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    cout<<"Hello world from processor "<<rank<<" out of "<<size<<endl;
  
    MPI_Finalize();
    return 0;
}