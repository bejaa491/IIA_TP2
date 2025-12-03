#include "hybrid.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "utils.h"

// Híbrido 1: Evolutivo + Hill Climbing nas melhores soluções
Solution hybrid1(int pop_size, int generations, Problem *prob) {
    Solution best = evolutionary_algorithm(pop_size, generations, 0.8, 0.1, 1, 1, prob);

    // Aplica hill climbing na melhor solução
    Solution current = best;
    for (int iter = 0; iter < 100; iter++) {
        Solution neighbor;
        neighborhood1(&current, &neighbor, prob);

        if (neighbor.fitness > current.fitness) {
            current = neighbor;
        }
    }

    if (current.fitness > best.fitness) {
        best = current;
    }

    return best;
}

// Híbrido 2: Hill Climbing + Evolutivo para escapar de ótimos locais
Solution hybrid2(int iterations, Problem *prob) {
    Solution best;
    random_solution(&best, prob);

    for (int restart = 0; restart < 5; restart++) {
        Solution current = hill_climbing(iterations / 5, 1, prob);

        if (current.fitness > best.fitness) {
            best = current;
        }

        // Usa evolutivo para diversificar
        Solution evolved = evolutionary_algorithm(20, 50, 0.7, 0.2, 1, 1, prob);
        if (evolved.fitness > best.fitness) {
            best = evolved;
        }
    }

    return best;
}