set -e
set -x

MPICH_DIR=/scratch2/wwu/mpich-4.2.1/install
OMPI_DIR=/usr/local/openmpi-4.1.5

OMPI_RUN=${OMPI_DIR}/bin/mpirun
MPICH_RUN=${MPICH_DIR}/bin/mpirun

export MPI_LIB=0
${MPICH_RUN} -np 2 ./test_type
${MPICH_RUN} -np 2 ./test_comm
${MPICH_RUN} -np 2 ./test_comm_c
${MPICH_RUN} -np 2 ./test_pingpong
${MPICH_RUN} -np 2 ./test_pingpong_c
${MPICH_RUN} -np 4 ./test_sendrecv
${MPICH_RUN} -np 4 ./test_allreduce
${MPICH_RUN} -np 4 ./test_bcast
${MPICH_RUN} -np 4 ./test_allgather
${MPICH_RUN} -np 3 ./test_alltoall
${MPICH_RUN} -np 4 ./test_gather

export MPI_LIB=1
${OMPI_RUN} -np 2 ./test_type
${OMPI_RUN} -np 2 ./test_comm
${OMPI_RUN} -np 2 ./test_comm_c
${OMPI_RUN} -np 2 ./test_pingpong
${OMPI_RUN} -np 2 ./test_pingpong_c
${OMPI_RUN} -np 4 ./test_sendrecv
${OMPI_RUN} -np 4 ./test_allreduce
${OMPI_RUN} -np 4 ./test_bcast
${OMPI_RUN} -np 4 ./test_allgather
${OMPI_RUN} -np 3 ./test_alltoall
${OMPI_RUN} -np 4 ./test_gather
