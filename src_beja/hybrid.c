#include "hybrid.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "utils.h"
#include <stdlib.h>

// ==========================================================
// HÍBRIDO 1
// EA GLOBAL  →  HC LOCAL
//
// Ideia:
// 1. O EA explora o espaço de busca para encontrar uma boa região.
// 2. O Hill Climbing faz intensificação sobre a melhor solução encontrada.
// ==========================================================
Solution hybrid1(int pop_size, int generations, Problem *prob) {
    // Execução do Algoritmo Evolutivo completo
    Solution best = evolutionary_algorithm(pop_size, generations, 0.8, 0.1, 1, 1, prob);

    // Refina a melhor solução usando 200 iterações de Hill Climbing simples
    Solution refined = best;
    Solution neighbor;
    
    for (int iter = 0; iter < 200; iter++) {
        // Gera vizinho via vizinhança 1 (swap 1-1)
        neighborhood1(&refined, &neighbor, prob);

        // Aceita apenas melhorias
        if (neighbor.fitness > refined.fitness) {
            refined = neighbor;
        }
    }

    // Retorna a melhor entre a solução EA e sua versão refinada
    if (refined.fitness > best.fitness) {
        best = refined;
    }

    return best;
}

// ==========================================================
// HÍBRIDO 2
// Alternância HC ↔ EA
//
// Ideia:
// - HC faz intensificação (ótimos locais)
// - EA faz diversificação (escapa dos ótimos locais)
// - Múltiplos restarts garantem variedade de regiões exploradas
// ==========================================================
Solution hybrid2(int iterations, Problem *prob) {
    Solution best;
    random_solution(&best, prob);

    int num_restarts = 5;
    
    for (int restart = 0; restart < num_restarts; restart++) {

        // Fase 1: Hill Climbing para exploração local
        Solution hc_solution = hill_climbing(iterations / num_restarts, 1, prob);
        if (hc_solution.fitness > best.fitness) {
            best = hc_solution;
        }

        // Fase 2: EA com mutação mais alta para diversificação
        Solution ea_solution = evolutionary_algorithm(30, 50, 0.7, 0.2, 1, 1, prob);
        if (ea_solution.fitness > best.fitness) {
            best = ea_solution;
        }
    }

    return best;
}

// ==========================================================
// HÍBRIDO 3 — Algoritmo Memético
//
// Combina:
// - Exploração global via EA
// - Exploração local nos melhores indivíduos da população
//
// Funcionamento:
// - Executa EA normal
// - Em cada geração, aplica HC curto aos 25% melhores indivíduos
// - Garante elitismo absoluto
// ==========================================================
Solution hybrid3(int pop_size, int generations, Problem *prob) {
    // Aloca populações
    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população com soluções aleatórias
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i], prob);
    }

    // Primeiro melhor é simplesmente o primeiro indivíduo inicial
    copy_solution(&best, &population[0]);

    // Evolução com refinamento local (versão memética)
    for (int gen = 0; gen < generations; gen++) {

        // Atualiza a melhor solução global
        for (int i = 0; i < pop_size; i++) {
            if (population[i].fitness > best.fitness) {
                copy_solution(&best, &population[i]);
            }
        }

        // Cria nova geração
        for (int i = 0; i < pop_size; i++) {

            // Seleção via torneio
            int p1 = tournament_selection(population, pop_size, 3);
            int p2 = tournament_selection(population, pop_size, 3);

            // Recombinação uniforme 80%
            if ((double)rand() / RAND_MAX < 0.8) {
                uniform_crossover(&population[p1], &population[p2], &new_population[i], prob);
            } else {
                copy_solution(&new_population[i], &population[p1]);
            }

            // Mutação swap 10%
            if ((double)rand() / RAND_MAX < 0.1) {
                swap_mutation(&new_population[i], prob);
            }

            // Aplicação do refinamento local (HC curto)
            // Apenas nos 25% melhores indivíduos da geração
            if (i < pop_size / 4) {
                Solution refined = new_population[i];
                Solution neighbor;

                // 10 iterações de HC
                for (int local_iter = 0; local_iter < 10; local_iter++) {
                    neighborhood1(&refined, &neighbor, prob);
                    if (neighbor.fitness > refined.fitness) {
                        refined = neighbor;
                    }
                }

                // Mantém apenas se melhorar
                if (refined.fitness > new_population[i].fitness) {
                    new_population[i] = refined;
                }
            }
        }

        // Elitismo: a melhor solução absoluta vai sempre para a nova população
        copy_solution(&new_population[0], &best);

        // Troca populações
        Solution *temp = population;
        population = new_population;
        new_population = temp;
    }

    // Libera memória
    free(population);
    free(new_population);

    return best;
}
