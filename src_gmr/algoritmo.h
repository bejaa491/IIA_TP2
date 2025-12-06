#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "funcao.h"

// Hill Climbing (HC - Trepa-Colinas)
Solution hill_climbing(int max_iter, int vizinhanca_tipo);
Solution hill_climbing_from(Solution start_sol, int max_iter, int vizinhanca_tipo); // Necessário para o Híbrido:

// Simulated Annealing (SA - Recristalização)
Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo);
Solution simulated_annealing_from(Solution start_sol, double tmax, double tmin, double alpha, int vizinhanca_tipo); // Necessário para o Híbrido:

// Genetic Algorithm (GA - Algoritmo Evolutivo) 
Solution evolutionary_algorithm(int pop_size, int generations, double prob_cross, double prob_mut, int sel_type, int cross_type); 

// Híbridos (GA + SA e GA + HC)
Solution hybrid_algorithm_1(int pop_size, int generations, double tmax, double tmin, int sel_type, int cross_type, int viz_type); // Evolutionary + SA
Solution hybrid_algorithm_2(int pop_size, int generations, int hc_iter, int sel_type, int cross_type, int viz_type); // Evolutionary + HC

#endif