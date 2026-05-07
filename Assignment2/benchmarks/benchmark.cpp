#include <iostream>
#include <vector>
#include <iomanip>

#include "matrix.hpp"
#include "lu.hpp"
#include "timer.hpp"

int main() {
    std::vector<int> sizes;
    for (int n = 1024; n <= 8192; n += 1024) {
        sizes.push_back(n);
    }

    // Print CSV header and enable immediate flushing
    std::cout << "size,time\n";
    std::cout << std::unitbuf;

    for (int n : sizes) {
        Matrix A(n);
        A.randomize();

        Timer timer;
        timer.start();
        lu_sequential(A);
        double time = timer.stop();

        std::cout << n << "," << std::fixed << std::setprecision(6) << time << "\n";
    }

    return 0;
}