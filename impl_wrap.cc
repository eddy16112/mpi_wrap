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
      abort();
    }
    return wrap;
  }

  static inline MPI_Info CONVERT_MPI_Info(WRAP_Info info)
  {
    if(info.ip == (intptr_t)WRAP_INFO_NULL) {
      return MPI_INFO_NULL;
    } else if(info.ip == (intptr_t)WRAP_INFO_ENV) {
      return MPI_INFO_ENV;
    } else {
#ifdef USE_MPICH
      return info.i;
#elif USE_OMPI
      return static_cast<MPI_Info>(info.p);
#else
#error NO ABI
#endif
    }
  }

  static inline WRAP_Info OUTPUT_MPI_Info(MPI_Info info)
  {
    WRAP_Info wrap = {0};
    if(info == MPI_INFO_NULL) {
      wrap.ip = (intptr_t)WRAP_INFO_NULL;
    } else if(info == MPI_INFO_ENV) {
      wrap.ip = (intptr_t)WRAP_INFO_ENV;
    } else {
#ifdef USE_MPICH
      wrap.i = info;
#elif USE_OMPI
      wrap.p = static_cast<MPI_Info>(info);
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

  static int WRAP_Comm_split(WRAP_Comm comm, int color, int key, WRAP_Comm *newcomm)
  {
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    MPI_Comm impl_newcomm;
    int rc = impl_mpi_handle->IMPL_Comm_split(impl_comm, color, key, &impl_newcomm);
    *newcomm = OUTPUT_MPI_Comm(impl_newcomm);
    return rc;
  }

  static int WRAP_Comm_split_type(WRAP_Comm comm, int split_type, int key, WRAP_Info info, WRAP_Comm *newcomm)
  {
    // this is the only place this conversion is required, so just inline it
    int impl_type = MPI_UNDEFINED;
    if(split_type == WRAP_COMM_TYPE_SHARED) {
      impl_type = MPI_COMM_TYPE_SHARED;
    } else {
      fprintf(stdout, "unsupported split_type\n");
      fflush(stdout);
      abort();
    }
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    MPI_Info impl_info = CONVERT_MPI_Info(info);
    MPI_Comm impl_newcomm;
    int rc = impl_mpi_handle->IMPL_Comm_split_type(impl_comm, impl_type, key, impl_info, &impl_newcomm);
    *newcomm = OUTPUT_MPI_Comm(impl_newcomm);
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

  static int WRAP_Recv(void *buf, int count, WRAP_Datatype datatype, int source, int tag, WRAP_Comm comm, WRAP_Status *status)
  {
    assert(status == WRAP_STATUS_IGNORE);
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Recv(buf, count, impl_datatype, source, tag, impl_comm, MPI_STATUS_IGNORE);
    return rc;
  }

  static int WRAP_Sendrecv(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, WRAP_Datatype recvtype, int source, int recvtag,
                           WRAP_Comm comm, WRAP_Status *status)
  {
    assert(status == WRAP_STATUS_IGNORE);
    MPI_Datatype impl_sendtype = CONVERT_MPI_Datatype(sendtype);
    MPI_Datatype impl_recvtype = CONVERT_MPI_Datatype(recvtype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Sendrecv(sendbuf, sendcount, impl_sendtype, dest, sendtag, recvbuf, recvcount, impl_recvtype, source, recvtag, impl_comm, MPI_STATUS_IGNORE);
    return rc;
  }

  static int WRAP_Allgather(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, void *recvbuf, int recvcount, WRAP_Datatype recvtype, WRAP_Comm comm)
  {
    // we do not have a use case of inplace
    assert((intptr_t)sendbuf != WRAP_IN_PLACE);
    MPI_Datatype impl_sendtype = CONVERT_MPI_Datatype(sendtype);
    MPI_Datatype impl_recvtype = CONVERT_MPI_Datatype(recvtype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Allgather(sendbuf, sendcount, impl_sendtype, recvbuf, recvcount, impl_recvtype, impl_comm);
    return rc;
  }

  static int WRAP_Allgatherv(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], WRAP_Datatype recvtype, WRAP_Comm comm)
  {
    // we do not have a use case of inplace
    assert((intptr_t)sendbuf != WRAP_IN_PLACE);
    MPI_Datatype impl_sendtype = CONVERT_MPI_Datatype(sendtype);
    MPI_Datatype impl_recvtype = CONVERT_MPI_Datatype(recvtype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Allgatherv(sendbuf, sendcount, impl_sendtype, recvbuf, recvcounts, displs, impl_recvtype, impl_comm);
    return rc;
  }

  static int WRAP_Allreduce(const void *sendbuf, void *recvbuf, int count, WRAP_Datatype datatype, WRAP_Op op, WRAP_Comm comm)
  {
    // we do not have a use case of inplace
    assert((intptr_t)sendbuf != WRAP_IN_PLACE);
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    MPI_Op impl_op;
    if(op.ip == (intptr_t)WRAP_SUM) {
      impl_op = MPI_SUM;
    } else {
      fprintf(stdout, "unsupported op\n");
      fflush(stdout);
      abort();
    }
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Allreduce(sendbuf, recvbuf, count, impl_datatype, impl_op, impl_comm);
    return rc;
  }

  static int WRAP_Alltoall(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, void *recvbuf, int recvcount, WRAP_Datatype recvtype, WRAP_Comm comm)
  {
    // we do not have a use case of inplace
    assert((intptr_t)sendbuf != WRAP_IN_PLACE);
    MPI_Datatype impl_sendtype = CONVERT_MPI_Datatype(sendtype);
    MPI_Datatype impl_recvtype = CONVERT_MPI_Datatype(recvtype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Alltoall(sendbuf, sendcount, impl_sendtype, recvbuf, recvcount, impl_recvtype, impl_comm);
    return rc;
  }

  static int WRAP_Barrier(WRAP_Comm comm)
  {
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Barrier(impl_comm);
    return rc;
  }

  static int WRAP_Bcast(void *buffer, int count, WRAP_Datatype datatype, int root, WRAP_Comm comm)
  {
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Bcast(buffer, count, impl_datatype, root, impl_comm);
    return rc;
  }

  static int WRAP_Gather(const void *sendbuf, int sendcount, WRAP_Datatype sendtype, void *recvbuf, int recvcount, WRAP_Datatype recvtype, int root, WRAP_Comm comm)
  {
    // we do not have a use case of inplace
    assert((intptr_t)sendbuf != WRAP_IN_PLACE);
    MPI_Datatype impl_sendtype = CONVERT_MPI_Datatype(sendtype);
    MPI_Datatype impl_recvtype = CONVERT_MPI_Datatype(recvtype);
    MPI_Comm impl_comm = CONVERT_MPI_Comm(comm);
    int rc = impl_mpi_handle->IMPL_Gather(sendbuf, sendcount, impl_sendtype, recvbuf, recvcount, impl_recvtype, root, impl_comm);
    return rc;
  }

  static int WRAP_Type_get_extent(WRAP_Datatype datatype, WRAP_Aint *lb, WRAP_Aint *extent)
  {
    MPI_Datatype impl_datatype = CONVERT_MPI_Datatype(datatype);
    int rc = impl_mpi_handle->IMPL_Type_get_extent(impl_datatype, lb, extent);
    return rc;
  }

  static int check_mpi_types(void)
  {
    static_assert(std::is_pod<impl_wrap_handle_t>::value);
    static_assert(std::is_pod<WRAP_Comm>::value);
    static_assert(std::is_pod<WRAP_Datatype>::value);
    static_assert(std::is_pod<WRAP_Op>::value);
    static_assert(std::is_pod<WRAP_Status>::value);

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
  handle->WRAP_Get_processor_name = IMPL::impl_mpi_handle->IMPL_Get_processor_name;

  // the following functions are not ABI stable, so we need a wrapper
  handle->WRAP_Comm_rank = IMPL::WRAP_Comm_rank;
  handle->WRAP_Comm_size = IMPL::WRAP_Comm_size;
  handle->WRAP_Comm_dup = IMPL::WRAP_Comm_dup;
  handle->WRAP_Comm_free = IMPL::WRAP_Comm_free;
  handle->WRAP_Comm_compare = IMPL::WRAP_Comm_compare;
  handle->WRAP_Comm_split = IMPL::WRAP_Comm_split;
  handle->WRAP_Comm_split_type = IMPL::WRAP_Comm_split_type;

  handle->WRAP_Send = IMPL::WRAP_Send;
  handle->WRAP_Recv = IMPL::WRAP_Recv;
  handle->WRAP_Sendrecv = IMPL::WRAP_Sendrecv;

  handle->WRAP_Allgather = IMPL::WRAP_Allgather;
  handle->WRAP_Allgatherv = IMPL::WRAP_Allgatherv;
  handle->WRAP_Allreduce = IMPL::WRAP_Allreduce;
  handle->WRAP_Alltoall = IMPL::WRAP_Alltoall;
  handle->WRAP_Barrier = IMPL::WRAP_Barrier;
  handle->WRAP_Bcast = IMPL::WRAP_Bcast;
  handle->WRAP_Gather = IMPL::WRAP_Gather;

  handle->WRAP_Type_get_extent = IMPL::WRAP_Type_get_extent;

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
