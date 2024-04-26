#include "mpi_wrap.h"
#include "impl.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

int (*MPI_Query_thread)(int *provided) = nullptr;

int (*MPI_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
int (*MPI_Comm_size)(MPI_Comm comm, int *size) = nullptr;
int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;
int (*MPI_Comm_free)(MPI_Comm *comm) = nullptr;
int (*MPI_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result) = nullptr;

int (*MPI_Send)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) = nullptr;
int (*MPI_Recv)(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) = nullptr;
int (*MPI_Sendrecv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
                    MPI_Status *status) = nullptr;

int (*MPI_Allgather)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) = nullptr;
int (*MPI_Allreduce)(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) = nullptr;
int (*MPI_Barrier)(MPI_Comm comm) = nullptr;
int (*MPI_Bcast)(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) = nullptr;

int (*MPI_Type_get_extent)(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent) = nullptr;
int (*MPI_Get_processor_name)(char *name, int *resultlen) = nullptr;
#ifdef __cplusplus
}
#endif

namespace MUK {

  static impl_wrap_handle_t impl_wrap_handle;

  static bool impl_wrap_handle_initialized = false;

  static void *wrap_so_handle = nullptr;

  static void *mpi_so_handle = nullptr;

  // static int init_mpi(void *handle, int *argc, char ***argv, int requested, int *provided)
  // {
  //   int rc;
  //   if(provided == NULL) {
  //     int (*WRAP_Init)(int *argc, char ***argv) = nullptr;
  //     WRAP_Init =
  //         reinterpret_cast<int (*)(int *, char ***)>(WRAP_DLSYM(handle, "MPI_Init"));
  //     rc = WRAP_Init(argc, argv);
  //   } else {
  //     assert(0);
  //   }
  //   if(rc) {
  //     printf("libinit: MPI initialization failed: %d\n", rc);
  //     abort();
  //   }
  //   return rc;
  // }

  static inline void *WRAP_DLSYM(void *handle, const char *symbol)
  {
    void *fp = dlsym(handle, symbol);
    if(fp == NULL) {
      printf("dlsym: failed to find %s - %s\n", symbol, dlerror());
      fflush(0);
    }
    return fp;
  }

  static int check_mpi_version(void *handle)
  {
    int resultlen;
    char lib_version[IMPL_MAX_LIBRARY_VERSION_STRING];
    int (*WRAP_Get_library_version)(char *version, int *resultlen) = nullptr;
    WRAP_Get_library_version = reinterpret_cast<int (*)(char *, int *)>(WRAP_DLSYM(handle, "MPI_Get_library_version"));
    WRAP_Get_library_version(lib_version, &resultlen);
    int mpi_version = -1;

    char *pos;
    pos = strstr(lib_version, "Open MPI");
    if(pos != NULL) {
      mpi_version = IMPL_OMPI;
    } else {
      pos = strstr(lib_version, "MPICH");
      if(pos != NULL) {
        mpi_version = IMPL_MPICH;
      } else {
        assert(0);
      }
    }
    printf("version %d\n", mpi_version);
    return mpi_version;
  }

  static int mpi_wrap_load(int *argc, char ***argv, int requested, int *provided)
  {
    if(impl_wrap_handle_initialized) {
      return 0;
    }

    char *env = getenv("MPI_LIB");
    int version = atoi(env);
    std::string impl_lib;
    std::string mpi_lib;
    if(version == 1) {
      mpi_lib = "/usr/local/openmpi-4.1.5/lib/libmpi.so";
    } else {
      mpi_lib = "/scratch2/wwu/mpich-4.2.1/install/lib/libmpi.so";
    }

    mpi_so_handle = dlopen(mpi_lib.c_str(), RTLD_LAZY);
    if(mpi_so_handle == NULL) {
      printf("dlopen of %s failed: %s\n", mpi_lib.c_str(), dlerror());
      abort();
    }
    // int mpi_init_status = init_mpi(mpi_so_handle, argc, argv, requested, provided);
    int mpi_version = check_mpi_version(mpi_so_handle);
    if(mpi_version == IMPL_MPICH) {
      impl_lib = "./libimpl_mpich.so";
      printf("start mpich\n");
    } else if(mpi_version == IMPL_OMPI) {
      impl_lib = "./libimpl_ompi.so";
      printf("start ompi\n");
    }

    wrap_so_handle = dlopen(impl_lib.c_str(), RTLD_LAZY);
    if(wrap_so_handle == NULL) {
      printf("dlopen of %s failed: %s\n", impl_lib.c_str(), dlerror());
      abort();
    }

    int (*impl_wrap_init_fnptr)(impl_wrap_handle_t *handle) = nullptr;
    impl_wrap_init_fnptr = reinterpret_cast<int (*)(impl_wrap_handle_t *)>(WRAP_DLSYM(wrap_so_handle, "impl_wrap_init"));

    impl_wrap_handle.mpi_so_handle = mpi_so_handle;
    impl_wrap_init_fnptr(&impl_wrap_handle);

    MPI_Query_thread = impl_wrap_handle.WRAP_Query_thread;

    MPI_Comm_rank = impl_wrap_handle.WRAP_Comm_rank;
    MPI_Comm_size = impl_wrap_handle.WRAP_Comm_size;
    MPI_Comm_dup = impl_wrap_handle.WRAP_Comm_dup;
    MPI_Comm_free = impl_wrap_handle.WRAP_Comm_free;
    MPI_Comm_compare = impl_wrap_handle.WRAP_Comm_compare;

    MPI_Send = impl_wrap_handle.WRAP_Send;
    MPI_Recv = impl_wrap_handle.WRAP_Recv;
    MPI_Sendrecv = impl_wrap_handle.WRAP_Sendrecv;

    MPI_Allgather = impl_wrap_handle.WRAP_Allgather;
    MPI_Allreduce = impl_wrap_handle.WRAP_Allreduce;
    MPI_Barrier = impl_wrap_handle.WRAP_Barrier;
    MPI_Bcast = impl_wrap_handle.WRAP_Bcast;

    MPI_Type_get_extent = impl_wrap_handle.WRAP_Type_get_extent;
    MPI_Get_processor_name = impl_wrap_handle.WRAP_Get_processor_name;

    impl_wrap_handle_initialized = true;

    return 0;
  }

  static int mpi_wrap_unload(void)
  {
    assert(impl_wrap_handle_initialized);
    int (*impl_wrap_finalize_fnptr)(impl_wrap_handle_t *handle) = nullptr;
    impl_wrap_finalize_fnptr = reinterpret_cast<int (*)(impl_wrap_handle_t *)>(WRAP_DLSYM(wrap_so_handle, "impl_wrap_finalize"));
    impl_wrap_finalize_fnptr(&impl_wrap_handle);
    dlclose(wrap_so_handle);
    dlclose(mpi_so_handle);
    impl_wrap_handle_initialized = false;
    return 0;
  }

}; // namespace MUK

extern "C" {

int MPI_Initialized(int *flag)
{
  if(!MUK::impl_wrap_handle_initialized) {
    *flag = 0;
    return MPI_SUCCESS;
  } else {
    int rc = MUK::impl_wrap_handle.WRAP_Initialized(flag);
    return rc;
  }
}

int MPI_Init(int *argc, char ***argv)
{
  int rc = 0;
  MUK::mpi_wrap_load(argc, argv, 0, NULL);
  rc = MUK::impl_wrap_handle.WRAP_Init(argc, argv);
  return rc;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
  int rc = 0;
  MUK::mpi_wrap_load(argc, argv, 0, NULL);
  rc = MUK::impl_wrap_handle.WRAP_Init_thread(argc, argv, required, provided);
  return rc;
}

int MPI_Finalize(void)
{
  int rc = MUK::impl_wrap_handle.WRAP_Finalize();
  MUK::mpi_wrap_unload();
  return rc;
}

int MPI_Finalized(int *flag)
{
  if(!MUK::impl_wrap_handle_initialized) {
    *flag = 1;
    return MPI_SUCCESS;
  } else {
    int rc = MUK::impl_wrap_handle.WRAP_Finalized(flag);
    return rc;
  }
}
}
