#pragma once

#include <vector>

typedef float real_t;

class Matrix {
public:
    int n;
    std::vector<real_t> data;

    Matrix(int size);

    real_t& operator()(int i, int j);
    const real_t& operator()(int i, int j) const;

    void randomize();
};