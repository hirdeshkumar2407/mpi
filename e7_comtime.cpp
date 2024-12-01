#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 1000000; // Number of elements to send
    std::vector<int> data(N, rank); // Data to send (all elements initialized to rank)

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize processes before starting the timer

    // Timing synchronous communication
    double sync_start = MPI_Wtime();
    if (rank == 0) {
        // Process 0 sends data to other processes
        for (int i = 1; i < size; ++i) {
            MPI_Send(data.data(), N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Other processes receive data
        MPI_Recv(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    double sync_end = MPI_Wtime();

    // Timing asynchronous communication
    MPI_Barrier(MPI_COMM_WORLD); // Synchronize processes before starting async timing
    double async_start = MPI_Wtime();
    MPI_Request request;
    if (rank == 0) {
        // Process 0 sends data asynchronously to other processes
        for (int i = 1; i < size; ++i) {
            MPI_Isend(data.data(), N, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE); // Wait for async send to complete
        }
    } else {
        // Other processes receive data
        MPI_Request recv_request;
        MPI_Irecv(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE); // Wait for async receive to complete
    }
    double async_end = MPI_Wtime();

    // Print the results
    if (rank == 0) {
        std::cout << "Sync communication time: " << sync_end - sync_start << " seconds." << std::endl;
        std::cout << "Async communication time: " << async_end - async_start << " seconds." << std::endl;
    }

    MPI_Finalize();
    return 0;
}
