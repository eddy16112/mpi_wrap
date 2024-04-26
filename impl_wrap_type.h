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

#ifndef MPI_WRAP_IMPL_COMMON_H
#define MPI_WRAP_IMPL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <dlfcn.h>
#include <stdio.h>

typedef union {
  void *p;
  int i;
  intptr_t ip;
} IMPL_Handle;

typedef struct
{
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
    int __kielletty__[5];
} WRAP_Status;

typedef IMPL_Handle WRAP_Comm;
typedef IMPL_Handle WRAP_Datatype;

// predefined communicators
#define WRAP_COMM_NULL 0
#define WRAP_COMM_SELF 1
#define WRAP_COMM_WORLD 2

// predefined data types
#define WRAP_CHAR 1
#define WRAP_INT 3
#define WRAP_UNSIGNED_LONG_LONG 12
#define WRAP_FLOAT 13
#define WRAP_DOUBLE 14
#define WRAP_INT8_T 18
#define WRAP_INT64_T 21
#define WRAP_UINT8_T 22
#define WRAP_UINT32_T 24
#define WRAP_UINT64_T 25
#define WRAP_BYTE 33

// predefined status
#define WRAP_STATUS_IGNORE  NULL

typedef long int WRAP_Aint;

#define IMPL_MPICH 0
#define IMPL_OMPI 1

enum
{
  // MPICH and OMPI agree on this
  WRAP_THREAD_SINGLE = 0,
  WRAP_THREAD_FUNNELED = 1,
  WRAP_THREAD_SERIALIZED = 2,
  WRAP_THREAD_MULTIPLE = 3
};

// error codes
enum
{
  WRAP_SUCCESS = 0
};

#endif /* MPI_WRAP_IMPL_COMMON_H */