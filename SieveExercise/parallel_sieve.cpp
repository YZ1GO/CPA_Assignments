#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define BLOCK_SIZE 32768

#define odd_index(x)        ((x)>>1)
#define index_to_odd(i)     (((i)<<1)|1)

#define BLOCK_LOW(b)        ((b) * BLOCK_SIZE)
#define BLOCK_HIGH(b, tot)  (((b) * BLOCK_SIZE + BLOCK_SIZE - 1) < (tot) ? \
                             (b) * BLOCK_SIZE + BLOCK_SIZE - 1 : (tot) - 1)
#define BLOCK_LEN(b, tot)   (BLOCK_HIGH(b, tot) - BLOCK_LOW(b) + 1)
#define BLOCK_TO_ODD(idx, first) ((first) + (idx) * 2)

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <power_of_10> [num_threads]\n", argv[0]);
        return 1;
    }

    long long power = atoll(argv[1]);
    long long n     = (long long)pow(10.0, (double)power);

    int num_threads = omp_get_max_threads();
    if (argc >= 3)
        num_threads = atoi(argv[2]);
    omp_set_num_threads(num_threads);

    printf("Sieving primes up to %lld (10^%lld)\n", n, power);
    printf("Threads : %d\n", num_threads);
    printf("Block   : %d odds = %d KB\n\n", BLOCK_SIZE, BLOCK_SIZE / 1024);

    /* Phase 1: seed primes up to sqrt(n) */
    long long sqrt_n    = (long long)ceil(sqrt((double)n));
    long long seed_size = odd_index(sqrt_n) + 1;

    char *seed_sieve = (char *)calloc(seed_size, sizeof(char));
    if (!seed_sieve) { fprintf(stderr, "Alloc failed\n"); return 1; }

    long long k = 3;
    while (k * k <= sqrt_n) {
        if (!seed_sieve[odd_index(k)])
            for (long long j = k * k; j <= sqrt_n; j += k<<1)
                seed_sieve[odd_index(j)] = 1;
        k += 2;
    }

    long long seed_count = 0;
    for (long long i = 1; i < seed_size; i++)
        if (!seed_sieve[i]) seed_count++;

    long long *seeds = (long long *)malloc(seed_count * sizeof(long long));
    if (!seeds) { fprintf(stderr, "Alloc failed\n"); free(seed_sieve); return 1; }

    long long si = 0;
    for (long long i = 1; i < seed_size; i++)
        if (!seed_sieve[i]) seeds[si++] = index_to_odd(i);

    free(seed_sieve);

    printf("Seeds up to sqrt(%lld) = %lld : %lld primes\n\n", n, sqrt_n, seed_count);

    /* Phase 2: parallel block sieve */
    long long first_odd  = (sqrt_n % 2 == 0) ? sqrt_n + 1 : sqrt_n + 2;
    long long last_odd   = (n % 2 == 0) ? n - 1 : n;
    long long total_odds = (last_odd - first_odd) / 2 + 1;
    long long num_blocks = (total_odds + BLOCK_SIZE - 1) / BLOCK_SIZE;

    long long total_count = 0;
    if (n >= 2) total_count++;
    total_count += seed_count;

    double t_start = omp_get_wtime();

    #pragma omp parallel for schedule(dynamic) reduction(+:total_count)
    for (long long b = 0; b < num_blocks; b++)
    {
        long long actual_size = BLOCK_LEN(b, total_odds);
        long long low         = BLOCK_TO_ODD(BLOCK_LOW(b),  first_odd);
        long long high        = BLOCK_TO_ODD(BLOCK_HIGH(b, total_odds), first_odd);

        char *local_block = (char *)calloc(actual_size, sizeof(char));
        if (!local_block) continue;

        for (long long s = 0; s < seed_count; s++)
        {
            long long k     = seeds[s];
            long long start = ((low + k - 1) / k) * k;
            if (start % 2 == 0) start += k;
            if (start == k)     start += k<<1;

            for (long long j = start; j <= high; j += k<<1)
                local_block[(j - low) >> 1] = 1;
        }

        long long local_count = 0;
        for (long long i = 0; i < actual_size; i++)
            if (!local_block[i]) local_count++;

        total_count += local_count;
        free(local_block);
    }

    double t_end = omp_get_wtime();

    printf("Total primes up to %lld : %lld\n", n, total_count);
    printf("Time elapsed            : %.4f seconds\n", t_end - t_start);

    free(seeds);
    return 0;
}
