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

#include "impl_internal.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

IMPL_MPI_Handle::IMPL_MPI_Handle(const std::string &mpi_lib)
{
  mpi_so_handle = dlopen(mpi_lib.c_str(), RTLD_LAZY);
  if(mpi_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", mpi_lib.c_str(), dlerror());
    abort();
  }

  MPI_Init = reinterpret_cast<int (*)(int*, char***)>(WRAP_DLSYM(mpi_so_handle, "MPI_Init"));
  MPI_Finalize = reinterpret_cast<int (*)()>(WRAP_DLSYM(mpi_so_handle, "MPI_Finalize"));
  MPI_Comm_rank = reinterpret_cast<int (*)(MPI_Comm, int*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_rank"));
  MPI_Comm_size = reinterpret_cast<int (*)(MPI_Comm, int*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_size"));
  MPI_Comm_dup = reinterpret_cast<int (*)(MPI_Comm, MPI_Comm*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_dup"));
}

IMPL_MPI_Handle::~IMPL_MPI_Handle()
{
  dlclose(mpi_so_handle);
}