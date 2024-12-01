#include <mpi.h>
#include <iostream>
#include <vector>
#include <map>

int main(int argc, char** argv) {
    int rank, size;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Error messages map
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

    // Create a vector with size equal to the number of processes
    std::vector<int> numbers(size);
    int local_sum = 0;

    // Only the master process initializes the data
    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            numbers[i] = i + 1; // Fill the vector with numbers 1, 2, ..., size
            std::cout << "Process " << rank << ": numbers[" << i << "] = " << numbers[i] << std::endl;
        }
    }

    // Declare the request object for non-blocking communication
    MPI_Request send_request, recv_request;
    MPI_Status send_status, recv_status;

    // Non-blocking scatter
    int local_number = 0;
    std::cout << "Process " << rank << ": Before MPI_Iscatter, sending data..." << std::endl;
    int scatter_err = MPI_Iscatter(numbers.data(), 1, MPI_INT, &local_number, 1, MPI_INT, 0, MPI_COMM_WORLD, &recv_request);

    // Error handling for MPI_Iscatter
    if (scatter_err != MPI_SUCCESS) {
        std::cerr << "Error in MPI_Iscatter on process " << rank << ": " 
                  << error_messages[scatter_err] << std::endl;
        MPI_Abort(MPI_COMM_WORLD, scatter_err);
        return 1;
    }

    // Wait for scatter to complete (since this is non-blocking)
    MPI_Wait(&recv_request, &recv_status);
    std::cout << "Process " << rank << ": After MPI_Iscatter, received local number " << local_number << std::endl;

    // Each process computes the sum of its number
    local_sum = local_number;
    std::cout << "Process " << rank << ": Local sum = " << local_sum << std::endl;

    // Non-blocking reduce
    int global_sum = 0;
    std::cout << "Process " << rank << ": Before MPI_Ireduce, local_sum = " << local_sum << std::endl;
    int reduce_err = MPI_Ireduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD, &send_request);

    // Error handling for MPI_Ireduce
    if (reduce_err != MPI_SUCCESS) {
        std::cerr << "Error in MPI_Ireduce on process " << rank << ": " 
                  << error_messages[reduce_err] << std::endl;
        MPI_Abort(MPI_COMM_WORLD, reduce_err);
        return 1;
    }

    // Wait for the reduce to complete
    MPI_Wait(&send_request, &send_status);

    // Only the root process will print the result
    if (rank == 0) {
        std::cout << "Process " << rank << ": The global sum is " << global_sum << std::endl;
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
