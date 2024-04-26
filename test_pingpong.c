#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>

#ifdef USE_REAL_MPI
#include <mpi.h>
#else
#include "mpi_wrap.h"
#endif

int *init_int32(size_t size, int value)
{
  int *buffer = (int*)malloc(sizeof(int) * size);
  for (size_t i = 0; i < size; i++) {
    buffer[i] = i + value;
  }
  return buffer;
}

void update_int32(size_t size, int* buffer)
{
  for (size_t i = 0; i < size; i++) {
    buffer[i] += 1;
  }
}

void check_int32(size_t size, int* buffer, int expected)
{
  for (size_t i = 0; i < size; i++) {
    if (buffer[i] != expected + (int)i) {
      fprintf(stdout, "wrong i %lu, expected %d, actual %d\n", i, expected, buffer[i]);
      fflush(stdout);
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

  int SIZE=100;

  int *buffer_int1 = init_int32(SIZE, me);
  int *buffer_int2 = init_int32(SIZE, me+1);
  if (me == 0) {
    MPI_Send(buffer_int1, SIZE, MPI_INT, 1, 0, comm);
    MPI_Send(buffer_int2, SIZE * sizeof(int), MPI_BYTE, 1, 1, comm);
    MPI_Recv(buffer_int1, SIZE, MPI_INT, 1, 0, comm, MPI_STATUS_IGNORE);
    MPI_Recv(buffer_int2, SIZE * sizeof(int), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
    check_int32(SIZE, buffer_int1, 1);
    check_int32(SIZE, buffer_int2, 2);
  } else {
    MPI_Recv(buffer_int1, SIZE, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);
    MPI_Recv(buffer_int2, SIZE, MPI_INT, 0, 1, comm, MPI_STATUS_IGNORE);
    update_int32(SIZE, buffer_int1);
    update_int32(SIZE, buffer_int2);
    MPI_Send(buffer_int1, SIZE, MPI_INT, 0, 0, comm);
    MPI_Send(buffer_int2, SIZE, MPI_INT, 0, 1, comm);
  }

  free(buffer_int1);
  free(buffer_int2);
  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
