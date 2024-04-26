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

#ifndef MPI_TEST_COMMON_H
#define MPI_TEST_COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>

#ifdef USE_REAL_MPI
#include <mpi.h>
#else
#include "mpi_wrap.h"
#endif

template <typename T>
T *init_array(size_t size, T value)
{
  T *buffer = (T *)malloc(sizeof(T) * size);
  for(size_t i = 0; i < size; i++) {
    buffer[i] = i + value;
  }
  return buffer;
}

template <typename T>
void check_array(size_t size, T *buffer, T expected)
{
  for(size_t i = 0; i < size; i++) {
    if(buffer[i] != expected + (T)i) {
      std::cout << "wrong i " << i << ", expected " << expected + (T)i << ", actual " << buffer[i] << std::endl << std::flush;
      assert(0);
    }
  }
}

template <typename T>
T *init_array_const(size_t size, T value)
{
  T *buffer = (T *)malloc(sizeof(T) * size);
  for(size_t i = 0; i < size; i++) {
    buffer[i] = value;
  }
  return buffer;
}

template <typename T>
void check_array_const(size_t size, T *buffer, T expected)
{
  for(size_t i = 0; i < size; i++) {
    if(buffer[i] != expected) {
      std::cout << "wrong i " << i << ", expected " << expected << ", actual " << buffer[i] << std::endl << std::flush;
      assert(0);
    }
  }
}

template <typename T>
void update_array(size_t size, T *buffer)
{
  for(size_t i = 0; i < size; i++) {
    buffer[i] += 1;
  }
}

#endif /* MPI_TEST_COMMON_H */