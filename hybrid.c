#include "hybrid.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "utils.h"

// Híbrido 1: Evolutivo + Hill Climbing na melhor solução
Solution hybrid1(int pop_size, int generations, Problem *prob) {
    // Executa algoritmo evolutivo
    Solution best = evolutionary_algorithm(pop_size, generations, 0.8, 0.1, 1, 1, prob);

    // Refina a melhor solução com hill climbing
    Solution refined = best;
    Solution neighbor;
    
    for (int iter = 0; iter < 200; iter++) {
        neighborhood1(&refined, &neighbor, prob);

        if (neighbor.fitness > refined.fitness) {
            refined = neighbor;
        }
    }

    if (refined.fitness > best.fitness) {
        best = refined;
    }

    return best;
}

// Híbrido 2: Hill Climbing + Evolutivo para diversificar
Solution hybrid2(int iterations, Problem *prob) {
    Solution best;
    random_solution(&best, prob);

    int num_restarts = 5;
    
    for (int restart = 0; restart < num_restarts; restart++) {
        // Fase 1: Hill Climbing
        Solution hc_solution = hill_climbing(iterations / num_restarts, 1, prob);

        if (hc_solution.fitness > best.fitness) {
            best = hc_solution;
        }

        // Fase 2: Evolutivo para diversificar
        Solution ea_solution = evolutionary_algorithm(30, 50, 0.7, 0.2, 1, 1, prob);
        
        if (ea_solution.fitness > best.fitness) {
            best = ea_solution;
        }
    }

    return best;
}

// Híbrido 3: Evolutivo com refinamento local em cada geração
Solution hybrid3(int pop_size, int generations, Problem *prob) {
    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i], prob);
    }

    copy_solution(&best, &population[0]);

    for (int gen = 0; gen < generations; gen++) {
        // Encontra melhor
        for (int i = 0; i < pop_size; i++) {
            if (population[i].fitness > best.fitness) {
                copy_solution(&best, &population[i]);
            }
        }

        // Nova geração com operadores evolutivos
        for (int i = 0; i < pop_size; i++) {
            int p1 = tournament_selection(population, pop_size, 3);
            int p2 = tournament_selection(population, pop_size, 3);

            if ((double)rand() / RAND_MAX < 0.8) {
                uniform_crossover(&population[p1], &population[p2], &new_population[i], prob);
            } else {
                copy_solution(&new_population[i], &population[p1]);
            }

            if ((double)rand() / RAND_MAX < 0.1) {
                swap_mutation(&new_population[i], prob);
            }

            // Refinamento local nas melhores soluções
            if (i < pop_size / 4) {
                Solution refined = new_population[i];
                Solution neighbor;
                
                for (int local_iter = 0; local_iter < 10; local_iter++) {
                    neighborhood1(&refined, &neighbor, prob);
                    if (neighbor.fitness > refined.fitness) {
                        refined = neighbor;
                    }
                }
                
                if (refined.fitness > new_population[i].fitness) {
                    new_population[i] = refined;
                }
            }
        }

        // Elitismo
        copy_solution(&new_population[0], &best);

        Solution *temp = population;
        population = new_population;
        new_population = temp;
    }

    free(population);
    free(new_population);

    return best;
}