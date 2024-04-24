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

#ifndef MPI_WRAP_IMPL_H
#define MPI_WRAP_IMPL_H

#include "impl_common.h"

typedef IMPL_Handle IMPL_Comm;
typedef IMPL_Handle IMPL_Datatype;

typedef struct impl_wrap_handle_s {
  int (*MPI_Init)(int *argc, char ***argv);
  int (*MPI_Finalize)(void);
  int (*MPI_Comm_rank)(IMPL_Comm comm, int *rank);
  int (*MPI_Comm_size)(IMPL_Comm comm, int *size);
  int (*MPI_Comm_dup)(IMPL_Comm comm, IMPL_Comm *newcomm);
} impl_wrap_handle_t;

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) int impl_wrap_init(impl_wrap_handle_t *handle, const char *mpi_lib);
__attribute__((visibility("default"))) int impl_wrap_finalize(impl_wrap_handle_t *handle);
#ifdef __cplusplus
}
#endif

#endif /* MPI_WRAP_IMPL_H */