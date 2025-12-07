#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "structures.h"
#include <stdio.h>

// Tipo de função para executar algoritmo genérico
// args: parâmetros específicos do algoritmo
// prob: problema a resolver
// Retorna: solução encontrada
typedef Solution (*AlgRunner)(void *args, Problem *prob);

// Estatísticas de múltiplas execuções de um algoritmo
typedef struct {
    double best;                    // Melhor resultado
    double worst;                   // Pior resultado
    double avg;                     // Média dos resultados
    double std_dev;                 // Desvio padrão
    double all_results[MAX_RUNS];  // Todos os resultados individuais
    int num_runs;                   // Número de execuções
} Statistics;

// Executa algoritmo múltiplas vezes e coleta estatísticas
// label: nome da configuração para logs
// runner: função que executa o algoritmo
// args: parâmetros do algoritmo
// num_runs: quantas vezes executar
// output: arquivo para log detalhado
// csv: arquivo para dados em CSV
// prob: problema a resolver
Statistics run_trials(const char *label, AlgRunner runner, void *args,
                      int num_runs, FILE *output, FILE *csv, Problem *prob);

#endif