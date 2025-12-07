#ifndef STRUCTURES_H
#define STRUCTURES_H

/* Constantes do sistema */
#define MAX_CANDIDATES 500   /* Número máximo de candidatos suportado */
#define MAX_RUNS 10          /* Número máximo de execuções para testes */
#define INF 1e9              /* Valor "infinito" para comparações */

/* Estrutura que representa uma solução do problema */
typedef struct {
    int selected[MAX_CANDIDATES];  /* 1 se ponto está selecionado, 0 caso contrário */
    int num_selected;              /* número de pontos selecionados (deve ser igual a m) */
    double fitness;                /* valor da função objetivo (distância média) */
} Solution;

/* Estrutura com dados do problema */
typedef struct {
    int C;                                    /* total de candidatos */
    int m;                                    /* número de pontos a selecionar (m <= C) */
    double dist[MAX_CANDIDATES][MAX_CANDIDATES]; /* matriz simétrica de distâncias */
} Problem;

#endif /* STRUCTURES_H */