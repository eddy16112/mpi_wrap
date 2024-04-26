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

namespace IMPL {

  IMPL_MPI_Handle::IMPL_MPI_Handle(void *mpi_so_handle)
    : mpi_so_handle(mpi_so_handle)
  {
    IMPL_Init = reinterpret_cast<int (*)(int *, char ***)>(WRAP_DLSYM(mpi_so_handle, "MPI_Init"));
    IMPL_Init_thread = reinterpret_cast<int (*)(int *, char ***, int, int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Init_thread"));
    IMPL_Initialized = reinterpret_cast<int (*)(int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Initialized"));
    IMPL_Query_thread = reinterpret_cast<int (*)(int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Query_thread"));
    IMPL_Finalize = reinterpret_cast<int (*)()>(WRAP_DLSYM(mpi_so_handle, "MPI_Finalize"));
    IMPL_Finalized = reinterpret_cast<int (*)(int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Finalized"));

    IMPL_Comm_rank = reinterpret_cast<int (*)(MPI_Comm, int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_rank"));
    IMPL_Comm_size = reinterpret_cast<int (*)(MPI_Comm, int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_size"));
    IMPL_Comm_dup = reinterpret_cast<int (*)(MPI_Comm, MPI_Comm *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_dup"));
    IMPL_Comm_free = reinterpret_cast<int (*)(MPI_Comm *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_free"));
    IMPL_Comm_compare = reinterpret_cast<int (*)(MPI_Comm, MPI_Comm, int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_compare"));

    IMPL_Send = reinterpret_cast<int (*)(const void *, int, MPI_Datatype, int, int, MPI_Comm)>(WRAP_DLSYM(mpi_so_handle, "MPI_Send"));
    IMPL_Recv = reinterpret_cast<int (*)(void *, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Recv"));
    IMPL_Sendrecv = reinterpret_cast<int (*)(const void *, int, MPI_Datatype, int, int, void *, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Sendrecv"));

    IMPL_Allgather = reinterpret_cast<int (*)(const void *, int, MPI_Datatype, void *, int, MPI_Datatype, MPI_Comm)>(WRAP_DLSYM(mpi_so_handle, "MPI_Allgather"));
    IMPL_Allreduce = reinterpret_cast<int (*)(const void *, void *, int, MPI_Datatype, MPI_Op, MPI_Comm)>(WRAP_DLSYM(mpi_so_handle, "MPI_Allreduce"));
    IMPL_Barrier = reinterpret_cast<int (*)(MPI_Comm)>(WRAP_DLSYM(mpi_so_handle, "MPI_Barrier"));
    IMPL_Bcast = reinterpret_cast<int (*)(void *, int, MPI_Datatype, int, MPI_Comm)>(WRAP_DLSYM(mpi_so_handle, "MPI_Bcast"));

    IMPL_Type_get_extent = reinterpret_cast<int (*)(MPI_Datatype, MPI_Aint *, MPI_Aint *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Type_get_extent"));
    IMPL_Get_processor_name = reinterpret_cast<int (*)(char *, int *)>(WRAP_DLSYM(mpi_so_handle, "MPI_Get_processor_name"));
  }

  IMPL_MPI_Handle::~IMPL_MPI_Handle() {}

}; // namespace IMPL