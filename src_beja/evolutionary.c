#include <stdlib.h>
#include <string.h>
#include "evolutionary.h"
#include "utils.h"

// Seleção por torneio: escolhe melhor entre tournament_size candidatos aleatórios
// Favorece indivíduos com melhor fitness sem ser determinístico
int tournament_selection(Solution *pop, int pop_size, int tournament_size) {
    int best = rand() % pop_size; // Primeiro candidato aleatório

    // Compara com outros tournament_size-1 candidatos
    for (int i = 1; i < tournament_size; i++) {
        int candidate = rand() % pop_size;
        if (pop[candidate].fitness > pop[best].fitness) {
            best = candidate;
        }
    }

    return best;
}

// Seleção por roleta: probabilidade proporcional ao fitness ajustado
// Usa shift para garantir probabilidades positivas
int roulette_selection(Solution *pop, int pop_size) {
    double total_fitness = 0.0;
    double min_fitness = INF;

    // Encontra fitness mínimo
    for (int i = 0; i < pop_size; i++) {
        if (pop[i].fitness < min_fitness) {
            min_fitness = pop[i].fitness;
        }
    }

    // Calcula soma total com shift para garantir valores positivos
    for (int i = 0; i < pop_size; i++) {
        total_fitness += (pop[i].fitness - min_fitness + 1.0);
    }

    // Gira a roleta
    double r = ((double)rand() / RAND_MAX) * total_fitness;
    double sum = 0.0;

    // Encontra indivíduo correspondente ao valor sorteado
    for (int i = 0; i < pop_size; i++) {
        sum += (pop[i].fitness - min_fitness + 1.0);
        if (sum >= r) {
            return i;
        }
    }

    return pop_size - 1; // Fallback para último indivíduo
}

// Repara solução garantindo exatamente m pontos selecionados
// Adiciona ou remove pontos aleatoriamente conforme necessário
static void repair_solution(Solution *s, Problem *prob) {
    // Adiciona pontos se faltarem
    while (s->num_selected < prob->m) {
        int pos = rand() % prob->C;
        if (!s->selected[pos]) {
            s->selected[pos] = 1;
            s->num_selected++;
        }
    }

    // Remove pontos se houver excesso
    while (s->num_selected > prob->m) {
        int pos = rand() % prob->C;
        if (s->selected[pos]) {
            s->selected[pos] = 0;
            s->num_selected--;
        }
    }
}

// Recombinação uniforme: cada gene (ponto) herdado aleatoriamente de p1 ou p2
// Gera alta diversidade mas pode criar soluções inválidas (requer reparo)
void uniform_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob) {
    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    // Para cada posição, escolhe aleatoriamente de qual pai herdar
    for (int i = 0; i < prob->C; i++) {
        child->selected[i] = (rand() % 2 == 0) ? p1->selected[i] : p2->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    // Repara para garantir exatamente m pontos
    repair_solution(child, prob);
    child->fitness = calculate_fitness(child, prob);
}

// Recombinação de um ponto: filho herda início de p1 e resto de p2
// Mantém mais estrutura dos pais mas pode criar soluções inválidas
void one_point_crossover(Solution *p1, Solution *p2, Solution *child, Problem *prob) {
    int point = rand() % prob->C; // Ponto de corte aleatório

    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    // Herda genes antes do ponto de p1
    for (int i = 0; i < point; i++) {
        child->selected[i] = p1->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    // Herda genes depois do ponto de p2
    for (int i = point; i < prob->C; i++) {
        child->selected[i] = p2->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    repair_solution(child, prob);
    child->fitness = calculate_fitness(child, prob);
}

// Mutação por troca: remove um ponto selecionado e adiciona um não selecionado
// Mantém número de pontos selecionados (não precisa reparo)
void swap_mutation(Solution *s, Problem *prob) {
    int sel = -1, unsel = -1;
    int attempts = 0;

    // Encontra ponto selecionado
    while (sel == -1 && attempts < 100) {
        int pos = rand() % prob->C;
        if (s->selected[pos]) sel = pos;
        attempts++;
    }

    // Encontra ponto não selecionado
    attempts = 0;
    while (unsel == -1 && attempts < 100) {
        int pos = rand() % prob->C;
        if (!s->selected[pos]) unsel = pos;
        attempts++;
    }

    // Realiza troca
    if (sel != -1 && unsel != -1) {
        s->selected[sel] = 0;
        s->selected[unsel] = 1;
        s->fitness = calculate_fitness(s, prob);
    }
}

// Mutação por inversão: inverte ordem de parte dos pontos selecionados
// Gera permutação diferente dos mesmos pontos
void inversion_mutation(Solution *s, Problem *prob) {
    int selected_indices[MAX_CANDIDATES];
    int count = 0;
    
    // Coleta índices dos pontos selecionados
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) {
            selected_indices[count++] = i;
        }
    }

    if (count < 2) return; // Precisa de pelo menos 2 pontos

    // Escolhe segmento aleatório para inverter
    int pos1 = rand() % count;
    int pos2 = rand() % count;
    
    if (pos1 > pos2) {
        int temp = pos1;
        pos1 = pos2;
        pos2 = temp;
    }

    // Inverte segmento
    while (pos1 < pos2) {
        int temp = selected_indices[pos1];
        selected_indices[pos1] = selected_indices[pos2];
        selected_indices[pos2] = temp;
        pos1++;
        pos2--;
    }

    // Reconstrói solução com ordem invertida
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;
    
    for (int i = 0; i < count; i++) {
        s->selected[selected_indices[i]] = 1;
        s->num_selected++;
    }

    s->fitness = calculate_fitness(s, prob);
}

// Algoritmo evolutivo completo: população + seleção + recombinação + mutação + elitismo
// Configurável em todos os parâmetros e operadores
Solution evolutionary_algorithm(int pop_size, int generations, double cross_prob,
    double mut_prob, int selection_type, int crossover_type, Problem *prob) {

    // Aloca populações (atual e próxima geração)
    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população com soluções aleatórias
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i], prob);
    }

    copy_solution(&best, &population[0]);

    // Loop principal: evolui por generations gerações
    for (int gen = 0; gen < generations; gen++) {
        // Atualiza melhor solução global
        for (int i = 0; i < pop_size; i++) {
            if (population[i].fitness > best.fitness) {
                copy_solution(&best, &population[i]);
            }
        }

        // Cria nova geração
        for (int i = 0; i < pop_size; i++) {
            int p1, p2;

            // Seleciona dois pais conforme método escolhido
            if (selection_type == 1) {
                p1 = tournament_selection(population, pop_size, 3);
                p2 = tournament_selection(population, pop_size, 3);
            } else {
                p1 = roulette_selection(population, pop_size);
                p2 = roulette_selection(population, pop_size);
            }

            // Aplica recombinação com probabilidade cross_prob
            if ((double)rand() / RAND_MAX < cross_prob) {
                if (crossover_type == 1) {
                    uniform_crossover(&population[p1], &population[p2], &new_population[i], prob);
                } else {
                    one_point_crossover(&population[p1], &population[p2], &new_population[i], prob);
                }
            } else {
                // Sem recombinação: copia p1 diretamente
                copy_solution(&new_population[i], &population[p1]);
            }

            // Aplica mutação com probabilidade mut_prob
            if ((double)rand() / RAND_MAX < mut_prob) {
                // Escolhe tipo de mutação aleatoriamente
                if (rand() % 2 == 0) {
                    swap_mutation(&new_population[i], prob);
                } else {
                    inversion_mutation(&new_population[i], prob);
                }
            }
        }

        // Elitismo: garante que melhor solução passa para próxima geração
        copy_solution(&new_population[0], &best);

        // Troca populações (new_population vira population)
        Solution *temp = population;
        population = new_population;
        new_population = temp;
    }

    free(population);
    free(new_population);

    return best;
}