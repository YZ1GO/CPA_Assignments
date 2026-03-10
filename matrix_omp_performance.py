import pandas as pd
import matplotlib.pyplot as plt

# 1. Read the raw benchmark CSV
df = pd.read_csv('benchmark_results.csv')

# 2. Automatically drop the first 2 "warm-up" runs for each setup
df_filtered = df[df['Run'] > 2]

# 3. Calculate the average GFlops dynamically
avg_df = df_filtered.groupby(['Threads', 'SIMD'])['GFlops'].mean().unstack()

# 4. Extract arrays for the plot
threads = avg_df.index.tolist()
non_simd_gflops = avg_df['No'].tolist()
simd_gflops = avg_df['Yes'].tolist()

# 5. Create the plot
plt.figure(figsize=(10, 6))

plt.plot(threads, non_simd_gflops, marker='o', label='Non-SIMD', linewidth=2)
plt.plot(threads, simd_gflops, marker='s', label='Explicit SIMD', linewidth=2)

# Formatting
plt.title('Performance Scaling of Element-Line Multiplication (8192x8192)')
plt.xlabel('Number of Threads')
plt.ylabel('Performance (GFlop/s)')
plt.xticks(threads) 
plt.legend()
plt.grid(True)

# Save the vector graphic for LaTeX
plt.tight_layout()
plt.savefig('performance_scaling_cpp.pdf', format='pdf', bbox_inches='tight')
print("Performance graph saved as performance_scaling_cpp.pdf")