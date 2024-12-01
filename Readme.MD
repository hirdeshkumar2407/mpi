# MPI Concepts: A Revision Guide

This guide provides an overview of the key concepts related to **Message Passing Interface (MPI)**, specifically the asynchronous operations and techniques discussed for matrix partitioning, parallel programming, and debugging. The guide also includes explanations of error codes and debugging techniques to help troubleshoot common MPI-related issues.

## Table of Contents
1. [Overview of MPI](#overview-of-mpi)
2. [Basic MPI Functions](#basic-mpi-functions)
3. [Asynchronous MPI Operations](#asynchronous-mpi-operations)

---

## 1. Overview of MPI

**MPI** (Message Passing Interface) is a standard for parallel programming, allowing processes to communicate with each other in distributed computing systems. MPI is designed for high-performance applications, and it supports both **synchronous** and **asynchronous** communication models.

### Key Features of MPI:
- **Point-to-point communication**: Communication between two processes.
- **Collective communication**: Communication involving multiple processes (e.g., scatter, gather).
- **Synchronization**: Controlling when processes communicate and coordinate tasks.
- **Non-blocking operations**: Operations that allow processes to continue their execution while communication occurs in the background.

---

## 2. Basic MPI Functions

Here are some of the basic MPI functions you’ll encounter when working with MPI in scientific computing and parallel programming:

### 1. `MPI_Init` and `MPI_Finalize`
- **`MPI_Init(&argc, &argv)`**: Initializes the MPI environment. It must be called before any other MPI function.
- **`MPI_Finalize()`**: Terminates the MPI environment and should be called once all MPI operations are done.

### 2. Process Communication Functions
- **`MPI_Comm_rank(MPI_Comm comm, int* rank)`**: Determines the rank (ID) of the calling process within a communicator.
- **`MPI_Comm_size(MPI_Comm comm, int* size)`**: Gets the total number of processes in the communicator.

### 3. Data Distribution Functions
- **`MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)`**: Distributes chunks of data from the root process to all other processes.
- **`MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)`**: Gathers data from all processes to the root process.
- **`MPI_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)`**: Broadcasts data from the root process to all other processes.

### 4. Non-blocking Communication
- **`MPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request* request)`**: Initiates a non-blocking send operation.
- **`MPI_Irecv(void* buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request* request)`**: Initiates a non-blocking receive operation.
- **`MPI_Wait(MPI_Request* request, MPI_Status* status)`**: Waits for a non-blocking operation to complete.
- **`MPI_Test(MPI_Request* request, int* flag, MPI_Status* status)`**: Tests whether a non-blocking operation has completed.

---

## 3. Asynchronous MPI Operations

### Non-blocking Communication in Detail:
- **`MPI_Iscatter`**: Distributes chunks of data asynchronously from the root to all processes. This allows processes to continue computing while the scatter operation is happening in the background.
- **`MPI_Igather`**: Gathers data asynchronously from all processes to the root process.
- **`MPI_Ibcast`**: Broadcasts data asynchronously from the root to all processes, allowing other processes to do computations while waiting for the broadcast to finish.

#### Example: Asynchronous Scatter and Gather
```cpp
MPI_Request send_request, recv_request;
MPI_Status status;

// Initiate non-blocking scatter
MPI_Iscatter(A.data(), K, MPI_INT, local_A.data(), K, MPI_INT, 0, MPI_COMM_WORLD, &recv_request);
MPI_Ibcast(B.data(), K * M, MPI_INT, 0, MPI_COMM_WORLD, &send_request);

// Wait for both scatter and broadcast to finish
MPI_Wait(&recv_request, &status);
MPI_Wait(&send_request, &status);
