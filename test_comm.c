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

int main(int argc, char *argv[])
{
#ifdef USE_REAL_MPI
  {
    int resultlen;
    char lib_version[MPI_MAX_LIBRARY_VERSION_STRING];
    MPI_Get_library_version(lib_version, &resultlen);
  }
#endif

  int rc, provided, flag;
  MPI_Initialized(&flag);
  assert(flag == 0);
  rc = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  assert(rc == MPI_SUCCESS);
  MPI_Initialized(&flag);
  assert(flag == 1);
  MPI_Query_thread(&provided);
  assert(provided == MPI_THREAD_MULTIPLE);

  int me, np;
  MPI_Comm comms[2];
  comms[0] = MPI_COMM_WORLD;
  MPI_Comm_dup(comms[0], &(comms[1]));
  char name[128];
  int resultlen;
  MPI_Get_processor_name(name, &resultlen);
  for(int i = 0; i < 2; i++) {
    MPI_Comm_rank(comms[i], &me);
    MPI_Comm_size(comms[i], &np);
    printf("I am %d of %d, host %s, resultlen %d\n", me, np, name, resultlen);
    MPI_Barrier(comms[i]);
  }

  int result;
  MPI_Comm_compare(comms[0], comms[1], &result);
  assert(result == MPI_CONGRUENT);
  
  MPI_Comm_free(&(comms[1]));

  MPI_Finalized(&flag);
  assert(flag == 0);
  MPI_Finalize();
  MPI_Finalized(&flag);
  assert(flag == 1);
  return 0;
}
