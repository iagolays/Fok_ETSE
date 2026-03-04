/*
 * acp2.c
 *
 * Arquitectura de Computadores - Práctica 1
 *
 * Variante:
 *   - Array A[] de tipo int
 *   - Acceso INDIRECTO mediante vector ind[]
 *
 * Saída CSV:
 *   D,R,L,S_media,Ciclos_por_bucle,Ciclos_por_acceso
 *
 * Compilación:
 *   gcc acp2.c -o acp2 -O0 -std=c17
 *
 * Uso:
 *   ./acp2 D L
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "counter.h"

#define REP 10
#define CLS 64
#define SIZE_INT 4

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
     * Para int:
     *   sizeof(int)=4
     *   CLS=64
     *   ints por liña = 64/4 = 16
     *
     * L ≈ (R * D * 4) / 64  ->  R ≈ (L*64)/(4*D) = (16L)/D
     */
    size_t R = (size_t)((L * (size_t)CLS) / ((size_t)SIZE_INT * D));
    if (R < 1) R = 1;

    size_t N = (R - 1) * D + 1;

    size_t bytes = N * sizeof(int);
    bytes = round_up_mult(bytes, CLS);

    int *A = (int *)aligned_alloc(CLS, bytes);
    if (!A) {
        perror("Erro reservando A");
        return 1;
    }

    long *ind = (long *)malloc(R * sizeof(long));
    if (!ind) {
        perror("Erro reservando ind");
        free(A);
        return 1;
    }

    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    /*
     * Inicialización: valores pequenos para evitar calquera risco
     * (a suma pode ser grande se R é grande).
     * Usamos rango [-1000, 1000].
     */
    for (size_t i = 0; i < N; i++) {
        int v = (rand() % 2001) - 1000;
        A[i] = v;
    }

    for (size_t i = 0; i < R; i++) {
        ind[i] = (long)(i * D);
    }

    long long S[REP]; //se usa long long para evitar overflow en la suma de enteros

    start_counter();

    for (int rep = 0; rep < REP; rep++) {
        long long sum = 0;
        for (size_t i = 0; i < R; i++) {
            sum += (long long)A[ind[i]];   // ACCESO INDIRECTO
        }
        S[rep] = sum;
    }

    double total_cycles = get_counter();

    /*
     * Media das sumas (en double para imprimir como no resto de variantes)
     */
    double S_media = 0.0;
    for (int i = 0; i < REP; i++) {
        S_media += (double)S[i];
    }
    S_media /= (double)REP;

    double ciclos_por_bucle  = total_cycles / (double)REP;
    double ciclos_por_acceso = total_cycles / ((double)REP * (double)R);

    printf("%zu,%zu,%zu,%.10lf,%.3lf,%.5lf\n",
           D, R, L, S_media,
           ciclos_por_bucle,
           ciclos_por_acceso);

    free(ind);
    free(A);

    return 0;
}