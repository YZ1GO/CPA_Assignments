#!/bin/bash

set -e

# Change to project root
cd "$(dirname "$0")/.."

RESULTS_DIR="results"

# --- Function to run a single benchmark ---
run_single_benchmark() {
    local ALGO_ID=$1
    
    mkdir -p build
    mkdir -p "$RESULTS_DIR"

    if [ "$ALGO_ID" -ge 5 ]; then
        echo "[1/2] Building project with SYCL support..."
        
        if ! command -v icpx &> /dev/null; then
            if [ -f "/opt/intel/oneapi/setvars.sh" ]; then
                echo "Auto-sourcing Intel oneAPI variables..."
                source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1
            fi
        fi
        
        make benchmark_sycl
        BENCH_EXEC="./build/benchmark_sycl"
    else
        echo "[1/2] Building project..."
        make benchmark
        BENCH_EXEC="./build/benchmark"
    fi

    echo
    echo "[2/2] Running benchmark for algorithm ID: $ALGO_ID..."
    echo

    TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
    OUTPUT_FILE="$RESULTS_DIR/benchmark_algo_${ALGO_ID}_${TIMESTAMP}.csv"

    stdbuf -oL "$BENCH_EXEC" "$ALGO_ID" | tee "$OUTPUT_FILE"

    echo
    echo "Benchmark completed"
    echo "Results saved to: $OUTPUT_FILE"
    echo "Done."
}

# --- Main Argument Check ---
if [ -z "$1" ]; then
    echo "Usage: ./scripts/run_benchmarks.sh <algorithm_id>"
    echo "Algorithm IDs:"
    echo "  1: Sequential"
    echo "  2: Blocked"
    echo "  3: OpenMP (Loop)"
    echo "  4: OpenMP (Tasks)"
    echo "  5: SYCL (CPU)"
    echo "  6: SYCL (iGPU)"
    echo "  7: All Algorithms"
    exit 1
fi

ALGO_ID=$1

echo "========================================="
echo "Shared Memory LU Benchmark Runner"
echo "========================================="
echo

# Check if RAPL is accessible for energy tracking
RAPL_FILE="/sys/class/powercap/intel-rapl:0/energy_uj"
if [ -f "$RAPL_FILE" ] && [ ! -r "$RAPL_FILE" ]; then
    echo "[WARNING] Cannot read energy counters!"
    echo "To track energy consumption, please cancel this script and run:"
    echo "  sudo chmod -R a+r /sys/class/powercap/intel-rapl"
    echo
    echo "Continuing without energy tracking in 3 seconds..."
    sleep 3
fi

if [ "$ALGO_ID" -eq 7 ]; then
    echo "Option 7 selected. Running ALL benchmarks (1 through 6)..."
    echo "========================================="
    echo
    for id in {1..6}; do
        echo "--> Launching Benchmark for Algorithm $id..."
        run_single_benchmark "$id"
        echo "------------------------------------------"
    done
    echo "All benchmarks completed successfully!"
else
    run_single_benchmark "$ALGO_ID"
fi