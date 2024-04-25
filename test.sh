set -e
set -x

MPICH_DIR=/scratch2/wwu/mpich-4.2.1/install
OMPI_DIR=/usr/local/openmpi-4.1.5

OMPI_RUN=${OMPI_DIR}/bin/mpirun
MPICH_RUN=${MPICH_DIR}/bin/mpirun

export MPI_LIB=0
${MPICH_RUN} -np 2 ./test_comm
${MPICH_RUN} -np 2 ./test_comm_c

export MPI_LIB=1
${OMPI_RUN} -np 2 ./test_comm
${OMPI_RUN} -np 2 ./test_comm_c
