#pragma once

#include "matrix.hpp"

void lu_sequential(Matrix& A);
void lu_blocked(Matrix& A, int block_size);
void lu_openmp_loop(Matrix& A, int block_size); // Data Parallel
void lu_openmp_task(Matrix& A, int block_size); // Tasks
#ifdef USE_SYCL
#include <sycl/sycl.hpp>
void lu_sycl(Matrix& A, int block_size, sycl::queue& q);
#endif