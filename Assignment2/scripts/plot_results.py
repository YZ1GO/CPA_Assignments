#!/usr/bin/env python3

import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)
RESULTS_DIR = os.path.join(PROJECT_ROOT, 'results')

NUM_CORES = 6

# Mapping algorithm IDs to display names
ALGO_NAMES = {
    1: 'Sequential',
    2: 'Blocked',
    3: 'OpenMP (Loop)',
    4: 'OpenMP (Tasks)',
    5: 'SYCL (CPU)',
    6: 'SYCL (iGPU)'
}

def get_latest_csv(algo_id):
    """Finds the most recent CSV file for a given algorithm ID."""
    pattern = os.path.join(RESULTS_DIR, f'benchmark_algo_{algo_id}_*.csv')
    files = glob.glob(pattern)
    if not files:
        return None
    return max(files, key=os.path.getctime)

def main():
    if not os.path.exists(RESULTS_DIR):
        print(f"Error: {RESULTS_DIR} directory not found.")
        return

    data_frames = {}
    
    # Load the latest data for each algorithm
    for algo_id, name in ALGO_NAMES.items():
        file_path = get_latest_csv(algo_id)
        if file_path:
            df = pd.read_csv(file_path)
            # Remove any trailing empty columns
            df = df.dropna(axis=1, how='all')
            data_frames[algo_id] = df
            print(f"Loaded {name}: {os.path.basename(file_path)}")
        else:
            print(f"Warning: No data found for {name}")

    if not data_frames:
        print("No CSV files found to plot.")
        return

    # Check if Sequential exists for Speedup calculations
    if 1 not in data_frames:
        print("Warning: Sequential data (Algorithm 1) is missing. Cannot calculate Speedup or Efficiency.")
        base_df = None
    else:
        base_df = data_frames[1].set_index('size')

    # Setup plots
    fig_time, ax_time = plt.subplots(figsize=(10, 6))
    fig_speedup, ax_speedup = plt.subplots(figsize=(10, 6))
    fig_eff, ax_eff = plt.subplots(figsize=(10, 6))
    fig_energy, ax_energy = plt.subplots(figsize=(10, 6))

    for algo_id, df in data_frames.items():
        name = ALGO_NAMES[algo_id]
        
        # Only plot data where time > 0.0 (meaning it didn't fail/crash)
        valid_df = df[df['time'] > 0.0].copy()
        if valid_df.empty:
            continue
            
        valid_df.set_index('size', inplace=True)
        
        # 1. Execution Time Plot
        ax_time.plot(valid_df.index, valid_df['time'], marker='o', label=name)

        # 2. Energy Plot (If available)
        if 'energy' in valid_df.columns:
            ax_energy.plot(valid_df.index, valid_df['energy'], marker='o', label=name)

        # 3. Speedup & 4. Efficiency Plots
        if base_df is not None:
            # We can only calculate speedup for sizes that exist in both baseline and current algo
            common_sizes = valid_df.index.intersection(base_df.index)
            if not common_sizes.empty:
                speedup = base_df.loc[common_sizes, 'time'] / valid_df.loc[common_sizes, 'time']
                ax_speedup.plot(common_sizes, speedup, marker='o', label=name)
                
                # Efficiency = Speedup / Number of Processors
                # Note: For iGPU, 'NUM_CORES' doesn't exactly map, but this is a standard approximation
                efficiency = speedup / NUM_CORES
                ax_eff.plot(common_sizes, efficiency, marker='o', label=name)

    # Configure Time Plot
    ax_time.set_title('Execution Time vs Matrix Size')
    ax_time.set_xlabel('Matrix Size (N)')
    ax_time.set_ylabel('Time (Seconds)')
    ax_time.grid(True, linestyle='--', alpha=0.7)
    ax_time.legend()
    fig_time.savefig(os.path.join(RESULTS_DIR, 'plot_time.pdf'), bbox_inches='tight')

    # Configure Speedup Plot
    ax_speedup.set_title('Speedup vs Matrix Size (Baseline: Sequential)')
    ax_speedup.set_xlabel('Matrix Size (N)')
    ax_speedup.set_ylabel('Speedup (T_seq / T_par)')
    ax_speedup.axhline(y=1.0, color='r', linestyle='--', alpha=0.5) # Baseline reference
    ax_speedup.grid(True, linestyle='--', alpha=0.7)
    ax_speedup.legend()
    fig_speedup.savefig(os.path.join(RESULTS_DIR, 'plot_speedup.pdf'), bbox_inches='tight')

    # Configure Efficiency Plot
    ax_eff.set_title(f'Efficiency vs Matrix Size (P={NUM_CORES} cores)')
    ax_eff.set_xlabel('Matrix Size (N)')
    ax_eff.set_ylabel('Efficiency (Speedup / P)')
    ax_eff.grid(True, linestyle='--', alpha=0.7)
    ax_eff.legend()
    fig_eff.savefig(os.path.join(RESULTS_DIR, 'plot_efficiency.pdf'), bbox_inches='tight')

    # Configure Energy Plot
    ax_energy.set_title('Energy Consumption vs Matrix Size')
    ax_energy.set_xlabel('Matrix Size (N)')
    ax_energy.set_ylabel('Energy (Joules)')
    ax_energy.grid(True, linestyle='--', alpha=0.7)
    ax_energy.legend()
    fig_energy.savefig(os.path.join(RESULTS_DIR, 'plot_energy.pdf'), bbox_inches='tight')

    print("\nGraphs successfully generated in the 'results/' directory:")
    print(" - plot_time.pdf")
    print(" - plot_speedup.pdf")
    print(" - plot_efficiency.pdf")
    print(" - plot_energy.pdf")

if __name__ == "__main__":
    main()