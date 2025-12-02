// problem.c
#include <stdio.h>
#include "problem.h"

Problem prob;  // AQUI é que a variável é “criada”

int read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Erro ao abrir ficheiro %s\n", filename);
        return 0;
    }

    fscanf(f, "%d %d", &prob.C, &prob.m);

    for (int i = 0; i < prob.C; i++) {
        for (int j = 0; j < prob.C; j++) {
            fscanf(f, "%lf", &prob.dist[i][j]);
        }
    }

    fclose(f);
    return 1;
}
