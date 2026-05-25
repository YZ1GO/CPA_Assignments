#include "lu.hpp"
#include <sycl/sycl.hpp>
#include <algorithm>
#include <iostream>

void lu_sycl(Matrix& A, int block_size, sycl::queue& q) {
    int n = A.n;

    // Create a buffer for the matrix data.
    // The buffer automatically manages data transfer between the host and the device.
    sycl::buffer<double, 1> buf_A(A.data.data(), sycl::range<1>(A.data.size()));

    for (int k = 0; k < n; k += block_size) {
        int end_k = std::min(n, k + block_size);

        // -------------------------------------------------------------
        // 1. Factorize the diagonal block (SEQUENTIAL ON DEVICE)
        // We use single_task to avoid moving data back and forth to the CPU
        // -------------------------------------------------------------
        q.submit([&](sycl::handler& cgh) {
            sycl::accessor dev_A(buf_A, cgh, sycl::read_write);
            cgh.single_task([=]() {
                for (int kk = k; kk < end_k; ++kk) {
                    for (int i = kk + 1; i < end_k; ++i) {
                        dev_A[i * n + kk] = dev_A[i * n + kk] / dev_A[kk * n + kk];
                        double L_ikk = dev_A[i * n + kk];
                        for (int j = kk + 1; j < end_k; ++j) {
                            dev_A[i * n + j] = dev_A[i * n + j] - L_ikk * dev_A[kk * n + j];
                        }
                    }
                }
            });
        });

        // -------------------------------------------------------------
        // 2. Compute U panel (SEQUENTIAL ON DEVICE)
        // -------------------------------------------------------------
        q.submit([&](sycl::handler& cgh) {
            sycl::accessor dev_A(buf_A, cgh, sycl::read_write);
            cgh.single_task([=]() {
                for (int kk = k; kk < end_k; ++kk) {
                    for (int i = k; i < kk; ++i) {
                        double L_kki = dev_A[kk * n + i];
                        for (int j = end_k; j < n; ++j) {
                            dev_A[kk * n + j] = dev_A[kk * n + j] - L_kki * dev_A[i * n + j];
                        }
                    }
                }
            });
        });

        // -------------------------------------------------------------
        // 3. Compute L panel (PARALLEL ON DEVICE)
        // -------------------------------------------------------------
        if (end_k < n) {
            q.submit([&](sycl::handler& cgh) {
                sycl::accessor dev_A(buf_A, cgh, sycl::read_write);
                cgh.parallel_for(sycl::range<1>(n - end_k), [=](sycl::id<1> idx) {
                    int i = end_k + idx[0];
                    for (int kk = k; kk < end_k; ++kk) {
                        double sum = 0.0;
                        for (int j = k; j < kk; ++j) {
                            sum += dev_A[i * n + j] * dev_A[j * n + kk];
                        }
                        dev_A[i * n + kk] = (dev_A[i * n + kk] - sum) / dev_A[kk * n + kk];
                    }
                });
            });
        }

        // -------------------------------------------------------------
        // 4. Update trailing submatrix (PARALLEL ON DEVICE)
        // -------------------------------------------------------------
        if (end_k < n) {
            q.submit([&](sycl::handler& cgh) {
                sycl::accessor dev_A(buf_A, cgh, sycl::read_write);
                cgh.parallel_for(sycl::range<1>(n - end_k), [=](sycl::id<1> idx) {
                    int i = end_k + idx[0];
                    for (int kk = k; kk < end_k; ++kk) {
                        double L_ikk = dev_A[i * n + kk];
                        for (int j = end_k; j < n; ++j) {
                            dev_A[i * n + j] = dev_A[i * n + j] - L_ikk * dev_A[kk * n + j];
                        }
                    }
                });
            });
        }
    }

    // Wait for the queue to finish all operations before the buffer is destroyed
    q.wait();
}