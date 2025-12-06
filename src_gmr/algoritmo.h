#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "funcao.h" // para usar Solution
// #include <math.h>

// Gera um vizinho da solução actual (troca 1 ponto escolhido com 1 não escolhido)
void gera_vizinho_swap(const Solution *actual, Solution *vizinho);
// Gera um vizinho da solução actual (troca 2 pontos escolhidos por 2 não escolhidos)
void gera_vizinho_swap2(const Solution *actual, Solution *vizinho);

// Trepa-colinas simples: começa numa solução aleatória e faz max_iter passos
Solution hill_climbing_old(int max_iter);
Solution hill_climbing(int max_iter, int vizinhanca_tipo);
// Trepa-colinas a partir de uma solução dada, faz max_iter passos
Solution hill_climbing_from(Solution start, int max_iter);

// Simulated Annealing: começa numa solução aleatória e faz arrefecimento
Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo);
Solution simulated_annealing_from(Solution start_sol, double tmax, double tmin, double alpha, int vizinhanca_tipo);

// Mutação Swap 2 (Troca 2 pontos de uma vez)
void swap2_mutation(Solution *s);

// Algoritmo Evolutivo Genérico
// selection_type: 1 = Torneio, 2 = Roleta
// crossover_type: 1 = Uniforme, 2 = One-Point
Solution evolutionary_algorithm(int pop_size, int max_generations, double prob_crossover, double prob_mutation, int selection_type, int crossover_type);



#endif
