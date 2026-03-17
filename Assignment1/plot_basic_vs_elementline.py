import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load data (Excluding first 2 runs)
df = pd.read_csv('energy_results.csv')
df = df[df['Run'] >= 3]

# Filter for Basic and Element-line
v1_time = df[df['Version'] == 1].groupby('Size')['Time_s'].mean()
v2_time = df[df['Version'] == 2].groupby('Size')['Time_s'].mean()

v1_energy = df[df['Version'] == 1].groupby('Size')['Energy_Joules'].mean()
v2_energy = df[df['Version'] == 2].groupby('Size')['Energy_Joules'].mean()

# Create figure with 2 subplots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))

sizes = v1_time.index.astype(str)
x = np.arange(len(sizes))
width = 0.35

# Plot 1 - Time comparison
bars1 = ax1.bar(x - width/2, v1_time.values, width, label='Basic (V1)', alpha=0.8)
bars2 = ax1.bar(x + width/2, v2_time.values, width, label='Element-line (V2)', alpha=0.8)
ax1.set_xlabel('Problem Size', fontsize=12)
ax1.set_ylabel('Time (seconds)', fontsize=12)
ax1.set_title('Basic vs Element-line: Execution Time', fontsize=12, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(sizes)
ax1.legend()
ax1.grid(axis='y', alpha=0.3)
ax1.bar_label(bars1, fmt='%.3f', padding=3)
ax1.bar_label(bars2, fmt='%.3f', padding=3)

# Plot 2 - Energy comparison
bars3 = ax2.bar(x - width/2, v1_energy.values, width, label='Basic (V1)', alpha=0.8)
bars4 = ax2.bar(x + width/2, v2_energy.values, width, label='Element-line (V2)', alpha=0.8)
ax2.set_xlabel('Problem Size', fontsize=12)
ax2.set_ylabel('Energy (Joules)', fontsize=12)
ax2.set_title('Basic vs Element-line: Energy Consumption', fontsize=12, fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(sizes)
ax2.legend()
ax2.grid(axis='y', alpha=0.3)
ax2.bar_label(bars3, fmt='%.2f', padding=3)
ax2.bar_label(bars4, fmt='%.2f', padding=3)

plt.tight_layout()
plt.savefig('comparison_basic_vs_elementline.pdf')