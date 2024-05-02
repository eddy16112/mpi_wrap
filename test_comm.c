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
#include "muk.h"
#endif

void init(void)
{
#ifndef USE_REAL_MPI
  MUK_Init_handle();
#endif
}

void finalize(void)
{
#ifndef USE_REAL_MPI
  MUK_Finalize_handle();
#endif
}

int main(int argc, char *argv[])
{
#ifdef USE_REAL_MPI
  {
    int resultlen;
    char lib_version[MPI_MAX_LIBRARY_VERSION_STRING];
    MPI_Get_library_version(lib_version, &resultlen);
  }
#endif

  init();
  int rc, provided, flag;
  MUK_Initialized(&flag);
  assert(flag == 0);
  rc = MUK_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  assert(rc == MPI_SUCCESS);
  MUK_Initialized(&flag);
  assert(flag == 1);
  MUK_Query_thread(&provided);
  assert(provided == MPI_THREAD_MULTIPLE);

  int me, np;
  MPI_Comm comms[2];
  comms[0] = MPI_COMM_WORLD;
  MUK_Comm_dup(comms[0], &(comms[1]));
  char name[128];
  int resultlen;
  MUK_Get_processor_name(name, &resultlen);
  for(int i = 0; i < 2; i++) {
    MUK_Comm_rank(comms[i], &me);
    MUK_Comm_size(comms[i], &np);
    printf("I am %d of %d, host %s, resultlen %d\n", me, np, name, resultlen);
    MUK_Barrier(comms[i]);
  }

  // assert(comms[0] == (MPI_Comm)MPI_COMM_WORLD);

  int result;
  MUK_Comm_compare(comms[0], comms[1], &result);
  assert(result == MPI_CONGRUENT);
  
  MUK_Comm_free(&(comms[1]));

  MUK_Finalized(&flag);
  assert(flag == 0);
  MUK_Finalize();
  MUK_Finalized(&flag);
  assert(flag == 1);
  finalize();
  return 0;
}
