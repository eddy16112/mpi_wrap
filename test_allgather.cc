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

template <typename T>
void check_result(size_t size, T *buffer, int np)
{
  for(int rank = 0; rank < np; rank++) {
    for(size_t i = 0; i < size; i++) {
      if(buffer[rank * size + i] != rank) {
        std::cout << "wrong rank " << rank << ",i " << i << ", expected " << rank << ", actual " << buffer[rank * size + i] << std::endl << std::flush;
        assert(0);
      }
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
  printf("Allgather, I am %d of %d\n", me, np);

  int SIZE = 100;

  {
    int *send_buf = init_array_const<int>(SIZE, me);
    int *recv_buf = init_array_const<int>(SIZE * np, me);
    MPI_Allgather(send_buf, SIZE, MPI_INT, recv_buf, SIZE, MPI_INT, comm); 
    check_result<int>(SIZE, recv_buf, np);

    free(send_buf);
    free(recv_buf);
  }

  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
