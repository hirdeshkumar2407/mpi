#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

int main(int argc, char** argv) {
    const int max_steps = 10; // Maximum number of modifications
    const int max_len = 100; // Maximum length of the message

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char message[max_len]; // Message buffer
    int step = 0;          // Number of modifications made

    if (rank == 0) {
        // Initialize the message in process 0
        snprintf(message, max_len, "Initial message from process %d", rank);
        printf("Process %d starting with message: %s\n", rank, message);

        // Send the message to the next process
        MPI_Send(&step, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
        MPI_Send(message, max_len, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
    }

    while (step < max_steps) {
        // Receive the step count and message
        MPI_Recv(&step, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(message, max_len, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Process %d received step %d with message: %s\n", rank, step, message);

        // Modify the message
        snprintf(message + strlen(message), max_len - strlen(message), " -> modified by process %d", rank);
        step++;

        // Send to the next process
        if (step < max_steps) {
            MPI_Send(&step, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
            MPI_Send(message, max_len, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
        } else {
            // Final message, stop circulating
            printf("Process %d completed the message passing.\n", rank);
        }
    }

    MPI_Finalize();
    return 0;
}
