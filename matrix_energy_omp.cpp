#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <omp.h>

using namespace std;

void LineMult_OMP(int n, const vector<double>& A, const vector<double>& B, vector<double>& C, int num_threads) {
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                C[i * n + j] += A[i * n + k] * B[k * n + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Check for correct command-line arguments
    if (argc != 3) {
        cout << "Usage: ./matrix_energy_omp <size: 1024|2048|3072> <threads>" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int threads = atoi(argv[2]);

    if (n != 1024 && n != 2048 && n != 3072) {
        cout << "Error: Size must be 1024, 2048, or 3072." << endl;
        return 1;
    }

    // Allocate memory
    vector<double> A(n * n, 1.0);
    vector<double> B(n * n, 2.0);
    vector<double> C(n * n, 0.0);

    // Start timer
    auto start_time = chrono::high_resolution_clock::now();

    // Execute the parallel version
    LineMult_OMP(n, A, B, C, threads);

    // Stop timer
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;

    cout << "Done! Time: " << fixed << setprecision(3) << elapsed.count() << " seconds" << endl;

    return 0;
}