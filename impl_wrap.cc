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

  static inline MPI_Datatype CONVERT_MPI_Datatype(WRAP_Datatype datatype)
  {
    if(datatype.ip == (intptr_t)WRAP_CHAR) {
      return MPI_CHAR;
    } else if(datatype.ip == (intptr_t)WRAP_INT) {
      return MPI_INT;
    } else if(datatype.ip == (intptr_t)WRAP_UNSIGNED_LONG_LONG) {
      return MPI_UNSIGNED_LONG_LONG;
    } else if(datatype.ip == (intptr_t)WRAP_FLOAT) {
      return MPI_FLOAT;
    } else if(datatype.ip == (intptr_t)WRAP_DOUBLE) {
      return MPI_DOUBLE;
    } else if(datatype.ip == (intptr_t)WRAP_INT8_T) {
      return MPI_INT8_T;
    } else if(datatype.ip == (intptr_t)WRAP_INT64_T) {
      return MPI_INT64_T;
    } else if(datatype.ip == (intptr_t)WRAP_UINT8_T) {
      return MPI_UINT8_T;
    } else if(datatype.ip == (intptr_t)WRAP_UINT32_T) {
      return MPI_UINT32_T;
    } else if(datatype.ip == (intptr_t)WRAP_UINT64_T) {
      return MPI_UINT64_T;
    } else if(datatype.ip == (intptr_t)WRAP_BYTE) {
      return MPI_BYTE;
    } else {
      fprintf(stdout, "Unknown data type");
      fflush(stdout);
      assert(0);
    }
  }

  static inline WRAP_Datatype OUTPUT_MPI_Datatype(MPI_Datatype datatype)
  {
    WRAP_Datatype wrap = {0};
    if(datatype == MPI_CHAR) {
      wrap.ip = (intptr_t)WRAP_CHAR;
    } else if(datatype == MPI_INT) {
      wrap.ip = (intptr_t)WRAP_INT;
    } else if(datatype == MPI_UNSIGNED_LONG_LONG) {
      wrap.ip = (intptr_t)WRAP_UNSIGNED_LONG_LONG;
    } else if(datatype == MPI_FLOAT) {
      wrap.ip = (intptr_t)WRAP_FLOAT;
    } else if(datatype == MPI_DOUBLE) {
      wrap.ip = (intptr_t)WRAP_DOUBLE;
    } else if(datatype == MPI_INT8_T) {
      wrap.ip = (intptr_t)WRAP_INT8_T;
    } else if(datatype == MPI_INT64_T) {
      wrap.ip = (intptr_t)WRAP_INT64_T;
    } else if(datatype == MPI_UINT8_T) {
      wrap.ip = (intptr_t)WRAP_UINT8_T;
    } else if(datatype == MPI_UINT32_T) {
      wrap.ip = (intptr_t)WRAP_UINT32_T;
    } else if(datatype == MPI_UINT64_T) {
      wrap.ip = (intptr_t)WRAP_UINT64_T;
    } else if(datatype == MPI_BYTE) {
      wrap.ip = (intptr_t)WRAP_BYTE;
    } else {
      fprintf(stdout, "Unknown data type");
      fflush(stdout);
      assert(0);
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

  static int WRAP_Send(const void *buf, int count, WRAP_Datatype datatype, int dest, int tag, WRAP_Comm comm)
  {
    // we do not support predefined ranks such as MPI_ANY_SOURCE
    assert(dest >= 0);
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Send(buf, count, impl_datatype, dest, tag, impl_comm);
    return rc;
  }

  int WRAP_Recv(void *buf, int count, WRAP_Datatype datatype, int source, int tag, WRAP_Comm comm, WRAP_Status *status)
  {
    assert(status == WRAP_STATUS_IGNORE);
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Recv(buf, count, impl_datatype, source, tag, impl_comm, MPI_STATUS_IGNORE);
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

    // static_assert(WRAP_STATUS_IGNORE == MPI_STATUS_IGNORE);

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

  handle->WRAP_Send = IMPL::WRAP_Send;
  handle->WRAP_Recv = IMPL::WRAP_Recv;

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
