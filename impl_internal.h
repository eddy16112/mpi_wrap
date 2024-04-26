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

#ifndef MPI_WRAP_IMPL_INTERNAL_H
#define MPI_WRAP_IMPL_INTERNAL_H

#include <mpi.h>
#include <string>

#include "impl.h"

namespace IMPL {

  class IMPL_MPI_Handle {
  public:
    IMPL_MPI_Handle(void *mpi_so_handle);
    ~IMPL_MPI_Handle();

    // init and finalize
    int (*IMPL_Init)(int *argc, char ***argv) = nullptr;
    int (*IMPL_Init_thread)(int *argc, char ***argv, int required, int *provided) = nullptr;
    int (*IMPL_Initialized)(int *flag) = nullptr;
    int (*IMPL_Query_thread)(int *provided) = nullptr;
    int (*IMPL_Finalize)(void) = nullptr;
    int (*IMPL_Finalized)(int *flag) = nullptr;

    // communicator
    int (*IMPL_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
    int (*IMPL_Comm_size)(MPI_Comm comm, int *size) = nullptr;
    int (*IMPL_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;
    int (*IMPL_Comm_free)(MPI_Comm *comm) = nullptr;
    int (*IMPL_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result) = nullptr;

    // p2p
    int (*IMPL_Send)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) = nullptr;
    int (*IMPL_Recv)(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) = nullptr;
    int (*IMPL_Sendrecv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
                         MPI_Status *status) = nullptr;

    int (*IMPL_Type_get_extent)(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent) = nullptr;
    int (*IMPL_Get_processor_name)(char *name, int *resultlen) = nullptr;

  private:
    void *mpi_so_handle = nullptr;
  };

  static inline void *WRAP_DLSYM(void *handle, const char *symbol)
  {
    void *fp = dlsym(handle, symbol);
    if(fp == NULL) {
      printf("dlsym: failed to find %s - %s\n", symbol, dlerror());
      fflush(0);
    }
    return fp;
  }

}; // namespace IMPL

#endif /* MPI_WRAP_IMPL_INTERNAL_H */