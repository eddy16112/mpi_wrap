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

  MPI_Aint lb, extent;
  MPI_Type_get_extent(MPI_CHAR, &lb, &extent);
  assert(sizeof(char) == extent);

  MPI_Type_get_extent(MPI_INT, &lb, &extent);
  assert(sizeof(int) == extent);

  MPI_Type_get_extent(MPI_UNSIGNED_LONG_LONG, &lb, &extent);
  assert(sizeof(unsigned long long) == extent);

  MPI_Type_get_extent(MPI_FLOAT, &lb, &extent);
  assert(sizeof(float) == extent);

  MPI_Type_get_extent(MPI_DOUBLE, &lb, &extent);
  assert(sizeof(double) == extent);

  MPI_Type_get_extent(MPI_INT8_T, &lb, &extent);
  assert(sizeof(int8_t) == extent);

  MPI_Type_get_extent(MPI_INT64_T, &lb, &extent);
  assert(sizeof(int64_t) == extent);

  MPI_Type_get_extent(MPI_UINT8_T, &lb, &extent);
  assert(sizeof(uint8_t) == extent);

  MPI_Type_get_extent(MPI_UINT32_T, &lb, &extent);
  assert(sizeof(uint32_t) == extent);

  MPI_Type_get_extent(MPI_UINT64_T, &lb, &extent);
  assert(sizeof(uint64_t) == extent);

  MPI_Type_get_extent(MPI_BYTE, &lb, &extent);
  assert(1 == extent);

  printf("type passed\n");
  MPI_Finalize();
  return 0;
}
