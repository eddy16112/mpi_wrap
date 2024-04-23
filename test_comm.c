// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "mpi_wrap.h"

int main(int argc, char* argv[])
{
    int rc;
    rc = MPI_Init(&argc,&argv);

    // printf("main: MPI_COMM_WORLD=%lx\n", (intptr_t)MPI_COMM_WORLD);

    int me, np;
    MPI_Comm_rank(MPI_COMM_WORLD,&me);
    MPI_Comm_size(MPI_COMM_WORLD,&np);
    printf("I am %d of %d\n", me, np);
    return rc;
}
