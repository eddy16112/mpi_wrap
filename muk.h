/* Copyright 2024 NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MPI_WRAP_H
#define MPI_WRAP_H

#include "impl_wrap_type.h"

typedef WRAP_Comm MPI_Comm;
typedef WRAP_Datatype MPI_Datatype;
typedef WRAP_Info MPI_Info;
typedef WRAP_Op MPI_Op;
typedef WRAP_Status MPI_Status;

// predefined communicators
#define MPI_COMM_NULL (MPI_Comm){.ip = WRAP_COMM_NULL}
#define MPI_COMM_SELF (MPI_Comm){.ip = WRAP_COMM_SELF}
#define MPI_COMM_WORLD (MPI_Comm){.ip = WRAP_COMM_WORLD}

// predefined data types
#define MPI_CHAR (MPI_Datatype){.ip = WRAP_CHAR}
#define MPI_INT (MPI_Datatype){.ip = WRAP_INT}
#define MPI_UNSIGNED_LONG_LONG (MPI_Datatype){.ip = WRAP_UNSIGNED_LONG_LONG}
#define MPI_FLOAT (MPI_Datatype){.ip = WRAP_FLOAT}
#define MPI_DOUBLE (MPI_Datatype){.ip = WRAP_DOUBLE}
#define MPI_INT8_T (MPI_Datatype){.ip = WRAP_INT8_T}
#define MPI_INT64_T (MPI_Datatype){.ip = WRAP_INT64_T}
#define MPI_UINT8_T (MPI_Datatype){.ip = WRAP_UINT8_T}
#define MPI_UINT32_T (MPI_Datatype){.ip = WRAP_UINT32_T}
#define MPI_UINT64_T (MPI_Datatype){.ip = WRAP_UINT64_T}
#define MPI_BYTE (MPI_Datatype){.ip = WRAP_BYTE}

// predefined info
const MPI_Info MPI_INFO_NULL = {.ip = WRAP_INFO_NULL};

// predefined ops
const MPI_Op MPI_SUM = {.ip = WRAP_SUM};

// predefined status
#define MPI_STATUS_IGNORE WRAP_STATUS_IGNORE
#define MPI_IN_PLACE WRAP_IN_PLACE

#define MPI_SUCCESS WRAP_SUCCESS

#define MPI_CONGRUENT WRAP_CONGRUENT

#define MPI_THREAD_SINGLE WRAP_THREAD_SINGLE
#define MPI_THREAD_FUNNELED WRAP_THREAD_FUNNELED
#define MPI_THREAD_SERIALIZED WRAP_THREAD_SERIALIZED
#define MPI_THREAD_MULTIPLE WRAP_THREAD_MULTIPLE

#define MPI_COMM_TYPE_SHARED WRAP_COMM_TYPE_SHARED

#define MPI_MAX_PROCESSOR_NAME 256

typedef WRAP_Aint MPI_Aint;

#ifdef __cplusplus
extern "C" {
#endif

// //#define MPI_COMM_WORLD 2
// const MPI_Comm MPI_COMM_WORLD = {.ip = 2};

// these two functions must be called before calling any MPI functions
extern int MUK_Init_handle(void);
extern int MUK_Finalize_handle(void);

extern int (*MUK_Init)(int *argc, char ***argv);
extern int (*MUK_Init_thread)(int *argc, char ***argv, int required, int *provided);
extern int (*MUK_Initialized)(int *flag);
extern int (*MUK_Finalize)(void);
extern int (*MUK_Finalized)(int *flag);
extern int (*MUK_Query_thread)(int *provided);

extern int (*MUK_Comm_rank)(MPI_Comm comm, int *rank);
extern int (*MUK_Comm_size)(MPI_Comm comm, int *size);
extern int (*MUK_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm);
extern int (*MUK_Comm_free)(MPI_Comm *comm);
extern int (*MUK_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result);
extern int (*MUK_Comm_split)(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
extern int (*MUK_Comm_split_type)(MPI_Comm comm, int split_type, int key, MPI_Info info, MPI_Comm *newcomm);

extern int (*MUK_Send)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
extern int (*MUK_Recv)(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
extern int (*MUK_Sendrecv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                           MPI_Comm comm, MPI_Status *status);

extern int (*MUK_Allgather)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
extern int (*MUK_Allgatherv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm);
extern int (*MUK_Allreduce)(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
extern int (*MUK_Alltoall)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
extern int (*MUK_Barrier)(MPI_Comm comm);
extern int (*MUK_Bcast)(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
extern int (*MUK_Gather)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);

extern int (*MUK_Type_get_extent)(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent);
extern int (*MUK_Get_processor_name)(char *name, int *resultlen);
#ifdef __cplusplus
}
#endif

#endif /* MPI_WRAP_H */