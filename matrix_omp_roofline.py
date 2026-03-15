import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# ---------------------------------------------------------
# 1. Dynamically parse hardware limits from your text files
# ---------------------------------------------------------
peak_compute = 0.0
peak_mem_bandwidth = 0.0

# Parse CPU Frequency
try:
    with open('feup_lscpu.txt', 'r') as f:
        for line in f:
            if "CPU max MHz:" in line:
                mhz_str = line.split(':')[1].strip()
                max_ghz = float(mhz_str) / 1000.0
                # 8 P-Cores * GHz * 16 FLOPs/cycle (AVX2 Double Precision)
                peak_compute = 8 * max_ghz * 16
                break
except FileNotFoundError:
    print("Error: feup_lscpu.txt not found. Using defaults.")
    peak_compute = 665.6

# Parse STREAM Memory Bandwidth
try:
    with open('feup_stream_benchmark.txt', 'r') as f:
        for line in f:
            if line.startswith("Triad:"):
                mb_s = float(line.split()[1])
                peak_mem_bandwidth = mb_s / 1000.0
                break
except FileNotFoundError:
    print("Error: feup_stream_benchmark.txt not found. Using defaults.")
    peak_mem_bandwidth = 29.31

print(f"--> Extracted Peak Compute: {peak_compute:.1f} GFlop/s")
print(f"--> Extracted Memory Bandwidth: {peak_mem_bandwidth:.2f} GB/s")

# ---------------------------------------------------------
# 2. Dynamically get experimental max from benchmark CSV
# ---------------------------------------------------------
try:
    df = pd.read_csv('benchmark_results.csv')
    df_filtered = df[df['Run'] > 2] # Drop warm-ups
    experimental_perf = df_filtered['GFlops'].max() # Find the absolute best run
except FileNotFoundError:
    print("Error: benchmark_results.csv not found. Using dummy data.")
    experimental_perf = 29.84
    
experimental_oi = 0.25 # Mathematically calculated for untiled i-k-j

print(f"--> Max Achieved Performance: {experimental_perf:.2f} GFlop/s")

# ---------------------------------------------------------
# 3. Generate Roofline Data
# ---------------------------------------------------------
oi_axis = np.logspace(-2, 3, 500)
memory_roof = oi_axis * peak_mem_bandwidth
compute_roof = np.full_like(oi_axis, peak_compute)
actual_roofline = np.minimum(memory_roof, compute_roof)

# ---------------------------------------------------------
# 4. Create the plot
# ---------------------------------------------------------
plt.figure(figsize=(10, 6))

# Plot the empirical hardware roofline
plt.plot(oi_axis, actual_roofline, color='red', linewidth=2, label='Empirical Hardware Limit')

# --- Label the specific Hardware Peak Limits on the graph ---
plt.text(20, peak_compute * 1.2, f'Peak Compute: {peak_compute:.1f} GFlop/s', color='red', fontsize=10, fontweight='bold')

x_pos = 0.1
y_pos = x_pos * peak_mem_bandwidth
plt.text(x_pos, y_pos * 1.3, f'Mem Bandwidth: {peak_mem_bandwidth:.2f} GB/s', color='red', fontsize=10, fontweight='bold')

# Plot your specific matrix multiplication test result
plt.plot(experimental_oi, experimental_perf, marker='o', markersize=10, color='#1f77b4', label='Actual Performance (Peak)')

# Add text label next to the point
plt.text(experimental_oi * 1.1, experimental_perf * 0.8, f' {experimental_perf:.2f} GFlop/s\n OI: {experimental_oi}', color='#1f77b4', fontweight='bold')

# Log-Log Formatting
plt.xscale('log', base=10)
plt.yscale('log', base=10)

# Titles and Labels
plt.title('Empirical Roofline Model: Element-Line Multiplication (FEUP i7-14700T)')
plt.xlabel('Operational Intensity (FLOPs/Byte)')
plt.ylabel('Performance (GFlop/s)')

plt.xlim(0.01, 1000)
plt.ylim(1, 2000)

plt.grid(True, which="both", ls="--", alpha=0.5)
plt.legend(loc='upper left')

# Add shaded regions for ridge point
ridge_point = peak_compute / peak_mem_bandwidth
plt.axvline(x=ridge_point, color='gray', linestyle=':', alpha=0.7)
plt.text(ridge_point * 1.1, 2, 'Compute-Bound Region \u2192', color='gray', fontsize=10)
plt.text(ridge_point * 0.9, 2, '\u2190 Memory-Bound Region', color='gray', fontsize=10, horizontalalignment='right')

# Save vector graphic
plt.tight_layout()
plt.savefig('roofline_model_cpp.pdf', format='pdf', bbox_inches='tight')
print("--> Roofline graph saved successfully as roofline_model_cpp.pdf!")