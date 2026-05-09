#include "lu.hpp"
#include <omp.h>
#include <algorithm>
#include <stdexcept>

void lu_openmp_task(Matrix& A, int block_size) {
    if (block_size <= 0) {
        throw std::invalid_argument("block_size must be positive");
    }

    int n = A.n;

    // Create the Thread Pool
    #pragma omp parallel
    // Assign ONE master thread to generate all the tasks
    #pragma omp single
    {
        for (int k = 0; k < n; k += block_size) {
            int end_k = std::min(n, k + block_size);

            // Factorize Diagonal Block
            // depend(inout: ...) means "I need exclusive access to this block"
            #pragma omp task depend(inout: A(k, k))
            {
                for (int kk = k; kk < end_k; ++kk) {
                    for (int i = kk + 1; i < end_k; ++i) {
                        A(i, kk) = A(i, kk) / A(kk, kk);
                        double L_ikk = A(i, kk);
                        for (int j = kk + 1; j < end_k; ++j) {
                            A(i, j) = A(i, j) - L_ikk * A(kk, j);
                        }
                    }
                }
            }

            // Compute U Panel (Block by Block)
            for (int j = end_k; j < n; j += block_size) {
                int end_j = std::min(n, j + block_size);
                
                // depend(in: A(k,k))   -> "I must wait for the diagonal block to finish"
                // depend(inout: A(k,j)) -> "I am updating this specific U block"
                #pragma omp task depend(in: A(k, k)) depend(inout: A(k, j))
                {
                    for (int kk = k; kk < end_k; ++kk) {
                        for (int i = k; i < kk; ++i) {
                            double L_kki = A(kk, i);
                            for (int jj = j; jj < end_j; ++jj) {
                                A(kk, jj) = A(kk, jj) - L_kki * A(i, jj);
                            }
                        }
                    }
                }
            }

            // Compute L Panel (Block by Block)
            for (int i = end_k; i < n; i += block_size) {
                int end_i = std::min(n, i + block_size);
                
                // depend(in: A(k,k))   -> "I must wait for the diagonal block to finish"
                // depend(inout: A(i,k)) -> "I am updating this specific L block"
                #pragma omp task depend(in: A(k, k)) depend(inout: A(i, k))
                {
                    for (int ii = i; ii < end_i; ++ii) {
                        for (int kk = k; kk < end_k; ++kk) {
                            double sum = 0.0;
                            for (int jj = k; jj < kk; ++jj) {
                                sum += A(ii, jj) * A(jj, kk);
                            }
                            A(ii, kk) = (A(ii, kk) - sum) / A(kk, kk);
                        }
                    }
                }
            }

            // Update Trailing Submatrix (Block by Block)
            for (int i = end_k; i < n; i += block_size) {
                int end_i = std::min(n, i + block_size);
                for (int j = end_k; j < n; j += block_size) {
                    int end_j = std::min(n, j + block_size);
                    
                    // depend(in: A(i,k), A(k,j)) -> "I need my specific L block AND my specific U block"
                    // depend(inout: A(i,j))      -> "I am updating this specific trailing block"
                    #pragma omp task depend(in: A(i, k), A(k, j)) depend(inout: A(i, j))
                    {
                        for (int ii = i; ii < end_i; ++ii) {
                            for (int kk = k; kk < end_k; ++kk) {
                                double L_ikk = A(ii, kk);
                                for (int jj = j; jj < end_j; ++jj) {
                                    A(ii, jj) = A(ii, jj) - L_ikk * A(kk, jj);
                                }
                            }
                        }
                    }
                }
            }
        } // End of outer 'k' loop

        // ==========================================
        // TIME SYNC ANCHOR
        // ==========================================
        #pragma omp taskwait
    } // End of parallel region
}