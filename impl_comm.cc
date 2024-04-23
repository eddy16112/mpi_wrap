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
#include "impl.h"

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
    return static_cast<MPI_Comm>(comm.p);
#else
#error NO ABI
#endif
  }
}

static inline IMPL_Comm OUTPUT_MPI_Comm(MPI_Comm comm)
{
  IMPL_Comm wrap = {0};
  if(comm == MPI_COMM_NULL) {
    wrap.ip = (intptr_t)IMPL_COMM_NULL;
  } else if(comm == MPI_COMM_WORLD) {
    wrap.ip = (intptr_t)IMPL_COMM_WORLD;
  } else if(comm == MPI_COMM_SELF) {
    wrap.ip = (intptr_t)IMPL_COMM_SELF;
  } else {
#ifdef USE_MPICH
    wrap.i = comm;
#elif USE_OMPI
    wrap.p = comm;
#else
#error NO ABI
#endif
  }
  return wrap;
}

IMPL_MPI_Handle *impl_handle = nullptr;

static impl_wrap_handle_t *impl_wrap_handle = nullptr;


static inline void *WRAP_DLSYM(void *handle, const char *symbol)
{
  void *fp = dlsym(handle, symbol);
  if(fp == NULL) {
    printf("MUK_DLSYM: failed to find %s - %s\n", symbol, dlerror());
    fflush(0);
  }
  return fp;
}

int WRAP_Comm_rank(IMPL_Comm comm, int *rank)
{
  MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
  int rc = impl_handle->MPI_Comm_rank(impl_comm, rank);
  return rc;
}

int WRAP_Comm_size(IMPL_Comm comm, int *size)
{
  MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
  int rc = impl_handle->MPI_Comm_size(impl_comm, size);
  return rc;
}

int WRAP_Comm_dup(IMPL_Comm comm, IMPL_Comm *newcomm)
{
  MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
  MPI_Comm impl_newcomm;
  int rc = impl_handle->MPI_Comm_dup(impl_comm, &impl_newcomm);
  *newcomm = OUTPUT_MPI_Comm(impl_newcomm);
  return rc;
}

int impl_init(IMPL_MPI_Handle *handle, void *mpi_so_handle)
{
  handle->MPI_Init = reinterpret_cast<int (*)(int*, char***)>(WRAP_DLSYM(mpi_so_handle, "MPI_Init"));
  handle->MPI_Finalize = reinterpret_cast<int (*)()>(WRAP_DLSYM(mpi_so_handle, "MPI_Finalize"));
  handle->MPI_Comm_rank = reinterpret_cast<int (*)(MPI_Comm, int*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_rank"));
  handle->MPI_Comm_size = reinterpret_cast<int (*)(MPI_Comm, int*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_size"));
  handle->MPI_Comm_dup = reinterpret_cast<int (*)(MPI_Comm, MPI_Comm*)>(WRAP_DLSYM(mpi_so_handle, "MPI_Comm_dup"));
  return 0;
}

extern "C" {

int impl_wrap_init(impl_wrap_handle_t *handle, const char *mpi_lib)
{
  void *mpi_so_handle = dlopen(mpi_lib, RTLD_LAZY);
  if(mpi_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", mpi_lib, dlerror());
    abort();
  }
  
  impl_handle = new IMPL_MPI_Handle();
  impl_handle->mpi_so_handle = mpi_so_handle;
  impl_init(impl_handle, mpi_so_handle);

  handle->MPI_Init = impl_handle->MPI_Init;
  handle->MPI_Finalize = impl_handle->MPI_Finalize;
  handle->MPI_Comm_rank = WRAP_Comm_rank;
  handle->MPI_Comm_size = WRAP_Comm_size;
  handle->MPI_Comm_dup = WRAP_Comm_dup;

  impl_wrap_handle = handle;
  return 0;
}
}
