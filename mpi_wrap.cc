#include "mpi_wrap.h"
#include "impl.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

int (*MPI_Comm_rank)(MPI_Comm comm, int *rank) = nullptr;
int (*MPI_Comm_size)(MPI_Comm comm, int *size) = nullptr;
int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm) = nullptr;

static impl_wrap_handle_t impl_wrap_handle;

static void *wrap_so_handle = NULL;

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

  // let's pick one to start. It is OK for now, because 
  // version check is ABI stable for all MPIs
  impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_mpich.so";
  int current_impl = IMPL_MPICH;
  wrap_so_handle = dlopen(impl_lib.c_str(), RTLD_LAZY);
  if(wrap_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", impl_lib.c_str(), dlerror());
    abort();
  }
  int (*impl_wrap_get_mpi_version_fnptr)(const char *mpi_lib, mpi_version_t *mpi_version) = nullptr;
  impl_wrap_get_mpi_version_fnptr = reinterpret_cast<int (*)(const char *, mpi_version_t *)>(WRAP_DLSYM(
      wrap_so_handle, "impl_wrap_get_mpi_version"));
  mpi_version_t mpi_version;
  impl_wrap_get_mpi_version_fnptr(mpi_lib.c_str(), &mpi_version);
  
  if (mpi_version.impl == current_impl) {
    printf("start mpich\n");
  } else if (mpi_version.impl == IMPL_OMPI) {
    impl_lib = "/scratch2/wwu/mpi_wrap/libimpl_ompi.so";
    printf("start ompi\n");
    dlclose(wrap_so_handle);
    wrap_so_handle = dlopen(impl_lib.c_str(), RTLD_LAZY);
    if(wrap_so_handle == NULL) {
      printf("dlopen of %s failed: %s\n", impl_lib.c_str(), dlerror());
      abort();
    }
  }

  int (*impl_wrap_init_fnptr)(impl_wrap_handle_t *handle) = nullptr;
  impl_wrap_init_fnptr = reinterpret_cast<int (*)(impl_wrap_handle_t *)>(WRAP_DLSYM(
      wrap_so_handle, "impl_wrap_init"));

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
