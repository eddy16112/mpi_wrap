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
    buffer[i] = i + value;
  }
  return buffer;
}

template <typename T>
void check_array(size_t size, T *buffer, T expected)
{
  for(size_t i = 0; i < size; i++) {
    if(buffer[i] != expected + (T)i) {
      std::cout << "wrong i " << i << ", expected " << expected + (T)i << ", actual " << buffer[i] << std::endl << std::flush;
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
    int *send_buf1 = init_array<int>(SIZE, me);
    int *recv_buf1 = init_array<int>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start int\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_INT, sendto, 0, recv_buf1, SIZE, MPI_INT, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<int>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  {
    float *send_buf1 = init_array<float>(SIZE, me);
    float *recv_buf1 = init_array<float>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start float\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_FLOAT, sendto, 0, recv_buf1, SIZE, MPI_FLOAT, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<float>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  {
    double *send_buf1 = init_array<double>(SIZE, me);
    double *recv_buf1 = init_array<double>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start double\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_DOUBLE, sendto, 0, recv_buf1, SIZE, MPI_DOUBLE, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<double>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  {
    char *send_buf1 = init_array<char>(SIZE, me);
    char *recv_buf1 = init_array<char>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start char\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_CHAR, sendto, 0, recv_buf1, SIZE, MPI_CHAR, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<char>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  {
    uint64_t *send_buf1 = init_array<uint64_t>(SIZE, me);
    uint64_t *recv_buf1 = init_array<uint64_t>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start uint64_t\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_UINT64_T, sendto, 0, recv_buf1, SIZE, MPI_UINT64_T, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<uint64_t>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  {
    unsigned long long *send_buf1 = init_array<unsigned long long>(SIZE, me);
    unsigned long long *recv_buf1 = init_array<unsigned long long>(SIZE, me);
    int sendto, recvfrom;
    sendto = me + 1;
    recvfrom = me - 1;
    if(me == 0) {
      printf("start unsigned long long\n");
      recvfrom = np - 1;
    } else if(me == np - 1) {
      sendto = 0;
    }
    MPI_Sendrecv(send_buf1, SIZE, MPI_UNSIGNED_LONG_LONG, sendto, 0, recv_buf1, SIZE, MPI_UNSIGNED_LONG_LONG, recvfrom, 0, comm, MPI_STATUS_IGNORE);
    check_array<unsigned long long>(SIZE, recv_buf1, recvfrom);

    free(send_buf1);
    free(recv_buf1);
  }

  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
