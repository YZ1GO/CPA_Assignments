import pandas as pd
import matplotlib.pyplot as plt

# 1. Read the CSV
df = pd.read_csv('benchmark_results.csv')

# 2. Filter out warm-ups
df_filtered = df[df['Run'] > 2]

# 3. Calculate average Time_s grouped by Threads and SIMD
avg_time_df = df_filtered.groupby(['Threads', 'SIMD'])['Time_s'].mean().unstack()

# 4. Extract data and calculate TRUE Speedup (using T_1 as the baseline)
threads = avg_time_df.index.tolist()

# Baseline times (1 thread)
t1_no_simd = avg_time_df['No'].loc[1]
t1_simd = avg_time_df['Yes'].loc[1]

# Calculate Speedup arrays
speedup_no_simd = [t1_no_simd / t for t in avg_time_df['No'].tolist()]
speedup_simd = [t1_simd / t for t in avg_time_df['Yes'].tolist()]

# Create the professional plot
plt.figure(figsize=(10, 6))

plt.plot(threads, speedup_no_simd, marker='o', label='Non-SIMD (-O3 Auto)', linewidth=2, color='#1f77b4')
plt.plot(threads, speedup_simd, marker='s', label='Explicit SIMD (#pragma)', linewidth=2, color='#ff7f0e')

# Add a dotted line representing "Ideal" absolute scaling
ideal_scaling = [t for t in threads] 
plt.plot(threads, ideal_scaling, linestyle='--', color='gray', label='Ideal Scaling', alpha=0.7)

# --- Add value labels to the data points ---
for i in range(len(threads)):
    # Place Non-SIMD values slightly above the dot
    plt.annotate(f"{speedup_no_simd[i]:.1f}x", (threads[i], speedup_no_simd[i]), 
                textcoords="offset points", xytext=(0, 8), ha='center', fontsize=9, color='#1f77b4', fontweight='bold')
    
    # Place Explicit SIMD values slightly below the dot
    plt.annotate(f"{speedup_simd[i]:.1f}x", (threads[i], speedup_simd[i]), 
                textcoords="offset points", xytext=(0, -14), ha='center', fontsize=9, color='#ff7f0e', fontweight='bold')

# Formatting
plt.title('Absolute Speedup of Element-Line Multiplication (Base = 1 Thread)')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup (T1 / Tn)')
plt.xticks(threads)
plt.legend()
plt.grid(True)

# Save the vector graphic for LaTeX
plt.tight_layout()
plt.savefig('speedup_scaling_cpp.pdf', format='pdf', bbox_inches='tight')
print("Speedup graph saved as speedup_scaling_cpp.pdf")