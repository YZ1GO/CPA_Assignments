#include "lu.hpp"

#include <omp.h>

void lu_openmp(Matrix& A) {

    int n = A.n;

    // TODO:
    // Parallel LU with OpenMP
    // Option 1:
    //   pragma omp parallel for
    // Option 2:
    //   OpenMP tasks

}