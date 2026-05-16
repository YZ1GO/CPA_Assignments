#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <iomanip>

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

    // Initialize SYCL device selection before timing starts
    if (lu_sycl_available()) {
        init_sycl_device();
    }

    for (int n : sizes) {
        std::cout << "\nMatrix size: " << n << "x" << n << "\n";

        Matrix A_base(n);
        A_base.randomize();

        Matrix A_seq = A_base;
        Matrix A_blk = A_base;
        Matrix A_omp_loop = A_base;
        Matrix A_omp_task = A_base;
        Matrix A_sycl = A_base;

        int block_size = 64;

        Timer timer_seq;
        timer_seq.start();
        lu_sequential(A_seq);
        double time_seq = timer_seq.stop();
        std::cout << "[1] Sequential LU:  " << std::fixed << std::setprecision(4) << time_seq << " s\n";

        Timer timer_blk;
        timer_blk.start();
        lu_blocked(A_blk, block_size);
        double time_blk = timer_blk.stop();
        std::cout << "[2] Blocked LU:     " << time_blk << " s  | Speedup: " << time_seq / time_blk << "x\n";

        Timer timer_omp_loop;
        timer_omp_loop.start();
        lu_openmp_loop(A_omp_loop, block_size);
        double time_omp_loop = timer_omp_loop.stop();
        std::cout << "[3] OpenMP (Loop):  " << time_omp_loop << " s  | Speedup: " << time_seq / time_omp_loop << "x\n";

        Timer timer_omp_task;
        timer_omp_task.start();
        lu_openmp_task(A_omp_task, block_size);
        double time_omp_task = timer_omp_task.stop();
        std::cout << "[4] OpenMP (Tasks): " << time_omp_task << " s  | Speedup: " << time_seq / time_omp_task << "x\n";

        if (lu_sycl_available()) {
            Timer timer_sycl;
            timer_sycl.start();
            lu_sycl(A_sycl);
            double time_sycl = timer_sycl.stop();
            std::cout << "[5] SYCL:           " << time_sycl << " s  | Speedup: " << time_seq / time_sycl << "x\n";
        } else {
            std::cout << "[5] SYCL:           unavailable in this build\n";
        }
    }

    return 0;
}