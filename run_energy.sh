#!/bin/bash

OUTPUT_FILE="energy_results.txt"
RUNS=10

# Compile the C++ code with optimization flags
echo "Compiling matrix_energy.cpp..."
g++ -O3 -march=native matrix_energy.cpp -o matrix_energy

echo "Starting Energy Measurements ($RUNS runs per config)..." | tee $OUTPUT_FILE
echo "Results will be saved to $OUTPUT_FILE" | tee -a $OUTPUT_FILE

# Loop through Version 1 (Basic) and Version 2 (Line)
for v in 1 2; do
    # Loop through the specific matrix sizes requested
    for s in 1024 2048 3072; do
        echo "================================================" | tee -a $OUTPUT_FILE
        echo " Profiling Version $v | Matrix Size $s" | tee -a $OUTPUT_FILE
        echo "================================================" | tee -a $OUTPUT_FILE
        
        # Run the test multiple times for accuracy
        for ((i=1; i<=RUNS; i++)); do
            echo "--- Run $i ---" | tee -a $OUTPUT_FILE
            
            # perf stat with RAPL events. 
            # 2>&1 redirects the perf output so 'tee' can write it to the file.
            perf stat -e power/energy-pkg/,power/energy-ram/ ./matrix_energy $v $s 2>&1 | tee -a $OUTPUT_FILE
            
            echo "" | tee -a $OUTPUT_FILE
        done
    done
done

echo "All energy measurements complete!" | tee -a $OUTPUT_FILE