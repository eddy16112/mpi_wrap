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
  printf("Bcast, I am %d of %d\n", me, np);

  int SIZE = 100;

  for (int i = 0; i < np; i++) {
    int expected = i;
    int *send_buf = init_array<int>(SIZE, me);
    MPI_Bcast(send_buf, SIZE * sizeof(int), MPI_BYTE, i, comm);
    check_array<int>(SIZE, send_buf, expected);

    free(send_buf);
  }

  MPI_Comm_free(&comm);

  MPI_Finalize();
  return 0;
}
