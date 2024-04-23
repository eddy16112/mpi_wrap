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

#include <mpi.h>

#include "impl.h"
// #include "impl_fnptr.h"

#include <stddef.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static inline MPI_Comm CONVERT_MPI_Comm(IMPL_Comm comm)
{
  if(comm.ip == (intptr_t)IMPL_COMM_WORLD) {
    return MPI_COMM_WORLD;
  } else if(comm.ip == (intptr_t)IMPL_COMM_SELF) {
    return MPI_COMM_SELF;
  } else if(comm.ip == (intptr_t)IMPL_COMM_NULL) {
    return MPI_COMM_NULL;
  } else {
#ifdef USE_MPICH
    return comm.i;
#elif USE_OMPI
    return comm.p;
#else
#error NO ABI
#endif
  }
}

static void *mpi_so_handle = NULL;

int (*IMPL_Init)(int *argc, char ***argv) = NULL;
int (*IMPL_Comm_rank)(MPI_Comm comm, int *rank);
int (*IMPL_Comm_size)(MPI_Comm comm, int *size);

static inline void *WRAP_DLSYM(void *restrict handle, const char *restrict symbol)
{
  void *fp = dlsym(handle, symbol);
  if(fp == NULL) {
    printf("MUK_DLSYM: failed to find %s - %s\n", symbol, dlerror() );
    fflush(0);
  }
  return fp;
}

int WRAP_Load_functions(char* mpi_lib) 
{
  mpi_so_handle = dlopen(mpi_lib, RTLD_LAZY);
  if(mpi_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", mpi_lib, dlerror());
    abort();
  }

  IMPL_Init = WRAP_DLSYM(mpi_so_handle, "MPI_Init");
  IMPL_Comm_rank = WRAP_DLSYM(mpi_so_handle, "MPI_Comm_rank");
  IMPL_Comm_size = WRAP_DLSYM(mpi_so_handle, "MPI_Comm_size");
  return 0; 
}

int IMPL_WRAP_Init(int *argc, char ***argv) { return IMPL_Init(argc, argv); }

int IMPL_WRAP_Comm_rank(IMPL_Comm comm, int *rank)
{
  MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
  int rc = IMPL_Comm_rank(impl_comm, rank);
  return rc;
}

int IMPL_WRAP_Comm_size(IMPL_Comm comm, int *size)
{
  MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
  int rc = IMPL_Comm_size(impl_comm, size);
  return rc;
}