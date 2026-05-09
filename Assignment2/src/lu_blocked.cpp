#include "lu.hpp"
#include <algorithm>
#include <stdexcept>

void lu_blocked(Matrix& A, int block_size) {
    if (block_size <= 0) {
        throw std::invalid_argument("block_size must be positive");
    }

    int n = A.n;

    for (int k = 0; k < n; k += block_size) {
        // Ensure we don't go out of bounds on the last block
        int end_k = std::min(n, k + block_size);

        // Factorize the diagonal block A[k:end_k, k:end_k]
        for (int kk = k; kk < end_k; ++kk) {
            for (int i = kk + 1; i < end_k; ++i) {
                A(i, kk) = A(i, kk) / A(kk, kk);
                double L_ikk = A(i, kk);
                for (int j = kk + 1; j < end_k; ++j) {
                    A(i, j) = A(i, j) - L_ikk * A(kk, j);
                }
            }
        }

        // Compute U panel (Upper triangular part)
        for (int kk = k; kk < end_k; ++kk) {
            for (int i = k; i < kk; ++i) {
                double L_kki = A(kk, i);
                for (int j = end_k; j < n; ++j) {
                    A(kk, j) = A(kk, j) - L_kki * A(i, j);
                }
            }
        }

        // Compute L panel (Lower triangular part) 
        for (int i = end_k; i < n; ++i) {
            for (int kk = k; kk < end_k; ++kk) {
                double sum = 0.0;
                for (int j = k; j < kk; ++j) {
                    sum += A(i, j) * A(j, kk);
                }
                A(i, kk) = (A(i, kk) - sum) / A(kk, kk);
            }
        }

        // Update trailing submatrix A[end_k:n, end_k:n]
        for (int i = end_k; i < n; ++i) {
            for (int kk = k; kk < end_k; ++kk) {
                double L_ikk = A(i, kk);
                for (int j = end_k; j < n; ++j) {
                    A(i, j) = A(i, j) - L_ikk * A(kk, j);
                }
            }
        }
    }

}