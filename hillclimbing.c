#include <stdlib.h>
#include "hillclimbing.h"
#include "utils.h"

// Vizinhança 1: Troca um ponto selecionado por um não selecionado
void neighborhood1(Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    // Encontra um ponto selecionado e um não selecionado
    int selected_pos = -1, unselected_pos = -1;

    for (int i = 0; i < prob->C && selected_pos == -1; i++) {
        if (neighbor->selected[i]) selected_pos = i;
    }

    for (int i = 0; i < prob->C && unselected_pos == -1; i++) {
        if (!neighbor->selected[i]) unselected_pos = i;
    }

    // Troca
    if (selected_pos != -1 && unselected_pos != -1) {
        neighbor->selected[selected_pos] = 0;
        neighbor->selected[unselected_pos] = 1;
        neighbor->fitness = calculate_fitness(neighbor, prob);
    }
}

// Vizinhança 2: Troca dois pontos selecionados por dois não selecionados
void neighborhood2(Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    int count = 0;
    int swap_count = (prob->m >= 2) ? 2 : 1;

    for (int tries = 0; tries < swap_count; tries++) {
        int sel = -1, unsel = -1;

        // Encontra ponto selecionado aleatório
        int attempts = 0;
        while (sel == -1 && attempts < 100) {
            int pos = rand() % prob->C;
            if (neighbor->selected[pos]) sel = pos;
            attempts++;
        }

        // Encontra ponto não selecionado aleatório
        attempts = 0;
        while (unsel == -1 && attempts < 100) {
            int pos = rand() % prob->C;
            if (!neighbor->selected[pos]) unsel = pos;
            attempts++;
        }

        if (sel != -1 && unsel != -1) {
            neighbor->selected[sel] = 0;
            neighbor->selected[unsel] = 1;
        }
    }

    neighbor->fitness = calculate_fitness(neighbor, prob);
}

// Hill Climbing
Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob) {
    Solution current, best, neighbor;

    random_solution(&current, prob);
    copy_solution(&best, &current);

    for (int iter = 0; iter < max_iter; iter++) {
        if (neighborhood_type == 1) {
            neighborhood1(&current, &neighbor, prob);
        } else {
            neighborhood2(&current, &neighbor, prob);
        }

        if (neighbor.fitness >= current.fitness) {
            copy_solution(&current, &neighbor);

            if (current.fitness > best.fitness) {
                copy_solution(&best, &current);
            }
        }
    }

    return best;
}