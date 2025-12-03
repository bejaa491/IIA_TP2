#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structures.h"
#include "utils.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "hybrid.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Uso: %s <ficheiro_entrada>\n", argv[0]);
        return 1;
    }

    Problem prob;
    if (!read_file(argv[1], &prob)) {
        return 1;
    };
    printf("Ficheiro %s lido com sucesso.\n", argv[1]);
    /* DEBUG: imprime matriz de distâncias para verificar leitura
    printf("Matriz de distâncias (C=%d):\n", prob.C);
    for (int i = 0; i < prob.C; i++) {
        for (int j = 0; j < prob.C; j++) {
            printf("%8.3f ", prob.dist[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    */
    printf("Problema carregado: C=%d, m=%d\n\n", prob.C, prob.m);

    // Teste Hill Climbing
    printf("=== HILL CLIMBING ===\n");
    Solution hc_best = hill_climbing(1000, 1, &prob);
    printf("Melhor solução HC:\n");
    print_solution(&hc_best, &prob);

    // Teste Evolutivo
    printf("\n=== EVOLUTIVO ===\n");
    Solution ea_best = evolutionary_algorithm(50, 100, 0.8, 0.1, 1, 1, &prob);
    printf("Melhor solução EA:\n");
    print_solution(&ea_best, &prob);

    // Teste Híbrido 1
    printf("\n=== HÍBRIDO 1 ===\n");
    Solution h1_best = hybrid1(50, 100, &prob);
    printf("Melhor solução H1:\n");
    print_solution(&h1_best, &prob);

    // Teste Híbrido 2
    printf("\n=== HÍBRIDO 2 ===\n");
    Solution h2_best = hybrid2(1000, &prob);
    printf("Melhor solução H2:\n");
    print_solution(&h2_best, &prob);

    return 0;
}