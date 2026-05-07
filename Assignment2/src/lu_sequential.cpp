#include "lu.hpp"
#include <cmath>

void lu_sequential(Matrix& A) {
    int n = A.n;

    for (int k = 0; k < n; ++k) {
        // Compute multipliers for column k
        for (int i = k + 1; i < n; ++i) {
            A(i, k) = A(i, k) / A(k, k);
        }

        // Update trailing submatrix
        for (int i = k + 1; i < n; ++i) {
            for (int j = k + 1; j < n; ++j) {
                A(i, j) = A(i, j) - A(i, k) * A(k, j);
            }
        }
    }
}