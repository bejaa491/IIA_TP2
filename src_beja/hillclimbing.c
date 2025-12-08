#include <stdlib.h>
#include <stdbool.h>
#include "hillclimbing.h"
#include "utils.h"

/* hillclimbing.c
   Implementa duas vizinhanças simples e variantes de Hill Climbing
   Comentários adicionados para facilitar leitura e manutenção.
*/

static int sample_from_list(const int *list, int size) {
    // Retorna índice aleatório de uma lista (ou -1 se lista vazia)
    if (size == 0) return -1;
    return list[rand() % size];
}

static int find_random_selected(const Solution *s, int C) {
    // Constrói lista de índices selecionados e escolhe um aleatoriamente
    int selected_list[C];
    int count = 0;
    for (int i = 0; i < C; i++)
        if (s->selected[i]) selected_list[count++] = i;
    return sample_from_list(selected_list, count);
}

static int find_random_unselected(const Solution *s, int C) {
    // Constrói lista de índices não selecionados e escolhe um aleatoriamente
    int unselected_list[C];
    int count = 0;
    for (int i = 0; i < C; i++)
        if (!s->selected[i]) unselected_list[count++] = i;
    return sample_from_list(unselected_list, count);
}

void neighborhood1(const Solution *current, Solution *neighbor, Problem *prob) {
    // Vizinhança 1: troca simples de 1 selecionado por 1 não selecionado
    // Faz cópia do estado atual, aplica uma troca aleatória e recalcula fitness
    copy_solution(neighbor, current);

    int sel = find_random_selected(neighbor, prob->C);
    int unsel = find_random_unselected(neighbor, prob->C);

    if (sel != -1 && unsel != -1) {
        neighbor->selected[sel] = 0;
        neighbor->selected[unsel] = 1;
        neighbor->fitness = calculate_fitness(neighbor, prob);
    }
}

void neighborhood2(const Solution *current, Solution *neighbor, Problem *prob) {
    // Vizinhança 2: realiza 2 trocas quando possível (maior mudança)
    // Mantém cópia da solução e tenta realizar 'swaps' vezes a troca
    copy_solution(neighbor, current);

    int swaps = (prob->m >= 2) ? 2 : 1;

    for (int i = 0; i < swaps; i++) {
        int sel = find_random_selected(neighbor, prob->C);
        int unsel = find_random_unselected(neighbor, prob->C);

        if (sel == -1 || unsel == -1) break;

        neighbor->selected[sel] = 0;
        neighbor->selected[unsel] = 1;
    }

    neighbor->fitness = calculate_fitness(neighbor, prob);
}

Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob) {
    // Hill Climbing simples com política de aceitar melhor ou igual (>=)
    // Gera solução inicial aleatória e mantém melhor encontrada
    Solution current, best, neighbor;

    random_solution(&current, prob);
    copy_solution(&best, &current);

    for (int iter = 0; iter < max_iter; iter++) {
        // Gera vizinho segundo o tipo pedido
        if (neighborhood_type == 1)
            neighborhood1(&current, &neighbor, prob);
        else
            neighborhood2(&current, &neighbor, prob);

        // Aceita progresso ou plateau (neighbor.fitness >= current.fitness)
        if (neighbor.fitness >= current.fitness) {
            copy_solution(&current, &neighbor);
            if (current.fitness > best.fitness)
                copy_solution(&best, &current);
        }

        // Estratégia simples de escape: a cada 200 iterações faz uma troca aleatória
        // para introduzir diversidade e escapar de platôs/óptimos locais
        if (iter % 200 == 0 && iter > 0) {
            int sel = find_random_selected(&current, prob->C);
            int unsel = find_random_unselected(&current, prob->C);
            if (sel != -1 && unsel != -1) {
                current.selected[sel] = 0;
                current.selected[unsel] = 1;
                current.fitness = calculate_fitness(&current, prob);
            }
        }
    }

    return best;
}

Solution hill_climbing_restart(int max_iter, int neighborhood_type, int num_restarts, Problem *prob) {
    // Executa múltiplos restarts: divide budget de iterações entre restarts
    // e escolhe o melhor resultado dentre as execuções independentes.
    Solution best_overall;
    random_solution(&best_overall, prob);

    for (int r = 0; r < num_restarts; r++) {
        int it = max_iter / num_restarts;
        Solution run_best = hill_climbing(it, neighborhood_type, prob);
        if (run_best.fitness > best_overall.fitness)
            copy_solution(&best_overall, &run_best);
    }

    return best_overall;
}
