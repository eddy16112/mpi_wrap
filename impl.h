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

#ifndef MPI_WRAP_IMPL_H
#define MPI_WRAP_IMPL_H

#include "impl_wrap_type.h"

#define IMPL_MAX_LIBRARY_VERSION_STRING 8192

typedef struct impl_wrap_handle_s {
  void *mpi_so_handle;

  // init and finalize
  int (*WRAP_Init)(int *argc, char ***argv);
  int (*WRAP_Init_thread)(int *argc, char ***argv, int required, int *provided);
  int (*WRAP_Initialized)(int *flag);
  int (*WRAP_Query_thread)(int *provided);
  int (*WRAP_Finalize)(void);
  int (*WRAP_Finalized)(int *flag);

  // communicator
  int (*WRAP_Comm_rank)(WRAP_Comm comm, int *rank);
  int (*WRAP_Comm_size)(WRAP_Comm comm, int *size);
  int (*WRAP_Comm_dup)(WRAP_Comm comm, WRAP_Comm *newcomm);
  int (*WRAP_Comm_free)(WRAP_Comm *comm);
  int (*WRAP_Comm_compare)(WRAP_Comm comm1, WRAP_Comm comm2, int *result);

  // p2p
  int (*WRAP_Send)(const void *buf, int count, WRAP_Datatype datatype, int dest, int tag, WRAP_Comm comm);
  int (*WRAP_Recv)(void *buf, int count, WRAP_Datatype datatype, int source, int tag, WRAP_Comm comm, WRAP_Status *status);
  int (*WRAP_Sendrecv)(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, WRAP_Datatype recvtype, int source, int recvtag, WRAP_Comm comm,
                       WRAP_Status *status);

  // collective
  int (*WRAP_Barrier)(WRAP_Comm comm) = nullptr;

  int (*WRAP_Type_get_extent)(WRAP_Datatype datatype, WRAP_Aint *lb, WRAP_Aint *extent);
  int (*WRAP_Get_processor_name)(char *name, int *resultlen);
} impl_wrap_handle_t;

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) int impl_wrap_init(impl_wrap_handle_t *handle);
__attribute__((visibility("default"))) int impl_wrap_finalize(impl_wrap_handle_t *handle);
#ifdef __cplusplus
}
#endif

#endif /* MPI_WRAP_IMPL_H */