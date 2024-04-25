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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace IMPL {

  IMPL_MPI_Handle *impl_mpi_handle = nullptr;

  static impl_wrap_handle_t *impl_wrap_handle = nullptr;

  static inline MPI_Comm CONVERT_MPI_Comm(WRAP_Comm comm)
  {
    if(comm.ip == (intptr_t)WRAP_COMM_WORLD) {
      return MPI_COMM_WORLD;
    } else if(comm.ip == (intptr_t)WRAP_COMM_SELF) {
      return MPI_COMM_SELF;
    } else if(comm.ip == (intptr_t)WRAP_COMM_NULL) {
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

  static inline WRAP_Comm OUTPUT_MPI_Comm(MPI_Comm comm)
  {
    WRAP_Comm wrap = {0};
    if(comm == MPI_COMM_NULL) {
      wrap.ip = (intptr_t)WRAP_COMM_NULL;
    } else if(comm == MPI_COMM_WORLD) {
      wrap.ip = (intptr_t)WRAP_COMM_WORLD;
    } else if(comm == MPI_COMM_SELF) {
      wrap.ip = (intptr_t)WRAP_COMM_SELF;
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

  // the following functions are the wrapper of MPI functions

  static int WRAP_Comm_rank(WRAP_Comm comm, int *rank)
  {
    MPI_Comm WRAP_Comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Comm_rank(WRAP_Comm, rank);
    return rc;
  }

  static int WRAP_Comm_size(WRAP_Comm comm, int *size)
  {
    MPI_Comm WRAP_Comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Comm_size(WRAP_Comm, size);
    return rc;
  }

  static int WRAP_Comm_dup(WRAP_Comm comm, WRAP_Comm *newcomm)
  {
    MPI_Comm WRAP_Comm = CONVERT_MPI_Comm(comm);
    MPI_Comm impl_newcomm;
    int rc = impl_mpi_handle->IMPL_Comm_dup(WRAP_Comm, &impl_newcomm);
    *newcomm = OUTPUT_MPI_Comm(impl_newcomm);
    return rc;
  }

  static int WRAP_Comm_free(WRAP_Comm *comm)
  {
    int rc;
    MPI_Comm WRAP_Comm = CONVERT_MPI_Comm(*comm);
    rc = impl_mpi_handle->IMPL_Comm_free(&WRAP_Comm);
    *comm = OUTPUT_MPI_Comm(WRAP_Comm);
    return rc;
  }

  static int WRAP_Comm_compare(WRAP_Comm comm1, WRAP_Comm comm2, int *result)
  {
    MPI_Comm WRAP_Comm1 = CONVERT_MPI_Comm(comm1);
    MPI_Comm WRAP_Comm2 = CONVERT_MPI_Comm(comm2);
    int rc = impl_mpi_handle->IMPL_Comm_compare(WRAP_Comm1, WRAP_Comm2, result);
    return rc;
  }

  static int check_mpi_types(void)
  {
    static_assert(sizeof(MPI_Aint) == sizeof(WRAP_Aint));
    static_assert(IMPL_MAX_LIBRARY_VERSION_STRING >= MPI_MAX_LIBRARY_VERSION_STRING);

    static_assert(static_cast<int>(WRAP_THREAD_SINGLE) == static_cast<int>(MPI_THREAD_SINGLE));
    static_assert(static_cast<int>(WRAP_THREAD_FUNNELED) == static_cast<int>(MPI_THREAD_FUNNELED));
    static_assert(static_cast<int>(WRAP_THREAD_SERIALIZED) == static_cast<int>(MPI_THREAD_SERIALIZED));
    static_assert(static_cast<int>(WRAP_THREAD_MULTIPLE) == static_cast<int>(MPI_THREAD_MULTIPLE));

    return 0;
  }
}; // namespace IMPL

extern "C" {

int impl_wrap_init(impl_wrap_handle_t *handle)
{
  IMPL::check_mpi_types();

  assert(IMPL::impl_mpi_handle == nullptr);
  IMPL::impl_mpi_handle = new IMPL::IMPL_MPI_Handle(handle->mpi_so_handle);

  // the following functions are ABI stable, so we can pass the MPI implementation directly
  handle->WRAP_Init = IMPL::impl_mpi_handle->IMPL_Init;
  handle->WRAP_Init_thread = IMPL::impl_mpi_handle->IMPL_Init_thread;
  handle->WRAP_Initialized = IMPL::impl_mpi_handle->IMPL_Initialized;
  handle->WRAP_Query_thread = IMPL::impl_mpi_handle->IMPL_Query_thread;
  handle->WRAP_Finalize = IMPL::impl_mpi_handle->IMPL_Finalize;
  handle->WRAP_Finalized = IMPL::impl_mpi_handle->IMPL_Finalized;

  // the following functions are not ABI stable, so we need a wrapper
  handle->WRAP_Comm_rank = IMPL::WRAP_Comm_rank;
  handle->WRAP_Comm_size = IMPL::WRAP_Comm_size;
  handle->WRAP_Comm_dup = IMPL::WRAP_Comm_dup;
  handle->WRAP_Comm_free = IMPL::WRAP_Comm_free;
  handle->WRAP_Comm_compare = IMPL::WRAP_Comm_compare;

  IMPL::impl_wrap_handle = handle;

  return 0;
}

int impl_wrap_finalize(impl_wrap_handle_t *handle)
{
  assert(handle == IMPL::impl_wrap_handle);
  delete IMPL::impl_mpi_handle;
  IMPL::impl_mpi_handle = nullptr;
  return 0;
}
}
