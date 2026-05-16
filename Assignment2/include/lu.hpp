#pragma once

#include "matrix.hpp"

void lu_sequential(Matrix& A);
void lu_blocked(Matrix& A, int block_size);
void lu_openmp_loop(Matrix& A, int block_size); // Data Parallel
void lu_openmp_task(Matrix& A, int block_size); // Tasks
bool lu_sycl_available();
void init_sycl_device();  // Initialize SYCL device selection (call before timing)
void lu_sycl(Matrix& A);