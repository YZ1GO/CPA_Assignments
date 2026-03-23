# Parallel Sieve of Eratosthenes

Parallel prime sieve using OpenMP with block segmentation for cache-friendly execution.

## Algorithm

*Phase 1 — Serial:* Sieve [0, √N] to find seed primes. These are stored in a
read-only array shared across all threads, eliminating any broadcast step.

*Phase 2 — Parallel:* The range (√N, N] is divided into blocks of BLOCK_SIZE
odd numbers. Each thread claims a block, allocates a local char array that fits in
L1/L2 cache, and uses the seed primes to mark composites via fast marking (jumping
directly to the first multiple of k in the block, no modulo per element).

*Phase 3 — Reduction:* OpenMP reduction(+:total_count) sums local prime counts
with no mutexes or atomics.

## Optimizations

| Technique | Effect |
|---|---|
| Odd-only storage (j >> 1) | Halves memory usage |
| Block segmentation | Each block fits in L1 cache (~32 KB) |
| Seeds read-only | Zero contention between threads |
| schedule(dynamic) | Balances load (early blocks have more marking work) |
| Fast marking | No mod per element — direct formula to first multiple |
| OMP reduction | No synchronization overhead during counting |

## Compile

```bash
gcc -O2 -fopenmp -o parallel_sieve parallel_sieve.cpp -lm
```


## Usage

```bash
./parallel_sieve <power_of_10> [num_threads]
```

Examples:

```bash
./parallel_sieve 9      # sieve up to 10^9, all cores
./parallel_sieve 9 4    # sieve up to 10^9, 4 threads
./parallel_sieve 12 8   # sieve up to 10^12, 8 threads
```


## Tuning

BLOCK_SIZE (defined at the top) controls how many odd numbers each block covers,
and therefore how much memory each thread's local array uses:

| BLOCK_SIZE | Memory per block | Target cache |
|---|---|---|
| 16384 | 16 KB | L1 (small) |
| 32768 | 32 KB | L1 (default) |
| 65536 | 64 KB | L1/L2 boundary |
| 262144 | 256 KB | L2 |

Benchmark different values for your specific CPU to find the sweet spot.

## Memory usage

Unlike a flat sieve, memory usage is constant regardless of N:

| N | Flat sieve | This implementation |
|---|---|---|
| 10⁹ | ~500 MB | ~32 KB + seeds |
| 10¹⁰ | ~5 GB | ~32 KB + seeds |
| 10¹² | ~500 GB | ~32 KB + seeds |

The seed array (primes up to √N) grows slowly: at N=10¹² it holds ~78k primes (~600 KB).

## Block decomposition macros

Adapted for block-based sieving:

```c
#define BLOCK_LOW(b)        ((b) * BLOCK_SIZE)
// Computes the starting index of block 'b' in the array of odd numbers

#define BLOCK_HIGH(b, tot)  (((b) * BLOCK_SIZE + BLOCK_SIZE - 1) < (tot) ? \
                             (b) * BLOCK_SIZE + BLOCK_SIZE - 1 : (tot) - 1)
// Computes the ending index of block 'b', clamped to total size

#define BLOCK_LEN(b, tot)   (BLOCK_HIGH(b, tot) - BLOCK_LOW(b) + 1)
// Calculates the length of block 'b'

#define BLOCK_TO_ODD(idx, first) ((first) + (idx) * 2)
// Converts a block-relative index to the actual odd number value
```

These macros divide the range of odd numbers into fixed-size blocks for parallel processing, ensuring each thread works on a contiguous segment that fits in cache.