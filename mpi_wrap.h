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
typedef WRAP_Op MPI_Op;
typedef WRAP_Status MPI_Status;

// predefined communicators
const MPI_Comm MPI_COMM_NULL = {.ip = WRAP_COMM_NULL};
const MPI_Comm MPI_COMM_SELF = {.ip = WRAP_COMM_SELF};
const MPI_Comm MPI_COMM_WORLD = {.ip = WRAP_COMM_WORLD};

// predefined data types
const MPI_Datatype MPI_CHAR = {.ip = WRAP_CHAR};
const MPI_Datatype MPI_INT = {.ip = WRAP_INT};
const MPI_Datatype MPI_UNSIGNED_LONG_LONG = {.ip = WRAP_UNSIGNED_LONG_LONG};
const MPI_Datatype MPI_FLOAT = {.ip = WRAP_FLOAT};
const MPI_Datatype MPI_DOUBLE = {.ip = WRAP_DOUBLE};
const MPI_Datatype MPI_INT8_T = {.ip = WRAP_INT8_T};
const MPI_Datatype MPI_INT64_T = {.ip = WRAP_INT64_T};
const MPI_Datatype MPI_UINT8_T = {.ip = WRAP_UINT8_T};
const MPI_Datatype MPI_UINT32_T = {.ip = WRAP_UINT32_T};
const MPI_Datatype MPI_UINT64_T = {.ip = WRAP_UINT64_T};
const MPI_Datatype MPI_BYTE = {.ip = WRAP_BYTE};

// predefined ops
const MPI_Op MPI_SUM = {.ip = WRAP_SUM};

// predefined status
#define MPI_STATUS_IGNORE WRAP_STATUS_IGNORE
#define MPI_IN_PLACE WRAP_IN_PLACE

#define MPI_SUCCESS WRAP_SUCCESS

#define MPI_CONGRUENT 1

#define MPI_THREAD_SINGLE WRAP_THREAD_SINGLE
#define MPI_THREAD_FUNNELED WRAP_THREAD_FUNNELED
#define MPI_THREAD_SERIALIZED WRAP_THREAD_SERIALIZED
#define MPI_THREAD_MULTIPLE WRAP_THREAD_MULTIPLE

typedef WRAP_Aint MPI_Aint;

#ifdef __cplusplus
extern "C" {
#endif

// //#define MPI_COMM_WORLD 2
// const MPI_Comm MPI_COMM_WORLD = {.ip = 2};

extern int MPI_Init(int *argc, char ***argv);
extern int MPI_Init_thread(int *argc, char ***argv, int required, int *provided);
extern int MPI_Initialized(int *flag);
extern int MPI_Finalize(void);
extern int MPI_Finalized(int *flag);

extern int (*MPI_Query_thread)(int *provided);

extern int (*MPI_Comm_rank)(MPI_Comm comm, int *rank);
extern int (*MPI_Comm_size)(MPI_Comm comm, int *size);
extern int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm);
extern int (*MPI_Comm_free)(MPI_Comm *comm);
extern int (*MPI_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result);

extern int (*MPI_Send)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
extern int (*MPI_Recv)(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
extern int (*MPI_Sendrecv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                           MPI_Comm comm, MPI_Status *status);

extern int (*MPI_Allreduce)(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
extern int (*MPI_Barrier)(MPI_Comm comm);

extern int (*MPI_Type_get_extent)(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent);
extern int (*MPI_Get_processor_name)(char *name, int *resultlen);
#ifdef __cplusplus
}
#endif

#endif /* MPI_WRAP_H */