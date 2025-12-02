#ifndef SOLUTION_H
#define SOLUTION_H

#include "problem.h"   // para usar MAX_CANDIDATES e prob.m, prob.C

typedef struct {
    int selected[MAX_CANDIDATES];  // 1 se o ponto está selecionado
    int num_selected;              // nº de pontos selecionados
    double fitness;                // valor da função objetivo
} Solution;

// Funções que trabalham sobre uma solução
double calculate_fitness(Solution *s);
void random_solution(Solution *s);
void copy_solution(Solution *dest, const Solution *src);
void print_solution(const Solution *s);

#endif
