#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "matrix.hpp"
#include "lu.hpp"
#include "timer.hpp"

int main(int argc, char** argv) {
    std::vector<int> sizes;

    if (argc == 2) {
        // Single size specified
        int n = std::atoi(argv[1]);
        if (n <= 0) {
            std::cerr << "Matrix size must be positive\n";
            return 1;
        }
        sizes.push_back(n);
    } else if (argc == 1) {
        // Run benchmark range: 1024 to 8192, step 1024
        for (int n = 1024; n <= 8192; n += 1024) {
            sizes.push_back(n);
        }
    } else {
        std::cerr << "Usage: " << argv[0] << " [matrix_size]\n";
        std::cerr << "If no size specified, runs benchmark range 1024-8192 step 1024\n";
        return 1;
    }

    std::cout << "LU Factorization Project\n";

    for (int n : sizes) {
        std::cout << "\nMatrix size: " << n << "x" << n << "\n";

        Matrix A_base(n);
        A_base.randomize();

        Matrix A_seq = A_base;
        Matrix A_blk = A_base;

        Timer timer_seq;
        timer_seq.start();
        lu_sequential(A_seq);
        double time_seq = timer_seq.stop();
        std::cout << "Sequential LU completed in  " << time_seq << " seconds\n";

        int block_size = 64;
        Timer timer_blk;
        timer_blk.start();
        lu_blocked(A_blk, block_size);
        double time_blk = timer_blk.stop();
        std::cout << "Blocked LU (" << block_size << ") completed in " << time_blk << " seconds\n";

        std::cout << "Speedup: " << time_seq / time_blk << "x faster\n";
    }

    return 0;
}