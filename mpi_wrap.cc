#include "muk.h"
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

int (*MUK_Init)(int *argc, char ***argv) = NULL;
int (*MUK_Init_thread)(int *argc, char ***argv, int required, int *provided) = NULL;
int (*MUK_Initialized)(int *flag) = NULL;
int (*MUK_Finalize)(void) = NULL;
int (*MUK_Finalized)(int *flag) = NULL;
int (*MUK_Query_thread)(int *provided) = NULL;

int (*MUK_Comm_rank)(MPI_Comm comm, int *rank) = NULL;
int (*MUK_Comm_size)(MPI_Comm comm, int *size) = NULL;
int (*MUK_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = NULL;
int (*MUK_Comm_free)(MPI_Comm *comm) = NULL;
int (*MUK_Comm_compare)(MPI_Comm comm1, MPI_Comm comm2, int *result) = NULL;
int (*MUK_Comm_split)(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) = NULL;
int (*MUK_Comm_split_type)(MPI_Comm comm, int split_type, int key, MPI_Info info, MPI_Comm *newcomm) = NULL;

int (*MUK_Send)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) = NULL;
int (*MUK_Recv)(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) = NULL;
int (*MUK_Sendrecv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
                    MPI_Status *status) = NULL;

int (*MUK_Allgather)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) = NULL;
int (*MUK_Allgatherv)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm) = NULL;
int (*MUK_Allreduce)(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) = NULL;
int (*MUK_Alltoall)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) = NULL;
int (*MUK_Barrier)(MPI_Comm comm) = NULL;
int (*MUK_Bcast)(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) = NULL;
int (*MUK_Gather)(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) = NULL;

int (*MUK_Type_get_extent)(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent) = NULL;
int (*MUK_Get_processor_name)(char *name, int *resultlen) = NULL;
#ifdef __cplusplus
}
#endif

namespace MUK {

  static impl_wrap_handle_t impl_wrap_handle;

  static bool impl_wrap_handle_initialized = false;

  // -1: unknown, 0: false, 1:true
  static int mpi_initialized = -1;
  static int mpi_finalized = -1;

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

  static int mpi_wrap_load(void)
  {
    assert(!impl_wrap_handle_initialized);

    char *env = getenv("MPI_LIB");
    int version = atoi(env);
    std::string impl_lib;
    std::string mpi_lib;
    if(version == 1) {
      mpi_lib = "/scratch2/wwu/ompi-5/lib/libmpi.so";
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
      impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_mpich.so";
      printf("start mpich\n");
    } else if(mpi_version == IMPL_OMPI) {
      impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_ompi.so";
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

    MUK_Init = impl_wrap_handle.WRAP_Init;
    MUK_Init_thread = impl_wrap_handle.WRAP_Init_thread;
    MUK_Initialized = impl_wrap_handle.WRAP_Initialized;
    MUK_Finalize = impl_wrap_handle.WRAP_Finalize;
    MUK_Finalized = impl_wrap_handle.WRAP_Finalized;
    MUK_Query_thread = impl_wrap_handle.WRAP_Query_thread;

    MUK_Comm_rank = impl_wrap_handle.WRAP_Comm_rank;
    MUK_Comm_size = impl_wrap_handle.WRAP_Comm_size;
    MUK_Comm_dup = impl_wrap_handle.WRAP_Comm_dup;
    MUK_Comm_free = impl_wrap_handle.WRAP_Comm_free;
    MUK_Comm_compare = impl_wrap_handle.WRAP_Comm_compare;
    MUK_Comm_split = impl_wrap_handle.WRAP_Comm_split;
    MUK_Comm_split_type = impl_wrap_handle.WRAP_Comm_split_type;

    MUK_Send = impl_wrap_handle.WRAP_Send;
    MUK_Recv = impl_wrap_handle.WRAP_Recv;
    MUK_Sendrecv = impl_wrap_handle.WRAP_Sendrecv;

    MUK_Allgather = impl_wrap_handle.WRAP_Allgather;
    MUK_Allgatherv = impl_wrap_handle.WRAP_Allgatherv;
    MUK_Allreduce = impl_wrap_handle.WRAP_Allreduce;
    MUK_Alltoall = impl_wrap_handle.WRAP_Alltoall;
    MUK_Barrier = impl_wrap_handle.WRAP_Barrier;
    MUK_Bcast = impl_wrap_handle.WRAP_Bcast;
    MUK_Gather = impl_wrap_handle.WRAP_Gather;

    MUK_Type_get_extent = impl_wrap_handle.WRAP_Type_get_extent;
    MUK_Get_processor_name = impl_wrap_handle.WRAP_Get_processor_name;

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

int MUK_Init_handle(void)
{
  return MUK::mpi_wrap_load();
}

int MUK_Finalize_handle(void)
{
  return MUK::mpi_wrap_unload();
}

}
