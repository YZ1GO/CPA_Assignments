#pragma once

#include "matrix.hpp"

void lu_sequential(Matrix& A);
void lu_blocked(Matrix& A, int block_size);
void lu_openmp(Matrix& A);