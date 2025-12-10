#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "funcao.h"

// --- Vizinhanças e Mutações ---
void gera_vizinho_swap(const Solution *actual, Solution *vizinho);
void gera_vizinho_swap2(const Solution *actual, Solution *vizinho);
void swap_mutation(Solution *s);
void swap2_mutation(Solution *s);

// --- Crossovers ---
void uniform_crossover(Solution *p1, Solution *p2, Solution *child);
void one_point_crossover(Solution *p1, Solution *p2, Solution *child);
void two_point_crossover(Solution *p1, Solution *p2, Solution *child);

// --- Pesquisa Local (HC - Hill Climbing) ---
Solution hill_climbing(int max_iter, int vizinhanca_tipo);
Solution hill_climbing_from(Solution start_sol, int max_iter, int vizinhanca_tipo); // Necessário para o Híbrido:

// --- Simulated Annealing (SA - Recristalização) ---

Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo);
Solution simulated_annealing_from(Solution start_sol, double tmax, double tmin, double alpha, int vizinhanca_tipo); // Necessário para o Híbrido:

// --- Algoritmo Evolutivo (GA - Genetic Algorithm ) ---
// --- Evolutivo + Memético + Estratégia de Inválidos ---
Solution evolutionary_algorithm(int pop_size, int generations, double prob_cross, double prob_mut, int sel_type, int cross_type, int viz_type, double prob_ls, int *out_div);

// --- Híbridos (GA + SA e GA + HC) ---
Solution hybrid_algorithm_1(int pop_size, int generations, double tmax, double tmin, int sel_type, int cross_type, int viz_type); // Evolutionary + SA
Solution hybrid_algorithm_2(int pop_size, int generations, int hc_iter, int sel_type, int cross_type, int viz_type); // Evolutionary + HC
#endif