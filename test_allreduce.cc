#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>

#include <unistd.h>

#ifdef USE_REAL_MPI
#include <mpi.h>
#else
#include "mpi_wrap.h"
#endif

template <typename T>
T *init_array(size_t size, T value)
{
  T *buffer = (T *)malloc(sizeof(T) * size);
  for(size_t i = 0; i < size; i++) {
    buffer[i] = value;
  }
  return buffer;
}

template <typename T>
void check_array(size_t size, T *buffer, T expected)
{
  for(size_t i = 0; i < size; i++) {
    if(buffer[i] != expected) {
      std::cout << "wrong i " << i << ", expected " << expected << ", actual " << buffer[i] << std::endl << std::flush;
      assert(0);
    }
  }
}

int main(int argc, char *argv[])
{
  int rc, provided;
  rc = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  assert(rc == MPI_SUCCESS);

  int me, np;
  MPI_Comm comm;
  MPI_Comm_dup(MPI_COMM_WORLD, &comm);
  MPI_Comm_rank(comm, &me);
  MPI_Comm_size(comm, &np);
  printf("I am %d of %d\n", me, np);

  int SIZE = 100;

  {
    unsigned long long expected = (np-1)*np/2;
    unsigned long long *send_buf1 = init_array<unsigned long long>(SIZE, me);
    unsigned long long *recv_buf1 = init_array<unsigned long long>(SIZE, me);
    MPI_Allreduce(send_buf1, recv_buf1, SIZE, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
    check_array<unsigned long long>(SIZE, recv_buf1, expected);

    free(send_buf1);
    free(recv_buf1);
  }

  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
