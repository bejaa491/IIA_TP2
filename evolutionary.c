#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "evolutionary.h"
#include "utils.h"

// Seleção por torneio
int tournament_selection(Solution *pop, int pop_size, int tournament_size) {
    int best = rand() % pop_size;

    for (int i = 1; i < tournament_size; i++) {
        int candidate = rand() % pop_size;
        if (pop[candidate].fitness > pop[best].fitness) {
            best = candidate;
        }
    }

    return best;
}

// Seleção por roleta
int roulette_selection(Solution *pop, int pop_size) {
    double total_fitness = 0.0;
    double min_fitness = INF;

    for (int i = 0; i < pop_size; i++) {
        if (pop[i].fitness < min_fitness) {
            min_fitness = pop[i].fitness;
        }
    }

    for (int i = 0; i < pop_size; i++) {
        total_fitness += (pop[i].fitness - min_fitness + 1.0);
    }

    double r = ((double)rand() / RAND_MAX) * total_fitness;
    double sum = 0.0;

    for (int i = 0; i < pop_size; i++) {
        sum += (pop[i].fitness - min_fitness + 1.0);
        if (sum >= r) {
            return i;
        }
    }

    return pop_size - 1;
}

// Recombinação uniforme
void uniform_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob) {
    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    // Combina genes dos pais
    for (int i = 0; i < prob->C; i++) {
        if (rand() % 2 == 0) {
            child->selected[i] = p1->selected[i];
        } else {
            child->selected[i] = p2->selected[i];
        }
        if (child->selected[i]) child->num_selected++;
    }

    // Repara solução
    while (child->num_selected < prob->m) {
        int pos = rand() % prob->C;
        if (!child->selected[pos]) {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }

    while (child->num_selected > prob->m) {
        int pos = rand() % prob->C;
        if (child->selected[pos]) {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child, prob);
}

// Recombinação de um ponto
void one_point_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob) {
    int point = rand() % prob->C;

    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    for (int i = 0; i < point; i++) {
        child->selected[i] = p1->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    for (int i = point; i < prob->C; i++) {
        child->selected[i] = p2->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    // Repara
    while (child->num_selected < prob->m) {
        int pos = rand() % prob->C;
        if (!child->selected[pos]) {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }

    while (child->num_selected > prob->m) {
        int pos = rand() % prob->C;
        if (child->selected[pos]) {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child, prob);
}

// Mutação por troca (CORRIGIDA)
void swap_mutation(Solution *s, Problem *prob) {
    int sel = -1, unsel = -1;

    // Encontra ponto selecionado aleatório
    int attempts = 0;
    while (sel == -1 && attempts < 100) {
        int pos = rand() % prob->C;
        if (s->selected[pos]) {
            sel = pos;
        }
        attempts++;
    }

    // Encontra ponto não selecionado aleatório
    attempts = 0;
    while (unsel == -1 && attempts < 100) {
        int pos = rand() % prob->C;
        if (!s->selected[pos]) {
            unsel = pos;
        }
        attempts++;
    }

    if (sel != -1 && unsel != -1) {
        s->selected[sel] = 0;
        s->selected[unsel] = 1;
        s->fitness = calculate_fitness(s, prob);
    }
}

//  Mutação por inversão
void inversion_mutation(Solution *s, Problem *prob) {
    // Cria array com índices dos pontos selecionados
    int selected_indices[MAX_CANDIDATES];
    int count = 0;
    
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) {
            selected_indices[count++] = i;
        }
    }

    if (count < 2) return; // Precisa de pelo menos 2 elementos

    // Escolhe dois pontos aleatórios para inverter
    int pos1 = rand() % count;
    int pos2 = rand() % count;
    
    if (pos1 > pos2) {
        int temp = pos1;
        pos1 = pos2;
        pos2 = temp;
    }

    // Inverte a subsequência
    while (pos1 < pos2) {
        int temp = selected_indices[pos1];
        selected_indices[pos1] = selected_indices[pos2];
        selected_indices[pos2] = temp;
        pos1++;
        pos2--;
    }

    // Reconstrói a solução
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;
    
    for (int i = 0; i < count; i++) {
        s->selected[selected_indices[i]] = 1;
        s->num_selected++;
    }

    s->fitness = calculate_fitness(s, prob);
}

// helper: conta fitness distintos (epsilon)
static int count_unique_fitness(Solution *pop, int pop_size) {
    const double eps = 1e-6;
    int unique = 0;
    for (int i = 0; i < pop_size; ++i) {
        int seen = 0;
        for (int j = 0; j < i; ++j) {
            if (fabs(pop[i].fitness - pop[j].fitness) < eps) { seen = 1; break; }
        }
        if (!seen) ++unique;
    }
    return unique;
}

// Algoritmo evolutivo
Solution evolutionary_algorithm(int pop_size, int generations, double cross_prob,
    double mut_prob, int selection_type, int crossover_type,
    Problem *prob) {

    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i], prob);
    }

    copy_solution(&best, &population[0]);

#ifdef DEBUG_ALGO
    printf("EA: init unique fitness = %d\n", count_unique_fitness(population, pop_size));
#endif

    for (int gen = 0; gen < generations; gen++) {
        // Encontra melhor
        for (int i = 0; i < pop_size; i++) {
            if (population[i].fitness > best.fitness) {
                copy_solution(&best, &population[i]);
            }
        }

        // Nova geração
        for (int i = 0; i < pop_size; i++) {
            int p1, p2;

            if (selection_type == 1) {
                p1 = tournament_selection(population, pop_size, 3);
                p2 = tournament_selection(population, pop_size, 3);
            } else {
                p1 = roulette_selection(population, pop_size);
                p2 = roulette_selection(population, pop_size);
            }

            if ((double)rand() / RAND_MAX < cross_prob) {
                if (crossover_type == 1) {
                    uniform_crossover(&population[p1], &population[p2], &new_population[i], prob);
                } else {
                    one_point_crossover(&population[p1], &population[p2], &new_population[i], prob);
                }
            } else {
                copy_solution(&new_population[i], &population[p1]);
            }

            if ((double)rand() / RAND_MAX < mut_prob) {
                // Alterna entre os dois tipos de mutação
                if (rand() % 2 == 0) {
                    swap_mutation(&new_population[i], prob);
                } else {
                    inversion_mutation(&new_population[i], prob);
                }
            }
        }

        // depois de preencher new_population (antes do swap)
#ifdef DEBUG_ALGO
        int unique_before = count_unique_fitness(population, pop_size);
        int unique_after  = count_unique_fitness(new_population, pop_size);
        printf("EA gen %d: unique_before=%d unique_after=%d best=%.4f\n",
               gen, unique_before, unique_after, best.fitness);
#endif

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