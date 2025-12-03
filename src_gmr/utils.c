#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"

// Inicializacao dos dados do problema
struct info init_data(char *s, int mat[][2])
{
    struct info d;
    FILE *f;
    int i, j;

    f = fopen(s, "r");
    if (f == NULL)
    {
        printf("Erro ao abrir o ficheiro %s\n", s);
        exit(1);
    }

    fscanf(f, "%d %d", &prob.C, &prob.m);

    for (i = 0; i < prob.C; i++) {
        for (j = 0; j < prob.C; j++) {
            fscanf(f, "%lf", &prob.dist[i][j]);
        }
    }

    fclose(f);
    return d;
}

// Inicializacao do gerador de numeros aleatorios
void init_rand()
{
    srand((unsigned)time(NULL));
}

// Simula o lancamento de uma moeda, retornando o valor 0 ou 1
int flip()
{
    if ((((float)rand()) / RAND_MAX) < 0.5)
        return 0;
    else
        return 1;
}

// Devolve um valor inteiro distribuido uniformemente entre min e max
int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Devolve um valor real distribuido uniformemente entre 0 e 1
float rand_01()
{
    return ((float)rand()) / RAND_MAX;
}
