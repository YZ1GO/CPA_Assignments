#!/bin/bash

OUTPUT_FILE="energy_results.csv"
RUNS=10

# Compile the C++ code with optimization flags
echo "Compiling matrix_energy.cpp..."
g++ -O3 -march=native matrix_energy.cpp -o matrix_energy

# Create the file and write the CSV header
echo "Version,Size,Run,Time_s,Energy_Joules" > $OUTPUT_FILE

echo "Starting Energy Measurements ($RUNS runs per config)..."
echo "Results will be saved as CSV to $OUTPUT_FILE"

# Loop through Version 1 (Basic) and Version 2 (Line)
for v in 1 2; do
    # Loop through the specific matrix sizes requested
    for s in 1024 2048 3072; do
        echo "================================================"
        echo " Profiling Version $v | Matrix Size $s"
        echo "================================================"
        
        # Run the test multiple times for accuracy
        for ((i=1; i<=RUNS; i++)); do
            # 1. LC_ALL=C forces dots instead of commas for decimals.
            # 2. -x, tells perf to output raw CSV format (e.g., "98.08,Joules...")
            # 3. We save perf's output to a hidden temporary file to keep the terminal clean.
            APP_OUT=$(sudo LC_ALL=C perf stat -x, -e power/energy-pkg/ ./matrix_energy $v $s 2> .perf.tmp)
            
            # Extract just the numerical Time from the C++ output
            TIME=$(echo "$APP_OUT" | grep -o "[0-9.]\+ seconds" | awk '{print $1}')
            
            # Extract just the numerical Joules from the perf temporary file
            JOULES=$(grep "power/energy-pkg" .perf.tmp | cut -d, -f1)
            
            # Print a clean, readable summary to your terminal
            echo "Run $i: Time = $TIME s | Energy = $JOULES Joules"
            
            # Append the raw data directly to your CSV file
            echo "$v,$s,$i,$TIME,$JOULES" >> $OUTPUT_FILE
        done
    done
done

# Clean up the temporary file
rm -f .perf.tmp

echo "All energy measurements complete! Check $OUTPUT_FILE"