#ifndef FUNCAO_H
#define FUNCAO_H

#include "utils.h"   // para MAX_CANDIDATES e Problem/prob

typedef struct {
    int selected[MAX_CANDIDATES];  // 1 se o ponto i está escolhido
    int num_selected;              // quantos pontos estão a 1
    double fitness;                // valor da função objetivo
} Solution;

double calculate_fitness(Solution *s);

void random_solution(Solution *s);
void copy_solution(Solution *dest, const Solution *src);
void print_solution(const Solution *s);
int count_unique_fitness(Solution *pop, int pop_size);

#endif
