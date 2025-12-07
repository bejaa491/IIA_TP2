#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "structures.h"
#include <stdio.h>

typedef Solution (*AlgRunner)(void *args, Problem *prob);

typedef struct {
    double best;
    double worst;
    double avg;
    double std_dev;
    double all_results[MAX_RUNS];
    int num_runs;
} Statistics;

// Roda um algoritmo várias vezes, escreve output e csv e devolve estatísticas
Statistics run_trials(const char *label, AlgRunner runner, void *args,
                      int num_runs, FILE *output, FILE *csv, Problem *prob);

#endif