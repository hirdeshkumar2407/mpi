#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <map>

// Error message map
std::map<int, std::string> error_messages = {
    {MPI_SUCCESS, "No error"},
    {MPI_ERR_BUFFER, "Invalid buffer pointer"},
    {MPI_ERR_COUNT, "Invalid count argument"},
    {MPI_ERR_TYPE, "Invalid datatype argument"},
    {MPI_ERR_TAG, "Invalid tag argument"},
    {MPI_ERR_COMM, "Invalid communicator"},
    {MPI_ERR_RANK, "Invalid rank"},
    {MPI_ERR_REQUEST, "Invalid request (handle)"},
    {MPI_ERR_ROOT, "Invalid root"},
    {MPI_ERR_GROUP, "Invalid group"},
    {MPI_ERR_OP, "Invalid operation"},
    {MPI_ERR_TOPOLOGY, "Invalid topology"},
    {MPI_ERR_DIMS, "Invalid dimension argument"},
    {MPI_ERR_ARG, "Invalid argument of some other kind"},
    {MPI_ERR_UNKNOWN, "Unknown error"},
    {MPI_ERR_TRUNCATE, "Message truncated on receive"},
    {MPI_ERR_OTHER, "Known error not in this list"},
    {MPI_ERR_INTERN, "Internal MPI (implementation) error"},
    {MPI_ERR_IN_STATUS, "Error code is in status"},
    {MPI_ERR_PENDING, "Pending request"},
    {MPI_ERR_KEYVAL, "Invalid keyval has been passed"},
    {MPI_ERR_NO_MEM, "MPI_ALLOC_MEM failed because memory is exhausted"},
    {MPI_ERR_BASE, "Invalid base passed to MPI_FREE_MEM"},
    {MPI_ERR_INFO_KEY, "Key longer than MPI_MAX_INFO_KEY"},
    {MPI_ERR_INFO_VALUE, "Value longer than MPI_MAX_INFO_VAL"},
    {MPI_ERR_INFO_NOKEY, "Invalid key passed to MPI_INFO_DELETE"},
    {MPI_ERR_SPAWN, "Error in spawning processes"},
    {MPI_ERR_PORT, "Invalid port name passed to MPI_COMM_CONNECT"},
    {MPI_ERR_SERVICE, "Invalid service name passed to MPI_UNPUBLISH_NAME"},
    {MPI_ERR_NAME, "Invalid service name passed to MPI_LOOKUP_NAME"},
    {MPI_ERR_WIN, "Invalid win argument"},
    {MPI_ERR_SIZE, "Invalid size argument"},
    {MPI_ERR_DISP, "Invalid disp argument"},
    {MPI_ERR_INFO, "Invalid info argument"},
    {MPI_ERR_LOCKTYPE, "Invalid locktype argument"},
    {MPI_ERR_ASSERT, "Invalid assert argument"},
    {MPI_ERR_RMA_CONFLICT, "Conflicting accesses to window"},
    {MPI_ERR_RMA_SYNC, "Wrong synchronization of RMA calls"},
    {MPI_ERR_FILE, "Invalid file handle"},
    {MPI_ERR_NOT_SAME, "Collective argument not identical on all processes"},
    {MPI_ERR_AMODE, "Error related to the amode passed to MPI_FILE_OPEN"},
    {MPI_ERR_UNSUPPORTED_DATAREP, "Unsupported datarep passed to MPI_FILE_SET_VIEW"},
    {MPI_ERR_UNSUPPORTED_OPERATION, "Unsupported operation, such as seeking on a file which supports sequential access only"},
    {MPI_ERR_NO_SUCH_FILE, "File does not exist"},
    {MPI_ERR_FILE_EXISTS, "File exists"},
    {MPI_ERR_BAD_FILE, "Invalid file name (e.g., path name too long)"},
    {MPI_ERR_ACCESS, "Permission denied"},
    {MPI_ERR_NO_SPACE, "Not enough space"},
    {MPI_ERR_QUOTA, "Quota exceeded"},
    {MPI_ERR_READ_ONLY, "Read-only file or file system"},
    {MPI_ERR_FILE_IN_USE, "File operation could not be completed, as the file is currently open by some process"},
    {MPI_ERR_DUP_DATAREP, "Conversion functions could not be registered because a data representation identifier was already defined"},
    {MPI_ERR_CONVERSION, "An error occurred in a user-supplied data conversion function"},
    {MPI_ERR_IO, "Other I/O error"},
    {MPI_ERR_LASTCODE, "Last error code"}
};

// Function to check MPI errors
void checkMPIError(int err_code) {
    if (err_code != MPI_SUCCESS) {
        std::cout << "MPI Error: " << error_messages[err_code] << std::endl;
        MPI_Abort(MPI_COMM_WORLD, err_code);
    }
}

// Random number generator
void fill_matrix_with_random_values(std::vector<int>& matrix, int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);

    for (int i = 0; i < size; ++i) {
        matrix[i] = dis(gen);
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 5; // Rows of A
    const int M = 10; // Columns of B
    const int K = 5; // Columns of A and rows of B

    // Flattened matrices for communication
    std::vector<int> A_flat(N * K);
    std::vector<int> B_flat(K * M);
    std::vector<int> C_flat(N * M, 0);

    // Fill matrices A and B with random values
    if (rank == 0) {
        fill_matrix_with_random_values(A_flat, N * K);
        fill_matrix_with_random_values(B_flat, K * M);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize processes before starting the timer
    double start_time, end_time;

    // Synchronous version
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Scatter matrix A and B for multiplication
    std::vector<int> local_A(K), local_C(M);
    checkMPIError(MPI_Scatter(A_flat.data(), K, MPI_INT, local_A.data(), K, MPI_INT, 0, MPI_COMM_WORLD));
    checkMPIError(MPI_Bcast(B_flat.data(), K * M, MPI_INT, 0, MPI_COMM_WORLD));

    // Matrix multiplication for each process
    for (int i = 0; i < M; ++i) {
        local_C[i] = 0;
        for (int j = 0; j < K; ++j) {
            local_C[i] += local_A[j] * B_flat[j * M + i];  // Access flattened B_flat matrix
        }
    }

    // Gather the results back to process 0
    checkMPIError(MPI_Gather(local_C.data(), M, MPI_INT, C_flat.data(), M, MPI_INT, 0, MPI_COMM_WORLD));

    if (rank == 0) {
        end_time = MPI_Wtime();
        std::cout << "Synchronous Matrix multiplication completed in " << (end_time - start_time) << " seconds." << std::endl;
    }

    // Asynchronous version
    MPI_Request send_request, recv_request;
    MPI_Status send_status, recv_status;

    // Scatter matrix A and B for multiplication asynchronously
    checkMPIError(MPI_Iscatter(A_flat.data(), K, MPI_INT, local_A.data(), K, MPI_INT, 0, MPI_COMM_WORLD, &recv_request));
    checkMPIError(MPI_Ibcast(B_flat.data(), K * M, MPI_INT, 0, MPI_COMM_WORLD, &send_request));

    // Wait for scatter and broadcast to complete
    checkMPIError(MPI_Wait(&recv_request, &recv_status));
    checkMPIError(MPI_Wait(&send_request, &send_status));

    // Matrix multiplication for each process
    for (int i = 0; i < M; ++i) {
        local_C[i] = 0;
        for (int j = 0; j < K; ++j) {
            local_C[i] += local_A[j] * B_flat[j * M + i];  // Access flattened B_flat matrix
        }
    }

    // Gather the results back to process 0
    checkMPIError(MPI_Igather(local_C.data(), M, MPI_INT, C_flat.data(), M, MPI_INT, 0, MPI_COMM_WORLD, &recv_request));
    checkMPIError(MPI_Wait(&recv_request, &recv_status));

    if (rank == 0) {
        end_time = MPI_Wtime();
        std::cout << "Asynchronous Matrix multiplication completed in " << (end_time - start_time) << " seconds." << std::endl;
    }

    MPI_Finalize();
    return 0;
}
