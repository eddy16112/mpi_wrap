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

#include "test_common.h"

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
    int *buffer_int1 = init_array<int>(SIZE, me);
    int *buffer_int2 = init_array<int>(SIZE, me + 1);
    if(me == 0) {
      printf("start int\n");
      MPI_Send(buffer_int1, SIZE, MPI_INT, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(int), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_INT, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(int), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<int>(SIZE, buffer_int1, 1);
      check_array<int>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_INT, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<int>(SIZE, buffer_int1);
      update_array<int>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_INT, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_INT, 0, 1, comm);
    }
    free(buffer_int1);
    free(buffer_int2);
  }

  {
    float *buffer_int1 = init_array<float>(SIZE, me);
    float *buffer_int2 = init_array<float>(SIZE, me + 1);
    if(me == 0) {
      printf("start float\n");
      MPI_Send(buffer_int1, SIZE, MPI_FLOAT, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(float), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_FLOAT, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(float), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<float>(SIZE, buffer_int1, 1);
      check_array<float>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_FLOAT, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_FLOAT, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<float>(SIZE, buffer_int1);
      update_array<float>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_FLOAT, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_FLOAT, 0, 1, comm);
    }
    free(buffer_int1);
    free(buffer_int2);
  }

  {
    double *buffer_int1 = init_array<double>(SIZE, me);
    double *buffer_int2 = init_array<double>(SIZE, me + 1);
    if(me == 0) {
      printf("start double\n");
      MPI_Send(buffer_int1, SIZE, MPI_DOUBLE, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(double), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_DOUBLE, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(double), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<double>(SIZE, buffer_int1, 1);
      check_array<double>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_DOUBLE, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_DOUBLE, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<double>(SIZE, buffer_int1);
      update_array<double>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_DOUBLE, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_DOUBLE, 0, 1, comm);
    }

    free(buffer_int1);
    free(buffer_int2);
  }

  {
    char *buffer_int1 = init_array<char>(SIZE, me);
    char *buffer_int2 = init_array<char>(SIZE, me + 1);
    if(me == 0) {
      printf("start char\n");
      MPI_Send(buffer_int1, SIZE, MPI_CHAR, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(char), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_CHAR, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(char), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<char>(SIZE, buffer_int1, 1);
      check_array<char>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_CHAR, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_CHAR, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<char>(SIZE, buffer_int1);
      update_array<char>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_CHAR, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_CHAR, 0, 1, comm);
    }
    free(buffer_int1);
    free(buffer_int2);
  }

  {
    uint64_t *buffer_int1 = init_array<uint64_t>(SIZE, me);
    uint64_t *buffer_int2 = init_array<uint64_t>(SIZE, me + 1);
    if(me == 0) {
      printf("start uint64\n");
      MPI_Send(buffer_int1, SIZE, MPI_UINT64_T, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(uint64_t), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_UINT64_T, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(uint64_t), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<uint64_t>(SIZE, buffer_int1, 1);
      check_array<uint64_t>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_UINT64_T, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_UINT64_T, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<uint64_t>(SIZE, buffer_int1);
      update_array<uint64_t>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_UINT64_T, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_UINT64_T, 0, 1, comm);
    }
    free(buffer_int1);
    free(buffer_int2);
  }

  {
    unsigned long long *buffer_int1 = init_array<unsigned long long>(SIZE, me);
    unsigned long long *buffer_int2 = init_array<unsigned long long>(SIZE, me + 1);
    if(me == 0) {
      printf("start unsigned long long\n");
      MPI_Send(buffer_int1, SIZE, MPI_UNSIGNED_LONG_LONG, 1, 0, comm);
      MPI_Send(buffer_int2, SIZE * sizeof(unsigned long long), MPI_BYTE, 1, 1, comm);
      MPI_Recv(buffer_int1, SIZE, MPI_UNSIGNED_LONG_LONG, 1, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE * sizeof(unsigned long long), MPI_BYTE, 1, 1, comm, MPI_STATUS_IGNORE);
      check_array<unsigned long long>(SIZE, buffer_int1, 1);
      check_array<unsigned long long>(SIZE, buffer_int2, 2);
    } else {
      MPI_Recv(buffer_int1, SIZE, MPI_UNSIGNED_LONG_LONG, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Recv(buffer_int2, SIZE, MPI_UNSIGNED_LONG_LONG, 0, 1, comm, MPI_STATUS_IGNORE);
      update_array<unsigned long long>(SIZE, buffer_int1);
      update_array<unsigned long long>(SIZE, buffer_int2);
      MPI_Send(buffer_int1, SIZE, MPI_UNSIGNED_LONG_LONG, 0, 0, comm);
      MPI_Send(buffer_int2, SIZE, MPI_UNSIGNED_LONG_LONG, 0, 1, comm);
    }
    free(buffer_int1);
    free(buffer_int2);
  }

  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
