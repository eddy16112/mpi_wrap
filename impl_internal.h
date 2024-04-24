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

#define IMPL_COMM_NULL 0
#define IMPL_COMM_SELF 1
#define IMPL_COMM_WORLD 2

class IMPL_MPI_Handle {
public:
  IMPL_MPI_Handle(const std::string &mpi_lib);
  ~IMPL_MPI_Handle();

  int (*MPI_Init)(int *argc, char ***argv) = nullptr;
  int (*MPI_Finalize)(void) = nullptr;
  int (*MPI_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
  int (*MPI_Comm_size)(MPI_Comm comm, int *size) = nullptr;
  int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;

public:
  mpi_version_t version;

private:
  void* mpi_so_handle = nullptr;
};

#endif /* MPI_WRAP_IMPL_INTERNAL_H */