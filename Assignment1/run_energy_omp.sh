#!/bin/bash

OUTPUT_FILE="energy_results_omp.csv"
RUNS=10
THREADS=16
VERSION=3

echo "Compiling matrix_energy_omp.cpp..."
g++ -O3 -fopenmp -march=native matrix_energy_omp.cpp -o matrix_energy_omp

echo "Version,Size,Run,Time_s,Energy_Joules" > $OUTPUT_FILE

echo "Starting OpenMP Energy Measurements ($RUNS runs per config)..."
echo "Results will be saved as CSV to $OUTPUT_FILE"

for s in 1024 2048 3072; do
    echo "================================================"
    echo " Profiling Version $VERSION (OpenMP) | Size $s | Threads $THREADS"
    echo "================================================"
    
    for ((i=1; i<=RUNS; i++)); do
        # Run perf and capture the output
        APP_OUT=$(sudo LC_ALL=C perf stat -x, -e power/energy-pkg/ ./matrix_energy_omp $s $THREADS 2> .perf.tmp)
        
        # Extract Time and Joules
        TIME=$(echo "$APP_OUT" | grep -o "[0-9.]\+ seconds" | awk '{print $1}')
        JOULES=$(grep "power/energy-pkg" .perf.tmp | cut -d, -f1)
        
        # Print summary to terminal
        echo "Run $i: Time = $TIME s | Energy = $JOULES Joules"
        
        # Append data to CSV using the Version 3 label
        echo "$VERSION,$s,$i,$TIME,$JOULES" >> $OUTPUT_FILE
    done
done

# Clean up
rm -f .perf.tmp

echo "All OpenMP energy measurements complete! Check $OUTPUT_FILE"