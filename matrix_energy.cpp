#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;

// Version 1: Basic Row-Column multiplication (i-j-k)
void Basic_Mult(int n, const vector<double>& A, const vector<double>& B, vector<double>& C) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// Version 2: Element-Line multiplication (i-k-j cache friendly)
void Line_Mult(int n, const vector<double>& A, const vector<double>& B, vector<double>& C) {
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
        cout << "Usage: ./matrix_energy <version: 1|2> <size: 1024|2048|3072>" << endl;
        return 1;
    }

    int version = atoi(argv[1]);
    int n = atoi(argv[2]);

    if ((version != 1 && version != 2) || (n != 1024 && n != 2048 && n != 3072)) {
        cout << "Error: Version must be 1 or 2. Size must be 1024, 2048, or 3072." << endl;
        return 1;
    }

    // Allocate memory
    vector<double> A(n * n, 1.0);
    vector<double> B(n * n, 2.0);
    vector<double> C(n * n, 0.0);

    cout << "Running Version " << version << " | Size: " << n << "x" << n << " ... " << flush;

    // Start timer
    auto start_time = chrono::high_resolution_clock::now();

    // Execute the requested version
    if (version == 1) {
        Basic_Mult(n, A, B, C);
    } else {
        Line_Mult(n, A, B, C);
    }

    // Stop timer
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;

    cout << "Done! Time: " << fixed << setprecision(3) << elapsed.count() << " seconds" << endl;

    return 0;
}