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
    int algo_choice = 0; // 0 means ALL

    if (argc >= 2) {
        algo_choice = std::atoi(argv[1]);
    }
    
    if (argc >= 3) {
        int n = std::atoi(argv[2]);
        if (n <= 0) {
            std::cerr << "Matrix size must be positive\n";
            return 1;
        }
        sizes.push_back(n);
    } else {
        // Run benchmark range: 1024 to 8192, step 1024
        for (int n = 1024; n <= 8192; n += 1024) {
            sizes.push_back(n);
        }
    }

    if (argc > 3 || algo_choice < 0 || algo_choice > 6) {
        std::cerr << "Usage: " << argv[0] << " [algorithm_id] [matrix_size]\n";
        std::cerr << "algorithm_id options:\n";
        std::cerr << "  0 : Run ALL algorithms (default)\n";
        std::cerr << "  1 : Sequential LU\n";
        std::cerr << "  2 : Blocked LU\n";
        std::cerr << "  3 : OpenMP (Loop)\n";
        std::cerr << "  4 : OpenMP (Tasks)\n";
        std::cerr << "  5 : SYCL (CPU)\n";
        std::cerr << "  6 : SYCL (iGPU)\n";
        std::cerr << "If matrix_size is omitted, runs benchmark range 1024-8192 step 1024\n";
        return 1;
    }

    std::cout << "LU Factorization Project\n";

    for (int n : sizes) {
        std::cout << "\nMatrix size: " << n << "x" << n << "\n";

        Matrix A_base(n);
        A_base.randomize();
        int block_size = 64;
        double time_seq = 0.0;

        // We run sequential baseline if algo 1 is selected, or if ALL is selected
        // (we need time_seq to calculate speedups for the others if ALL is selected)
        if (algo_choice == 0 || algo_choice == 1) {
            Matrix A_seq = A_base;
            Timer timer_seq;
            timer_seq.start();
            lu_sequential(A_seq);
            time_seq = timer_seq.stop();
            std::cout << "[1] Sequential LU:  " << std::fixed << std::setprecision(4) << time_seq << " s\n";
        } else {
            time_seq = 1.0; // Dummy value if sequential is skipped but we need a speedup divisor
        }

        if (algo_choice == 0 || algo_choice == 2) {
            Matrix A_blk = A_base;
            Timer timer_blk;
            timer_blk.start();
            lu_blocked(A_blk, block_size);
            double time_blk = timer_blk.stop();
            std::cout << "[2] Blocked LU:     " << time_blk << " s  | Speedup: " << (algo_choice == 0 ? time_seq / time_blk : 0) << "x\n";
        }

        if (algo_choice == 0 || algo_choice == 3) {
            Matrix A_omp_loop = A_base;
            Timer timer_omp_loop;
            timer_omp_loop.start();
            lu_openmp_loop(A_omp_loop, block_size);
            double time_omp_loop = timer_omp_loop.stop();
            std::cout << "[3] OpenMP (Loop):  " << time_omp_loop << " s  | Speedup: " << (algo_choice == 0 ? time_seq / time_omp_loop : 0) << "x\n";
        }

        if (algo_choice == 0 || algo_choice == 4) {
            Matrix A_omp_task = A_base;
            Timer timer_omp_task;
            timer_omp_task.start();
            lu_openmp_task(A_omp_task, block_size);
            double time_omp_task = timer_omp_task.stop();
            std::cout << "[4] OpenMP (Tasks): " << time_omp_task << " s  | Speedup: " << (algo_choice == 0 ? time_seq / time_omp_task : 0) << "x\n";
        }

        #ifdef USE_SYCL
        if (algo_choice == 0 || algo_choice == 5) {
            try {
                sycl::queue q_cpu{sycl::cpu_selector_v};
                Matrix A_sycl_cpu = A_base;
                Timer timer_sycl_cpu;
                timer_sycl_cpu.start();
                lu_sycl(A_sycl_cpu, block_size, q_cpu);
                double time_sycl_cpu = timer_sycl_cpu.stop();
                std::cout << "[5] SYCL (CPU):     " << time_sycl_cpu << " s  | Speedup: " << (algo_choice == 0 ? time_seq / time_sycl_cpu : 0) << "x\n";
            } catch (sycl::exception const& e) {
                std::cout << "[5] SYCL (CPU):     Not available (" << e.what() << ")\n";
            }
        }

        if (algo_choice == 0 || algo_choice == 6) {
            try {
                sycl::queue q_gpu{sycl::gpu_selector_v};
                Matrix A_sycl_gpu = A_base;
                Timer timer_sycl_gpu;
                timer_sycl_gpu.start();
                lu_sycl(A_sycl_gpu, block_size, q_gpu);
                double time_sycl_gpu = timer_sycl_gpu.stop();
                std::cout << "[6] SYCL (iGPU):    " << time_sycl_gpu << " s  | Speedup: " << (algo_choice == 0 ? time_seq / time_sycl_gpu : 0) << "x\n";
            } catch (sycl::exception const& e) {
                std::cout << "[6] SYCL (iGPU):    Not available (" << e.what() << ")\n";
            }
        }
        #endif
    }

    return 0;
}