#ifndef EVOLUTIONARY_H
#define EVOLUTIONARY_H

#include "structures.h"

// Seleção por torneio: escolhe melhor de tournament_size indivíduos aleatórios
int tournament_selection(Solution *pop, int pop_size, int tournament_size);

// Seleção por roleta: probabilidade proporcional ao fitness
int roulette_selection(Solution *pop, int pop_size);

// Recombinação uniforme: cada gene herdado aleatoriamente de um dos pais
void uniform_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob);

// Recombinação de um ponto: filho herda início de p1 e fim de p2
void one_point_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob);

// Mutação por troca: troca um ponto selecionado por um não selecionado
void swap_mutation(Solution *s, Problem *prob);

// Mutação por inversão: inverte ordem de parte dos pontos selecionados
void inversion_mutation(Solution *s, Problem *prob);

// Algoritmo evolutivo completo com todos os operadores configuráveis
Solution evolutionary_algorithm(int pop_size, int generations, double cross_prob,
                                double mut_prob, int selection_type, int crossover_type,
                                Problem *prob);

#endif