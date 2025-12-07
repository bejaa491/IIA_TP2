#include <stdlib.h>
#include <stdbool.h> // Para usar 'bool'
#include "hillclimbing.h"
#include "utils.h"
// Encontra aleatoriamente o índice de um ponto selecionado (IN)
// Retorna -1 se nenhum ponto for selecionado.
static int find_random_selected(const Solution *s, int C) {
    // Melhor abordagem seria listar todos os selecionados e escolher um
    // Mas para manter o espírito de aleatoriedade imediata (com limite)
    // mantemos a lógica original.
    int attempts = 0;
    while (attempts < 100) {
        int pos = rand() % C;
        if (s->selected[pos]) return pos;
        attempts++;
    }
    return -1;
}

// Encontra aleatoriamente o índice de um ponto não selecionado (OUT)
// Retorna -1 se todos os pontos forem selecionados.
static int find_random_unselected(const Solution *s, int C) {
    int attempts = 0;
    while (attempts < 100) {
        int pos = rand() % C;
        if (!s->selected[pos]) return pos;
        attempts++;
    }
    return -1;
}

// ==========================================================
// VIZINHANÇAS (OPERADORES DE MOVIMENTO)
// ==========================================================

// Vizinhança 1 (Swap 1-1): Troca 1 ponto selecionado por 1 não selecionado
// Movimento mais fino para busca local detalhada.
void neighborhood1(const Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    // Encontra os índices para o swap
    int sel = find_random_selected(neighbor, prob->C);
    int unsel = find_random_unselected(neighbor, prob->C);

    if (sel != -1 && unsel != -1) {
        // Executa a troca (Swap)
        neighbor->selected[sel] = 0;
        neighbor->selected[unsel] = 1;
        // Recalcula o fitness do vizinho
        neighbor->fitness = calculate_fitness(neighbor, prob);
    } 
    // Se não encontrou pontos (ex: C é muito pequeno ou aleatoriedade falhou), 
    // o neighbor é uma cópia exata do current e o fitness não muda.
}

// Vizinhança 2 (Swap 2-2 ou 1-1): Troca 2 pontos selecionados por 2 não selecionados (se prob->m >= 2)
// Movimento mais amplo para exploração agressiva.
void neighborhood2(const Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    // Determina o número de trocas (swaps)
    int swap_count = (prob->m >= 2) ? 2 : 1;

    // Realiza as trocas
    for (int tries = 0; tries < swap_count; tries++) {
        int sel = find_random_selected(neighbor, prob->C);
        int unsel = find_random_unselected(neighbor, prob->C);

        if (sel != -1 && unsel != -1) {
            neighbor->selected[sel] = 0;
            neighbor->selected[unsel] = 1;
        }
    }

    // Recalcula o fitness uma única vez, após todos os swaps
    neighbor->fitness = calculate_fitness(neighbor, prob);
}

// ==========================================================
// ALGORITMOS DE BUSCA LOCAL
// ==========================================================

// Hill Climbing simples: busca local que aceita apenas melhorias (ou iguais).
// Estratégia: explora vizinhança até atingir máximo de iterações.
Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob) {
    Solution current, best, neighbor;

    // Inicializa com solução aleatória
    random_solution(&current, prob);
    copy_solution(&best, &current);

    // Loop principal de busca
    for (int iter = 0; iter < max_iter; iter++) {
        // 1. Gera um vizinho
        if (neighborhood_type == 1) {
            neighborhood1(&current, &neighbor, prob);
        } else {
            neighborhood2(&current, &neighbor, prob);
        }

        // 2. Critério de Aceitação (Melhoria ou Igualdade)
        // Aceitar >= ajuda a evitar estagnação em platôs.
        if (neighbor.fitness >= current.fitness) {
            // Mover para o vizinho
            copy_solution(&current, &neighbor);

            // 3. Atualiza a melhor solução global
            if (current.fitness > best.fitness) {
                copy_solution(&best, &current);
            }
        }
        // Se neighbor.fitness < current.fitness, o movimento é rejeitado e a busca continua com 'current'
    }

    return best;
}

// Hill Climbing com Múltiplos Restarts: diversifica a busca para encontrar o ótimo global.
Solution hill_climbing_restart(int max_iter, int neighborhood_type, int num_restarts, Problem *prob) {
    Solution best_overall;
    // Inicializa a melhor solução geral com uma solução aleatória inicial
    random_solution(&best_overall, prob); 

    // Itera pelo número de restarts
    for (int restart = 0; restart < num_restarts; restart++) {
        // Cada restart recebe uma fração das iterações totais
        int iterations_per_restart = max_iter / num_restarts;
        
        // Executa o Hill Climbing simples a partir de um novo ponto aleatório
        Solution current_run_best = hill_climbing(iterations_per_restart, neighborhood_type, prob);
        
        // Atualiza a melhor solução global
        if (current_run_best.fitness > best_overall.fitness) {
            copy_solution(&best_overall, &current_run_best);
        }
    }

    return best_overall;
}