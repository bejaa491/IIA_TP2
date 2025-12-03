#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MAX_CANDIDATES 500
#define MAX_RUNS 10
#define INF 1e9

#include "funcao.h"
#include "algoritmo.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    int mat[1][2];          // só para satisfazer a assinatura por agora
    struct info EA_param;

    if (argc < 2) {
        printf("Uso: %s <ficheiro_tourism>\n", argv[0]);
        return 1;
    }

    EA_param = init_data(argv[1], mat);  // só para testar leitura

    printf("Problema carregado: C=%d, m=%d\n", prob.C, prob.m);
    return 0;
}
