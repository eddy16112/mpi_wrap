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

#define WRAP_COMM_NULL 0
#define WRAP_COMM_SELF 1
#define WRAP_COMM_WORLD 2

typedef long int WRAP_Aint;

namespace IMPL {

  class IMPL_MPI_Handle {
  public:
    IMPL_MPI_Handle(void *mpi_so_handle);
    ~IMPL_MPI_Handle();

    int (*IMPL_Init)(int *argc, char ***argv) = nullptr;
    int (*IMPL_Init_thread)(int *argc, char ***argv, int required, int *provided) = nullptr;
    int (*IMPL_Finalize)(void) = nullptr;
    int (*IMPL_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
    int (*IMPL_Comm_size)(MPI_Comm comm, int *size) = nullptr;
    int (*IMPL_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;
    int (*IMPL_Comm_free)(MPI_Comm *comm) = nullptr;
    int (*IMPL_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result) = nullptr;

  private:
    void *mpi_so_handle = nullptr;
  };

}; // namespace IMPL

#endif /* MPI_WRAP_IMPL_INTERNAL_H */