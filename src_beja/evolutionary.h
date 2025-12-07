#ifndef EVOLUTIONARY_H
#define EVOLUTIONARY_H

#include "structures.h"

// Seleção por torneio
int tournament_selection(Solution *pop, int pop_size, int tournament_size);

// Seleção por roleta
int roulette_selection(Solution *pop, int pop_size);

// Recombinação uniforme
void uniform_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob);

// Recombinação de um ponto
void one_point_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob);

// Mutação por troca
void swap_mutation(Solution *s, Problem *prob);

// Mutação por inversão
void inversion_mutation(Solution *s, Problem *prob);

// Algoritmo evolutivo
Solution evolutionary_algorithm(int pop_size, int generations, double cross_prob,
                                double mut_prob, int selection_type, int crossover_type,
                                Problem *prob);

#endif