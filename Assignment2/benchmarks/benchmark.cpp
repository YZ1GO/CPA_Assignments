#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>

#include "matrix.hpp"
#include "lu.hpp"
#include "timer.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <algorithm_id>\n";
        std::cerr << "Algorithm IDs:\n";
        std::cerr << "  1: Sequential\n";
        std::cerr << "  2: Blocked\n";
        std::cerr << "  3: OpenMP (Loop)\n";
        std::cerr << "  4: OpenMP (Tasks)\n";
        std::cerr << "  5: SYCL (CPU)\n";
        std::cerr << "  6: SYCL (iGPU)\n";
        return 1;
    }

    int algo_choice = std::atoi(argv[1]);
    int block_size = 64;

    std::vector<int> sizes;
    for (int n = 1024; n <= 8192; n += 1024) {
        sizes.push_back(n);
    }

    // Print CSV header and enable immediate flushing
    std::cout << "size,time\n";
    std::cout << std::unitbuf;

    for (int n : sizes) {
        double total_time = 0.0;
        bool success = true;
        int num_iterations = 5;

        for (int iter = 0; iter < num_iterations; ++iter) {
            Matrix A(n);
            A.randomize();
            double time = 0.0;
            Timer timer;

            switch(algo_choice) {
                case 1:
                    timer.start();
                    lu_sequential(A);
                    time = timer.stop();
                    break;
                case 2:
                    timer.start();
                    lu_blocked(A, block_size);
                    time = timer.stop();
                    break;
                case 3:
                    timer.start();
                    lu_openmp_loop(A, block_size);
                    time = timer.stop();
                    break;
                case 4:
                    timer.start();
                    lu_openmp_task(A, block_size);
                    time = timer.stop();
                    break;
                case 5:
#ifdef USE_SYCL
                    try {
                        sycl::queue q_cpu{sycl::cpu_selector_v};
                        timer.start();
                        lu_sycl(A, block_size, q_cpu);
                        time = timer.stop();
                    } catch (...) {
                        success = false;
                    }
#else
                    success = false;
#endif
                    break;
                case 6:
#ifdef USE_SYCL
                    try {
                        sycl::queue q_gpu{sycl::gpu_selector_v};
                        timer.start();
                        lu_sycl(A, block_size, q_gpu);
                        time = timer.stop();
                    } catch (...) {
                        success = false;
                    }
#else
                    success = false;
#endif
                    break;
                default:
                    std::cerr << "Invalid algorithm ID\n";
                    return 1;
            }

            if (!success) break; // Don't run remaining iterations if it failed
            total_time += time;
        }

        if (success) {
            std::cout << n << "," << std::fixed << std::setprecision(6) << (total_time / num_iterations) << "\n";
        } else {
            std::cout << n << ",0.0\n";
        }
    }

    return 0;
}