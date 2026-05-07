#pragma once

#include <vector>

class Matrix {
public:
    int n;
    std::vector<double> data;

    Matrix(int size);

    double& operator()(int i, int j);
    const double& operator()(int i, int j) const;

    void randomize();
};