# Shared Memory LU Factorization

## Overview

This project implements and analyzes several versions of the LU factorization algorithm for dense matrices:

- Sequential LU factorization
- Block-oriented sequential LU factorization
- Shared memory parallel LU factorization using OpenMP
- SYCL implementation targeting multicore CPUs and integrated GPUs

The goal is to evaluate performance, scalability, and efficiency across different architectures and programming models.

---

## LU Factorization

LU factorization decomposes a matrix `A` into:

```math
A=LU
``` 

Where:

- `L` is a lower triangular matrix
- `U` is an upper triangular matrix

This decomposition is commonly used to solve systems of linear equations:

```math
Ax=b
```

The implemented algorithm uses an in-place formulation where:

- Elements below the diagonal store `L`
- Elements on and above the diagonal store `U`

The computational complexity is:
```math
\Theta\left(\frac{2}{3}n^3\right)
```

---

## Project Structure

```text
Assignment2/
├── README.md
├── Makefile
├── report/
├── data/
├── include/
├── src/
├── benchmarks/
├── scripts/
├── results/
└── build/
```

### Important Directories

| Directory | Purpose |
|---|---|
| `include/` | Header files |
| `src/` | Core implementations |
| `benchmarks/` | Benchmark executables |
| `scripts/` | Automation and plotting |
| `results/` | Benchmark output data |
| `report/` | Final report and analysis |

---

## Implementations

### 1. Sequential LU

Basic in-place LU factorization without blocking or parallelism.

**Status: Implemented**

Purpose:
- Correctness baseline
- Performance reference

---

### 2. Block-Oriented Sequential LU

Blocked implementation that improves cache locality by operating on submatrices (tiles).

Purpose:
- Reduce cache misses
- Improve memory hierarchy utilization

---

### 3. OpenMP LU

Shared memory parallel implementation using:
- Data parallel loops
- OpenMP tasks

Purpose:
- Exploit multicore CPUs
- Analyze speedup and efficiency

---

### 4. SYCL LU

SYCL implementation capable of running on:
- CPU
- Integrated GPU (iGPU)
- Other SYCL-compatible accelerators

The program explicitly implements targeting for both the CPU (`sycl::cpu_selector_v`) and the iGPU (`sycl::gpu_selector_v`). The user can choose to benchmark them together or isolate them independently via command-line arguments.

---

## Build

### Requirements

- C++17 compiler
- OpenMP support
- SYCL compiler/runtime

Examples:
- GCC
- Clang
- Intel oneAPI DPC++ (Required for `make sycl`)
- AdaptiveCpp

### Intel oneAPI DPC++ Setup (Ubuntu)

To compile the SYCL version, the Intel oneAPI DPC++ compiler (`icpx`) is required.

**Installation commands:**
```bash
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
sudo apt update
sudo apt install intel-oneapi-compiler-dpcpp-cpp
```

> [!NOTE]
> If you encounter a `C++ header location not resolved` error when compiling `make sycl`, it means the Clang-based `icpx` compiler cannot find your system's C++ standard library. You can fix this by installing the dev headers:
> ```bash
> sudo apt install libstdc++-12-dev libstdc++-11-dev
> ```

Before running the compiler, remember to load the environment variables:
```bash
source /opt/intel/oneapi/setvars.sh
```

**Uninstallation commands:**
To remove the compiler and repository after you are done with the assignment:
```bash
sudo apt remove --purge intel-oneapi-compiler-dpcpp-cpp
sudo rm /etc/apt/sources.list.d/oneAPI.list
sudo rm /usr/share/keyrings/oneapi-archive-keyring.gpg
sudo apt update
```

---

### Compile

```bash
make clean && make main  # Compiles Sequential and OpenMP versions
make sycl                # Compiles the SYCL version (requires icpx to be installed and sourced)
```

---

## Run

### Main executable

Runs the LU factorization algorithms on matrices of specified size(s). You can specify which algorithm to run to save time.

> [!IMPORTANT]
> The project compiles into two separate binaries:
> - `./build/lu`: Compiled using standard `g++`. Includes algorithms 1 through 4. It skips SYCL code, meaning it can run on any machine.
> - `./build/lu_sycl`: Compiled using Intel `icpx`. Includes all algorithms (1 through 6). You **must** use this binary if you want to run the SYCL tests.

```bash
./build/lu_sycl [algorithm_id] [matrix_size]
```

**Algorithm IDs:**
- `0`: Run ALL algorithms (Default)
- `1`: Sequential LU
- `2`: Blocked LU
- `3`: OpenMP (Loop)
- `4`: OpenMP (Tasks)
- `5`: SYCL (CPU)
- `6`: SYCL (iGPU)

**Examples:**
```bash
./build/lu              # Run ALL algorithms on the full benchmark range (1024-8192)
./build/lu 0 2048       # Run ALL algorithms on a 2048x2048 matrix
./build/lu 4            # Run ONLY OpenMP Tasks on the full benchmark range
./build/lu_sycl 6 4096  # Run ONLY SYCL iGPU on a 4096x4096 matrix (use ./build/lu_sycl instead of ./build/lu)
```

> [!NOTE]
> When you run a single parallel algorithm (e.g., ID 2-6), its `Speedup` will print as `0.0x` because it skips the sequential baseline to save time. If you use ID `0` to run all algorithms, it correctly calculates all speedups relative to the sequential execution

---

### Benchmarks

The benchmarking infrastructure isolates testing for individual algorithms to prevent massive execution times. For each matrix size, it runs **5 separate iterations** and computes the average execution time to ensure statistically accurate and smooth results. It also automatically tracks RAPL energy consumption.

> [!WARNING]
> Because Linux restricts access to power metrics, you **must** grant read permissions to the energy counters before running the benchmarks, otherwise energy will be reported as `0.0`:
> ```bash
> sudo chmod -R a+r /sys/class/powercap/intel-rapl
> ```

To run the benchmarks, use the automated bash script and provide the algorithm ID:
```bash
bash scripts/run_benchmarks.sh <algorithm_id>
```

**Algorithm IDs:**
- `1`: Sequential LU
- `2`: Blocked LU
- `3`: OpenMP (Loop)
- `4`: OpenMP (Tasks)
- `5`: SYCL (CPU)
- `6`: SYCL (iGPU)
- `7`: **Run ALL Algorithms (1 through 6)**

**Examples:**
```bash
bash scripts/run_benchmarks.sh 1   # Run Sequential benchmark, save to results/
bash scripts/run_benchmarks.sh 6   # Run SYCL iGPU benchmark, save to results/
bash scripts/run_benchmarks.sh 7   # Run ALL benchmarks sequentially, save all CSVs
```

The script will automatically compile the correct target (using `g++` or `icpx`) and save the formatted `size,time,energy` CSV data into the `results/` folder (e.g., `results/benchmark_algo_6_20260528_153000.csv`).

---

## Benchmark Configuration

Matrix sizes tested:

```text
1024 -> 8192
step = 1024
```

Metrics collected:
- Execution time
- Speedup
- Efficiency
- Scalability

---

## Performance Metrics

### Speedup

```math
S(P)=\frac{T(1)}{T(P)}
``` 

Where:
- `T(1)` is sequential execution time
- `T(P)` is parallel execution time using `P` processors

---

### Efficiency

```math
E(P)=\frac{S(P)}{P}
```

---

## Output

Benchmark results are stored as CSV files in:

```text
results/
```

### Generating Plots
Once you have generated the benchmark CSV files, you can automatically calculate Speedup and Efficiency and generate PDF graphs by running:

```bash
python3 scripts/plot_results.py
```
This will output `plot_time.pdf`, `plot_speedup.pdf`, `plot_efficiency.pdf`, and `plot_energy.pdf` directly into the `results/` directory.

---

## Current Status

- [X] Sequential LU
- [X] Blocked LU
- [X] OpenMP LU
- [X] SYCL LU
- [ ] Benchmark automation
- [ ] Plot generation
- [ ] Performance report

---

## Authors

| Name | Student Number |
|---|---|
| Bruno Huang | 202207517 |
| Francisco Fernandes | 202104843 |
| Francisco Mendonça | 202006728 |

Project developed for the advanced parallel computing class.
