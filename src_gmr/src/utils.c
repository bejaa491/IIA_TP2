#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"

// Inicializacao dos dados do problema
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

Problem prob;  // definição da variável global

int init_data(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erro ao abrir o ficheiro %s\n", filename);
        return 0;
    }

    // 1) Ler C e m
    if (fscanf(f, "%d %d", &prob.C, &prob.m) != 2) {
        printf("Erro a ler C e m\n");
        fclose(f);
        return 0;
    }

    // 2) Inicializar matriz a 0
    for (int i = 0; i < prob.C; i++) {
        for (int j = 0; j < prob.C; j++) {
            prob.dist[i][j] = 0.0;
        }
    }

    // 3) Ler cada linha do tipo: e1 e2 50.171360
    char s1[8], s2[8];
    int a, b;
    double d;

    while (fscanf(f, "%s %s %lf", s1, s2, &d) == 3) {
        // s1 = "e1", s2 = "e2" → extrair os índices numéricos
        if (sscanf(s1, "e%d", &a) != 1) continue;
        if (sscanf(s2, "e%d", &b) != 1) continue;

        // passar de 1-based (e1..eC) para 0-based (0..C-1)
        a--; 
        b--;

        prob.dist[a][b] = d;
        prob.dist[b][a] = d; // matriz simétrica
    }

    fclose(f);
    return 1;
}

// Inicializacao do gerador de numeros aleatorios
void init_rand()
{
    srand((unsigned)time(NULL));
}

// Simula o lancamento de uma moeda, retornando o valor 0 ou 1
int flip() {
    return (rand_01() < 0.5) ? 0 : 1;
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
