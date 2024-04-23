// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#ifdef USE_REALM_MPI
#include <mpi.h>
#else
#include "mpi_wrap.h"
#endif


int main(int argc, char *argv[])
{
  int rc;
  rc = MPI_Init(&argc, &argv);

  // printf("main: MPI_COMM_WORLD=%lx\n", (intptr_t)MPI_COMM_WORLD);

  int me, np;
  MPI_Comm comms[2];
  comms[0] = MPI_COMM_WORLD;
  (*MPI_Comm_dup)(comms[0], &(comms[1]));
  for(int i = 0; i < 2; i++) {
    MPI_Comm_rank(comms[i], &me);
    MPI_Comm_size(comms[i], &np);
    printf("I am %d of %d\n", me, np);
  }

  MPI_Finalize();
  return rc;
}
