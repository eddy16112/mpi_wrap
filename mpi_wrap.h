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

#include "impl_common.h"

typedef IMPL_Handle MPI_Comm;

const MPI_Comm MPI_COMM_NULL = {.ip = 0};
const MPI_Comm MPI_COMM_SELF = {.ip = 1};
const MPI_Comm MPI_COMM_WORLD = {.ip = 2};

#define MPI_SUCCESS 0

#define MPI_CONGRUENT 1

#define MPI_THREAD_SINGLE WRAP_THREAD_SINGLE
#define MPI_THREAD_FUNNELED WRAP_THREAD_FUNNELED
#define MPI_THREAD_SERIALIZED WRAP_THREAD_SERIALIZED
#define MPI_THREAD_MULTIPLE WRAP_THREAD_MULTIPLE

typedef ptrdiff_t MPI_Aint;

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

#ifdef __cplusplus
}
#endif

#endif /* MPI_WRAP_H */