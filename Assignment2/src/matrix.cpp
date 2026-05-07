#include "matrix.hpp"

#include <random>

Matrix::Matrix(int size)
    : n(size), data(size * size, 0.0) {
}

double& Matrix::operator()(int i, int j) {
    return data[i * n + j];
}

const double& Matrix::operator()(int i, int j) const {
    return data[i * n + j];
}

void Matrix::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            (*this)(i, j) = dis(gen);
        }
        // Make diagonally dominant to ensure non-singularity
        double sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (j != i) sum += std::abs((*this)(i, j));
        }
        (*this)(i, i) = sum + 1.0; // Ensure diagonal element is larger than sum of others
    }
}