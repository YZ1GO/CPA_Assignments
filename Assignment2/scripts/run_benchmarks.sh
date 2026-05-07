#!/bin/bash

set -e

# Change to project root
cd "$(dirname "$0")/.."

RESULTS_DIR="results"
BENCH_EXEC="./build/benchmark"

echo "========================================="
echo "Shared Memory LU Benchmark Runner"
echo "========================================="
echo

mkdir -p build
mkdir -p $RESULTS_DIR

echo "[1/2] Building project..."
make benchmark

echo
echo "[2/2] Running benchmarks..."
echo

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
OUTPUT_FILE="$RESULTS_DIR/benchmark_$TIMESTAMP.csv"

stdbuf -oL "$BENCH_EXEC" | tee "$OUTPUT_FILE"

echo
echo "Benchmark completed"
echo "Results: $OUTPUT_FILE"
echo
echo "Done."