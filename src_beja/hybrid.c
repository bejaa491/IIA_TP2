#include "hybrid.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "utils.h"
#include <stdlib.h>

// Híbrido 1: EA global + HC local
// Fase 1: Usa EA para encontrar região promissora do espaço de busca
// Fase 2: Refina melhor solução do EA com HC para ótimo local
Solution hybrid1(int pop_size, int generations, Problem *prob) {
    // Executa algoritmo evolutivo completo
    Solution best = evolutionary_algorithm(pop_size, generations, 0.8, 0.1, 1, 1, prob);

    // Refina melhor solução com hill climbing (200 iterações)
    Solution refined = best;
    Solution neighbor;
    
    for (int iter = 0; iter < 200; iter++) {
        neighborhood1(&refined, &neighbor, prob);

        if (neighbor.fitness > refined.fitness) {
            refined = neighbor;
        }
    }

    // Mantém melhor entre original e refinada
    if (refined.fitness > best.fitness) {
        best = refined;
    }

    return best;
}

// Híbrido 2: Alternância HC ↔ EA
// Estratégia: usa HC para intensificação e EA para escapar de ótimos locais
// Múltiplos restarts garantem exploração de diferentes regiões
Solution hybrid2(int iterations, Problem *prob) {
    Solution best;
    random_solution(&best, prob);

    int num_restarts = 5;
    
    for (int restart = 0; restart < num_restarts; restart++) {
        // Fase 1: Hill Climbing para busca local intensiva
        Solution hc_solution = hill_climbing(iterations / num_restarts, 1, prob);

        if (hc_solution.fitness > best.fitness) {
            best = hc_solution;
        }

        // Fase 2: Evolutivo com alta mutação para diversificar
        Solution ea_solution = evolutionary_algorithm(30, 50, 0.7, 0.2, 1, 1, prob);
        
        if (ea_solution.fitness > best.fitness) {
            best = ea_solution;
        }
    }

    return best;
}

// Híbrido 3: Algoritmo Memético (EA + busca local em melhores indivíduos)
// Combina exploração global da população com exploração local nos melhores
Solution hybrid3(int pop_size, int generations, Problem *prob) {
    // Aloca populações
    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população aleatória
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i], prob);
    }

    copy_solution(&best, &population[0]);

    // Evolução com refinamento local
    for (int gen = 0; gen < generations; gen++) {
        // Atualiza melhor global
        for (int i = 0; i < pop_size; i++) {
            if (population[i].fitness > best.fitness) {
                copy_solution(&best, &population[i]);
            }
        }

        // Gera nova geração com operadores padrão
        for (int i = 0; i < pop_size; i++) {
            // Seleção por torneio
            int p1 = tournament_selection(population, pop_size, 3);
            int p2 = tournament_selection(population, pop_size, 3);

            // Recombinação uniforme (80% probabilidade)
            if ((double)rand() / RAND_MAX < 0.8) {
                uniform_crossover(&population[p1], &population[p2], &new_population[i], prob);
            } else {
                copy_solution(&new_population[i], &population[p1]);
            }

            // Mutação swap (10% probabilidade)
            if ((double)rand() / RAND_MAX < 0.1) {
                swap_mutation(&new_population[i], prob);
            }

            // Refinamento local nos 25% melhores indivíduos (aspecto memético)
            if (i < pop_size / 4) {
                Solution refined = new_population[i];
                Solution neighbor;
                
                // 10 iterações de busca local
                for (int local_iter = 0; local_iter < 10; local_iter++) {
                    neighborhood1(&refined, &neighbor, prob);
                    if (neighbor.fitness > refined.fitness) {
                        refined = neighbor;
                    }
                }
                
                // Mantém refinamento se melhorou
                if (refined.fitness > new_population[i].fitness) {
                    new_population[i] = refined;
                }
            }
        }

        // Elitismo: melhor solução sempre passa
        copy_solution(&new_population[0], &best);

        // Troca gerações
        Solution *temp = population;
        population = new_population;
        new_population = temp;
    }

    free(population);
    free(new_population);

    return best;
}