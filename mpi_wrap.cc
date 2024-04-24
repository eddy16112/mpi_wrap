#include "mpi_wrap.h"
#include "impl.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

static void *impl_so_handle = NULL;

int (*WRAP_Load_functions)(char *mpi_lib);

int (*WRAP_Init)(int *argc, char ***argv);
int (*WRAP_Finalize)(void);
int (*MPI_Comm_rank)(MPI_Comm comm, int *rank);
int (*MPI_Comm_size)(MPI_Comm comm, int *size);
int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm);

impl_wrap_handle_t impl_wrap_handle;

int (*impl_wrap_init_fnptr)(impl_wrap_handle_t *handle, const char *mpi_lib);

static int mpi_wrap_load(void)
{
  char *env = getenv("MPI_LIB");
  int version = atoi(env);
  char *impl_name;
  char *mpi_lib;
  if(version == 1) {
    impl_name = "/scratch2/wwu/mpi_wrap/libimpl_ompi.so";
    mpi_lib = "/usr/local/openmpi-4.1.5/lib/libmpi.so";
  } else {
    impl_name = "/scratch2/wwu/mpi_wrap/libimpl_mpich.so";
    mpi_lib = "/scratch2/wwu/mpich-4.2.1/install/lib/libmpi.so";
  }
  impl_so_handle = dlopen(impl_name, RTLD_LAZY);
  if(impl_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", impl_name, dlerror());
    abort();
  }

  printf("done with dlopen\n");

  impl_wrap_init_fnptr = (int (*)(impl_wrap_handle_t *, const char *))WRAP_DLSYM(
      impl_so_handle, "impl_wrap_init");
  impl_wrap_init_fnptr(&impl_wrap_handle, mpi_lib);

  MPI_Comm_rank = impl_wrap_handle.MPI_Comm_rank;
  MPI_Comm_size = impl_wrap_handle.MPI_Comm_size;
  MPI_Comm_dup = impl_wrap_handle.MPI_Comm_dup;

  return 0;
}

extern "C" {

int MPI_Init(int *argc, char ***argv)
{
  mpi_wrap_load();
  return impl_wrap_handle.MPI_Init(argc, argv);
}

int MPI_Finalize(void) { return impl_wrap_handle.MPI_Finalize(); }
}
