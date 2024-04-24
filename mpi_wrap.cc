#include "mpi_wrap.h"
#include "impl.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <cstring>

int (*MPI_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
int (*MPI_Comm_size)(MPI_Comm comm, int *size) = nullptr;
int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;

static impl_wrap_handle_t impl_wrap_handle;

static void *wrap_so_handle = nullptr;

static void *mpi_so_handle = nullptr;

static int check_mpi_version(void *handle, int * argc, char *** argv, int requested, int * provided)
{
  // we need to init MPI for MPICH before calling get library version
  int resultlen;
  char lib_version[128];
  int (*MPI_Get_library_version)(char *version, int *resultlen) = nullptr;
  MPI_Get_library_version = reinterpret_cast<int (*)(char *, int *)>(WRAP_DLSYM(
      handle, "MPI_Get_library_version"));
  MPI_Get_library_version(lib_version, &resultlen);
  int version = -1;

  char * pos;
  pos = strstr(lib_version, "Open MPI");
  if (pos != NULL) {
    version = IMPL_OMPI;
  } else {
    pos = strstr(lib_version, "MPICH");
    if (pos != NULL) {
      version = IMPL_MPICH;
    } else {
      assert(0);
    }
  }
  return version;
}

static int mpi_wrap_load(void)
{
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
  int mpi_version = check_mpi_version(mpi_so_handle);
  if (mpi_version == IMPL_MPICH) {
    impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_mpich.so";
    printf("start mpich\n");
  } else if (mpi_version == IMPL_OMPI) {
    impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_ompi.so";
    printf("start ompi\n");
  }

  wrap_so_handle = dlopen(impl_lib.c_str(), RTLD_LAZY);
  if(wrap_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", impl_lib.c_str(), dlerror());
    abort();
  }

  int (*impl_wrap_init_fnptr)(impl_wrap_handle_t *handle) = nullptr;
  impl_wrap_init_fnptr = reinterpret_cast<int (*)(impl_wrap_handle_t *)>(WRAP_DLSYM(
      wrap_so_handle, "impl_wrap_init"));

  impl_wrap_handle.mpi_so_handle = mpi_so_handle;
  impl_wrap_init_fnptr(&impl_wrap_handle);

  MPI_Comm_rank = impl_wrap_handle.MPI_Comm_rank;
  MPI_Comm_size = impl_wrap_handle.MPI_Comm_size;
  MPI_Comm_dup = impl_wrap_handle.MPI_Comm_dup;

  return 0;
}

static int mpi_wrap_unload(void)
{
  int (*impl_wrap_finalize_fnptr)(impl_wrap_handle_t *handle) = nullptr;
  impl_wrap_finalize_fnptr = reinterpret_cast<int (*)(impl_wrap_handle_t *)>(WRAP_DLSYM(
      wrap_so_handle, "impl_wrap_finalize"));
  impl_wrap_finalize_fnptr(&impl_wrap_handle);
  dlclose(wrap_so_handle);
  dlclose(mpi_so_handle);
  return 0;
}

extern "C" {

int MPI_Init(int *argc, char ***argv)
{
  mpi_wrap_load();
  return impl_wrap_handle.MPI_Init(argc, argv);
}

int MPI_Finalize(void) 
{ 
  int rc = impl_wrap_handle.MPI_Finalize();
  mpi_wrap_unload();
  return rc;
}

}
