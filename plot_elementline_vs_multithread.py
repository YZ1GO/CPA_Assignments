import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load data (Excluding first 2 runs)
df_energy = pd.read_csv('energy_results.csv')
df_omp = pd.read_csv('energy_results_omp.csv')

df_energy = df_energy[df_energy['Run'] >= 3]
df_omp = df_omp[df_omp['Run'] >= 3]

# Filter for Element-line Multithread
v2_time = df_energy[df_energy['Version'] == 2].groupby('Size')['Time_s'].mean()
v3_time = df_omp[df_omp['Version'] == 3].groupby('Size')['Time_s'].mean()

v2_energy = df_energy[df_energy['Version'] == 2].groupby('Size')['Energy_Joules'].mean()
v3_energy = df_omp[df_omp['Version'] == 3].groupby('Size')['Energy_Joules'].mean()

# Figure with 2 subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

sizes = v2_time.index.astype(str)
x = np.arange(len(sizes))
width = 0.35

# Plot 1 - Time comparison
bars1 = ax1.bar(x - width/2, v2_time.values, width, label='Element-line ($V_2$)', alpha=0.8)
bars2 = ax1.bar(x + width/2, v3_time.values, width, label='Multithread ($V_3$)', alpha=0.8)
ax1.set_xlabel('Problem Size', fontsize=12)
ax1.set_ylabel('Time (seconds)', fontsize=12)
ax1.set_title('Element-line vs Multithread: Execution Time', fontsize=12, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(sizes)
ax1.legend()
ax1.grid(axis='y', alpha=0.3)
ax1.bar_label(bars1, fmt='%.3f', padding=3)
ax1.bar_label(bars2, fmt='%.3f', padding=3)

# Plot 2 - Energy comparison
bars3 = ax2.bar(x - width/2, v2_energy.values, width, label='Element-line ($V_2$)', alpha=0.8)
bars4 = ax2.bar(x + width/2, v3_energy.values, width, label='Multithread ($V_3$)', alpha=0.8)
ax2.set_xlabel('Problem Size', fontsize=12)
ax2.set_ylabel('Energy (Joules)', fontsize=12)
ax2.set_title('Element-line vs Multithread: Energy Consumption', fontsize=12, fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(sizes)
ax2.legend()
ax2.grid(axis='y', alpha=0.3)
ax2.bar_label(bars3, fmt='%.2f', padding=3)
ax2.bar_label(bars4, fmt='%.2f', padding=3)

plt.tight_layout()
# Changed extension to .pdf and removed dpi parameter for vector format
plt.savefig('comparison_elementline_vs_multithread.pdf')