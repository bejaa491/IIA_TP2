#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_CANDIDATES 500
#define MAX_RUNS 10
#define INF 1e9

// Estrutura para representar uma solução
typedef struct {
    int selected[MAX_CANDIDATES];  // 1 se o ponto está selecionado, 0 caso contrário
    int num_selected;              // Número de pontos selecionados
    double fitness;                // Valor da função objetivo
} Solution;

// Estrutura para os dados do problema
typedef struct {
    int C;                                    // Total de candidaturas
    int m;                                    // Número de pontos a selecionar
    double dist[MAX_CANDIDATES][MAX_CANDIDATES]; // Matriz de distâncias
} Problem;

// Estrutura para guardar resultados dos melhores algoritmos
typedef struct {
    char name[100];
    Statistics stats;
} AlgorithmResult;

// Estrutura para armazenar resultados
typedef struct {
    double best;
    double worst;
    double avg;
    double std_dev;
    double all_results[MAX_RUNS];
    int num_runs;
} Statistics;

#endif