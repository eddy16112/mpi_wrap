#include "mpi_wrap.h"
// #include "impl_fnptr.h"

#include <stddef.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

static void *impl_so_handle = NULL;

int (*WRAP_Load_functions)(char *mpi_lib);

int (*WRAP_Init)(int *argc, char ***argv);
int (*WRAP_Comm_rank)(MPI_Comm comm, int *rank);
int (*WRAP_Comm_size)(MPI_Comm comm, int *size);

static inline void *WRAP_DLSYM(void *restrict handle, const char *restrict symbol)
{
  void *fp = dlsym(handle, symbol);
  if(fp == NULL) {
    printf("MUK_DLSYM: failed to find %s - %s\n", symbol, dlerror() );
    fflush(0);
  }
  return fp;
}

static int mpi_wrap_load(void)
{
  char * env = getenv("MPI_LIB");
  int version = atoi(env);
  char *impl_name;
  char *mpi_lib;
  if (version == 1) {
    impl_name = "/scratch2/wwu/mpi_wrap/libimpl-ompi.so";
    mpi_lib = "/usr/local/openmpi-4.1.5/lib/libmpi.so";
  } else {
    impl_name = "/scratch2/wwu/mpi_wrap/libimpl-mpich.so";
    mpi_lib = "/scratch2/wwu/mpich-4.2.1/install/lib/libmpi.so";
  }
  impl_so_handle = dlopen(impl_name, RTLD_LAZY);
  if(impl_so_handle == NULL) {
    printf("dlopen of %s failed: %s\n", impl_name, dlerror());
    abort();
  }

  printf("done with dlopen\n");

  WRAP_Init = WRAP_DLSYM(impl_so_handle, "IMPL_WRAP_Init");
  WRAP_Comm_rank = WRAP_DLSYM(impl_so_handle, "IMPL_WRAP_Comm_rank");
  WRAP_Comm_size = WRAP_DLSYM(impl_so_handle, "IMPL_WRAP_Comm_size");

  WRAP_Load_functions = WRAP_DLSYM(impl_so_handle, "WRAP_Load_functions");
  WRAP_Load_functions(mpi_lib);

  return 0;
}

int MPI_Init(int *argc, char ***argv)
{
  mpi_wrap_load();
  return WRAP_Init(argc, argv);
}

int MPI_Comm_rank(MPI_Comm comm, int *rank) { return WRAP_Comm_rank(comm, rank); }

int MPI_Comm_size(MPI_Comm comm, int *size) { return WRAP_Comm_size(comm, size); }