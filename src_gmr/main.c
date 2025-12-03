#include <stdio.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <ficheiro_tourism>\n", argv[0]);
        return 1;
    }

    if (!init_data(argv[1])) {
        return 1;
    }

    init_rand();

    printf("Problema carregado: C = %d, m = %d\n", prob.C, prob.m);

    Solution s;
    random_solution(&s);
    print_solution(&s);

    
    Solution best = hill_climbing(1000);
    printf("=== HILL CLIMBING ===\n");
    print_solution(&best);
/* 
        Solution s0;
    random_solution(&s0);
    printf("Solucao inicial:\n");
    print_solution(&s0);

    Solution best = hill_climbing_from(s0, 1000);
    printf("\n=== HILL CLIMBING ===\n");
    printf("Solucao final:\n");
    print_solution(&best); */

    return 0;
}
