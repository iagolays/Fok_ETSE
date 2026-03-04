/*
 * acp3.c
 *
 * Arquitectura de Computadores - Práctica 1
 *
 * Variante:
 *   - Array A[] de tipo double
 *   - Acceso DIRECTO: A[i*D] (sen vector ind[])
 *
 * Saída CSV:
 *   D,R,L,S_media,Ciclos_por_bucle,Ciclos_por_acceso
 *
 * Compilación:
 *   gcc acp3.c -o acp3 -O0 -std=c17
 *
 * Uso:
 *   ./acp3 D L
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "counter.h"

#define REP 10
#define CLS 64
#define SIZE_DOUBLE 8

static inline size_t round_up_mult(size_t x, size_t m) {
    return ((x + m - 1) / m) * m;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Uso: %s D L\n", argv[0]);
        return 1;
    }

    long D_in = atol(argv[1]);
    long L_in = atol(argv[2]);

    if (D_in <= 0 || L_in <= 0) {
        fprintf(stderr, "Error: D y L deben ser positivos.\n");
        return 1;
    }

    const size_t D = (size_t)D_in;
    const size_t L = (size_t)L_in;

    /*
     * Para double:
     * R ≈ (L*64)/(8*D) = (8L)/D
     */
    size_t R = (size_t)((L * (size_t)CLS) / ((size_t)SIZE_DOUBLE * D));
    if (R < 1) R = 1;

    size_t N = (R - 1) * D + 1;

    size_t bytes = N * sizeof(double);
    bytes = round_up_mult(bytes, CLS);

    double *A = (double *)aligned_alloc(CLS, bytes);
    if (!A) {
        perror("Erro reservando A");
        return 1;
    }

    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    for (size_t i = 0; i < N; i++) {
        double signo = (rand() & 1) ? 1.0 : -1.0;
        A[i] = signo * (1.0 + ((double)rand() / (double)RAND_MAX));
    }

    double S[REP];

    start_counter();

    for (int rep = 0; rep < REP; rep++) {
        double sum = 0.0;

        for (size_t i = 0; i < R; i++) {
            sum += A[i * D];   // ACCESO DIRECTO
        }

        S[rep] = sum;
    }

    double total_cycles = get_counter();

    double S_media = 0.0;
    for (int i = 0; i < REP; i++)
        S_media += S[i];
    S_media /= (double)REP;

    double ciclos_por_bucle  = total_cycles / (double)REP;
    double ciclos_por_acceso = total_cycles / ((double)REP * (double)R);

    printf("%zu,%zu,%zu,%.10lf,%.3lf,%.5lf\n",
           D, R, L, S_media,
           ciclos_por_bucle,
           ciclos_por_acceso);

    free(A);

    return 0;
}