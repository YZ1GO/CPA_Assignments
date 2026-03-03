#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <iomanip>
#define MATRIX_SIZE 8192

using namespace std;

void LineMult(int n, const vector<double>& A, const vector<double>& B, vector<double>& C, int num_threads, bool use_simd) {
    fill(C.begin(), C.end(), 0.0);

    omp_set_num_threads(num_threads);

    if (use_simd) {
        // With SIMD
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                #pragma omp simd
                for (int j = 0; j < n; j++) {
                    C[i * n + j] += A[i * n + k] * B[k * n + j];
                }
            }
        }
    } else {
        // Without SIMD
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                for (int j = 0; j < n; j++) {
                    C[i * n + j] += A[i * n + k] * B[k * n + j];
                }
            }
        }
    }
}

int main() {
    cout << "Allocating memory for " << MATRIX_SIZE << "x" << MATRIX_SIZE << " matrices..." << endl;
    
    vector<double> A(MATRIX_SIZE * MATRIX_SIZE, 1.0); 
    vector<double> B(MATRIX_SIZE * MATRIX_SIZE, 2.0);
    vector<double> C(MATRIX_SIZE * MATRIX_SIZE, 0.0);

    vector<int> thread_counts = {4, 8, 12, 16, 20, 24};
    
    // 2n^3
    double total_flops = 2.0 * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE;

    cout << "------------------------------------------------------------" << endl;
    cout << " Threads | SIMD | Time (seconds) | Performance (GFlop/s)" << endl;
    cout << "------------------------------------------------------------" << endl;

    for (int threads : thread_counts) {
        for (bool simd : {false, true}) {
            
            auto start_time = chrono::high_resolution_clock::now();
            
            LineMult(MATRIX_SIZE, A, B, C, threads, simd);
            
            auto end_time = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end_time - start_time;
            
            // GFlop/s: (2n^3 / Execution_Time) / 10^9
            double gflops = (total_flops / elapsed.count()) / 1e9;

            cout << " " << setw(7) << threads 
                << " | " << setw(4) << (simd ? "Yes" : "No") 
                << " | " << setw(14) << fixed << setprecision(3) << elapsed.count() 
                << " | " << setw(21) << fixed << setprecision(2) << gflops << endl;
        }
    }

    return 0;
}