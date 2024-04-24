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

#ifdef __cplusplus
union IMPL_Handle {
  void *p;
  int i;
  intptr_t ip;
  IMPL_Handle() = default;
  IMPL_Handle(intptr_t ip_)
    : ip(ip_)
  {}
};
#else
typedef union {
  void *p;
  int i;
  intptr_t ip;
} IMPL_Handle;
#endif

#define IMPL_MPICH 0
#define IMPL_OMPI 1

static inline void *WRAP_DLSYM(void *handle, const char *symbol)
{
  void *fp = dlsym(handle, symbol);
  if(fp == NULL) {
    printf("dlsym: failed to find %s - %s\n", symbol, dlerror());
    fflush(0);
  }
  return fp;
}

#endif /* MPI_WRAP_IMPL_COMMON_H */