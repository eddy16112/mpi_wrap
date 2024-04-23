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

#ifndef MPI_WRAP_H
#define MPI_WRAP_H

#include <stdint.h>

// typedef union {
//   void *p;
//   int i;
//   intptr_t ip;
// } MUK_Handle;

union MUK_Handle{
  void *p;
  int i;
  intptr_t ip;
  MUK_Handle() = default;
  MUK_Handle(intptr_t ip_)
  : ip(ip_)
  {}
};

typedef MUK_Handle MPI_Comm;

#define MPI_COMM_NULL  0
#define MPI_COMM_SELF  1
#define MPI_COMM_WORLD 2

#ifdef __cplusplus
extern "C" {
#endif

extern int MPI_Init(int *argc, char ***argv);
extern int MPI_Finalize(void);

extern int (*MPI_Comm_rank)(MPI_Comm comm, int *rank);
extern int (*MPI_Comm_size)(MPI_Comm comm, int *size);
extern int (*MPI_Comm_dup)(MPI_Comm comm, MPI_Comm *newcomm);

#ifdef __cplusplus
}
#endif


#endif /* MPI_WRAP_H */