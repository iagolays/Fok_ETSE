/*
 * acp1.c
 *
 * Arquitectura de Computadores - Práctica 1
 *
 * Obxectivo:
 * Estudar o comportamento da xerarquía de memoria
 * medindo o custo de acceso a memoria con distintos
 * strides (D) e distintos tamaños efectivos (L).
 *
 * Saída en formato CSV:
 *   D,R,L,S_media,Ciclos_por_bucle,Ciclos_por_acceso
 *
 * Compilación:
 *   gcc acp1.c -o acp1 -O0 -std=c17
 *
 * Uso:
 *   ./acp1 D L
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "counter.h"

/* Número de repeticións do experimento.
 * Permite reducir variabilidade e obter media estable.
 */
#define REP 10

/* Tamaño da liña de caché en bytes.
 * Normalmente 64B en arquitecturas modernas.
 */
#define CLS 64

/* Tamaño dun double en bytes */
#define SIZE_DOUBLE 8


/* 
 * Función auxiliar:
 * Redondea x ao seguinte múltiplo de m.
 *
 * É necesaria porque aligned_alloc require que
 * o tamaño reservado sexa múltiplo do aliñamento.
*/
static inline size_t round_up_mult(size_t x, size_t m) {
    return ((x + m - 1) / m) * m;
}

int main(int argc, char *argv[]) {

    /* --- Comprobación de argumentos --- */

    if (argc != 3) {
        fprintf(stderr, "Uso: %s D L\n", argv[0]);
        return 1;
    }

    long D_in = atol(argv[1]);   // Stride
    long L_in = atol(argv[2]);   // Nº liñas efectivas

    if (D_in <= 0 || L_in <= 0) {
        fprintf(stderr, "Error: D y L deben ser positivos.\n");
        return 1;
    }

    /* Convertimos a size_t porque imos traballar
     * con tamaños de memoria.
    */
    const size_t D = (size_t)D_in;
    const size_t L = (size_t)L_in;

    /*
     * Cálculo de R:
     *
     * Queremos acceder aproximadamente a L liñas de caché.
     *
     * Cada liña ten CLS = 64 bytes.
     * Cada double ocupa 8 bytes.
     *
     * Número de doubles por liña:
     *   64 / 8 = 8 doubles
     *
     * Se stride = D, cada acceso salta D doubles.
     *
     * Queremos cubrir L liñas:
     *
     * R ≈ (L * 64) / (8 * D)
     *   = (8L) / D
     *
     * R é o número total de accesos que se van facer.
    */
    size_t R = (size_t)((L * (size_t)CLS) / ((size_t)SIZE_DOUBLE * D));

    if (R < 1) R = 1; // Garantir que facemos polo menos un acceso

    /*
     * Tamaño mínimo necesario do vector:
     *
     * O último acceso será:
     *   A[(R-1)*D]
     *
     * Polo tanto necesitamos que o vector
     * teña polo menos:
     *
     *   N = (R-1)*D + 1
     */
    size_t N = (R - 1) * D + 1;

    /*
     * Reserva aliñada:
     *
     * aligned_alloc require que:
     *  - o aliñamento sexa potencia de 2
     *  - o tamaño sexa múltiplo do aliñamento
     *
     * Aliñamos a 64 bytes (tamaño liña caché).
     */
    size_t bytes = N * sizeof(double);
    bytes = round_up_mult(bytes, CLS);

    double *A = (double *)aligned_alloc(CLS, bytes);
    if (!A) {
        perror("Erro reservando A");
        return 1;
    }

    /*
     * Vector de índices para acceso indirecto.
     *
     * Isto evita que o compilador transforme o acceso
     * nun patrón máis optimizable.
     */
    long *ind = (long *)malloc(R * sizeof(long));
    if (!ind) {
        perror("Erro reservando ind");
        free(A);
        return 1;
    }

    /*
     * Inicialización aleatoria.
     *
     * Evita:
     *  - que o compilador detecte patrón constante
     *  - que a suma sexa trivial
     */
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    for (size_t i = 0; i < N; i++) {
        double signo = (rand() & 1) ? 1.0 : -1.0;
        A[i] = signo * (1.0 + ((double)rand() / (double)RAND_MAX));
    }

    /*
     * Construímos os índices con stride D:
     *
     * ind[i] = i * D
     *
     * Isto produce accesos espazados.
     */
    for (size_t i = 0; i < R; i++) {
        ind[i] = (long)(i * D);
    }

    double S[REP];

    /*
     * Medición global:
     *
     * Medimos as 10 repeticións como un único bloque.
     * Isto reduce overhead de medición.
     */
    start_counter();

    for (int rep = 0; rep < REP; rep++) {

        double sum = 0.0;

        for (size_t i = 0; i < R; i++) {

            /*
             * ACCESO INDIRECTO:
             *
             * A[ind[i]]
             *
             * Isto é o núcleo do experimento.
             * Aquí prodúcense os accesos a memoria
             * que queremos medir.
             */
            sum += A[ind[i]];
        }

        /*
         * Gardamos o resultado para evitar
         * que o compilador elimine o bucle.
         */
        S[rep] = sum;
    }

    double total_cycles = get_counter();

    /*
     * Media das sumas.
     * Só serve como ancla anti-optimización.
     */
    double S_media = 0.0;
    for (int i = 0; i < REP; i++)
        S_media += S[i];

    S_media /= (double)REP;

    /*
     * Métricas finais:
     *
     * ciclos_por_bucle:
     *   custo dunha repetición completa
     *
     * ciclos_por_acceso:
     *   custo medio dun acceso individual
     */
    double ciclos_por_bucle  = total_cycles / (double)REP;
    double ciclos_por_acceso = total_cycles / ((double)REP * (double)R);

    /*
     * Saída en formato CSV para posterior análise.
     */
    printf("%zu,%zu,%zu,%.10lf,%.3lf,%.5lf\n",
           D, R, L, S_media,
           ciclos_por_bucle,
           ciclos_por_acceso);

    free(ind);
    free(A);

    return 0;
}