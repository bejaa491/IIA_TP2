#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MAX_CANDIDATES 500
#define MAX_RUNS 10
#define INF 1e9

// Estrutura para representar uma solução
typedef struct {
    int selected[MAX_CANDIDATES];  // 1 se o ponto está selecionado, 0 caso contrário
    int num_selected;              // Número de pontos selecionados
    double fitness;                // Valor da função objetivo
} Solution;

// Estrutura para os dados do problema
typedef struct {
    int C;                                    // Total de candidaturas
    int m;                                    // Número de pontos a selecionar
    double dist[MAX_CANDIDATES][MAX_CANDIDATES]; // Matriz de distâncias
} Problem;

// Variáveis globais
Problem prob;

// ============= FUNÇÕES AUXILIARES =============

// Lê o ficheiro de entrada
int read_file(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Erro ao abrir ficheiro %s\n", filename);
        return 0;
    }

    fscanf(f, "%d %d", &prob.C, &prob.m);

    for (int i = 0; i < prob.C; i++) {
        for (int j = 0; j < prob.C; j++) {
            fscanf(f, "%lf", &prob.dist[i][j]);
        }
    }

    fclose(f);
    return 1;
}

// Calcula a distância média de uma solução
double calculate_fitness(Solution *s) {
    if (s->num_selected != prob.m) {
        return -INF; // Solução inválida
    }

    double sum = 0.0;
    int count = 0;

    // Converte array binário em lista de índices
    int points[MAX_CANDIDATES];
    int idx = 0;
    for (int i = 0; i < prob.C; i++) {
        if (s->selected[i]) {
            points[idx++] = i;
        }
    }

    // Calcula soma das distâncias
    for (int i = 0; i < prob.m - 1; i++) {
        for (int j = i + 1; j < prob.m; j++) {
            sum += prob.dist[points[i]][points[j]];
            count++;
        }
    }

    return sum / prob.m;
}

// Cria uma solução aleatória válida
void random_solution(Solution *s) {
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;

    // Seleciona m pontos aleatórios diferentes
    while (s->num_selected < prob.m) {
        int pos = rand() % prob.C;
        if (!s->selected[pos]) {
            s->selected[pos] = 1;
            s->num_selected++;
        }
    }

    s->fitness = calculate_fitness(s);
}

// Copia uma solução
void copy_solution(Solution *dest, Solution *src) {
    memcpy(dest->selected, src->selected, sizeof(src->selected));
    dest->num_selected = src->num_selected;
    dest->fitness = src->fitness;
}

// Imprime solução
void print_solution(Solution *s) {
    printf("Pontos selecionados: ");
    for (int i = 0; i < prob.C; i++) {
        if (s->selected[i]) printf("%d ", i);
    }
    printf("\nFitness: %.2f\n", s->fitness);
}

// ============= TREPA-COLINAS =============

// Vizinhança 1: Troca um ponto selecionado por um não selecionado
void neighborhood1(Solution *current, Solution *neighbor) {
    copy_solution(neighbor, current);

    // Encontra um ponto selecionado e um não selecionado
    int selected_pos = -1, unselected_pos = -1;

    for (int i = 0; i < prob.C && selected_pos == -1; i++) {
        if (neighbor->selected[i]) selected_pos = i;
    }

    for (int i = 0; i < prob.C && unselected_pos == -1; i++) {
        if (!neighbor->selected[i]) unselected_pos = i;
    }

    // Troca
    if (selected_pos != -1 && unselected_pos != -1) {
        neighbor->selected[selected_pos] = 0;
        neighbor->selected[unselected_pos] = 1;
        neighbor->fitness = calculate_fitness(neighbor);
    }
}

// Vizinhança 2: Troca dois pontos selecionados por dois não selecionados
void neighborhood2(Solution *current, Solution *neighbor) {
    copy_solution(neighbor, current);

    int count = 0;
    int swap_count = (prob.m >= 2) ? 2 : 1;

    for (int tries = 0; tries < swap_count; tries++) {
        int sel = -1, unsel = -1;

        // Encontra ponto selecionado aleatório
        int attempts = 0;
        while (sel == -1 && attempts < 100) {
            int pos = rand() % prob.C;
            if (neighbor->selected[pos]) sel = pos;
            attempts++;
        }

        // Encontra ponto não selecionado aleatório
        attempts = 0;
        while (unsel == -1 && attempts < 100) {
            int pos = rand() % prob.C;
            if (!neighbor->selected[pos]) unsel = pos;
            attempts++;
        }

        if (sel != -1 && unsel != -1) {
            neighbor->selected[sel] = 0;
            neighbor->selected[unsel] = 1;
        }
    }

    neighbor->fitness = calculate_fitness(neighbor);
}

// Hill Climbing
Solution hill_climbing(int max_iter, int neighborhood_type) {
    Solution current, best, neighbor;

    random_solution(&current);
    copy_solution(&best, &current);

    for (int iter = 0; iter < max_iter; iter++) {
        if (neighborhood_type == 1) {
            neighborhood1(&current, &neighbor);
        } else {
            neighborhood2(&current, &neighbor);
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

// ============= ALGORITMO EVOLUTIVO =============

typedef struct {
    Solution *population;
    int pop_size;
    double crossover_prob;
    double mutation_prob;
} EvolutionaryAlgorithm;

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
void uniform_crossover(Solution *p1, Solution *p2, Solution *child) {
    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    // Combina genes dos pais
    for (int i = 0; i < prob.C; i++) {
        if (rand() % 2 == 0) {
            child->selected[i] = p1->selected[i];
        } else {
            child->selected[i] = p2->selected[i];
        }
        if (child->selected[i]) child->num_selected++;
    }

    // Repara solução
    while (child->num_selected < prob.m) {
        int pos = rand() % prob.C;
        if (!child->selected[pos]) {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }

    while (child->num_selected > prob.m) {
        int pos = rand() % prob.C;
        if (child->selected[pos]) {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child);
}

// Recombinação de um ponto
void one_point_crossover(Solution *p1, Solution *p2, Solution *child) {
    int point = rand() % prob.C;

    memset(child->selected, 0, sizeof(child->selected));
    child->num_selected = 0;

    for (int i = 0; i < point; i++) {
        child->selected[i] = p1->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    for (int i = point; i < prob.C; i++) {
        child->selected[i] = p2->selected[i];
        if (child->selected[i]) child->num_selected++;
    }

    // Repara
    while (child->num_selected < prob.m) {
        int pos = rand() % prob.C;
        if (!child->selected[pos]) {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }

    while (child->num_selected > prob.m) {
        int pos = rand() % prob.C;
        if (child->selected[pos]) {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child);
}

// Mutação por troca
void swap_mutation(Solution *s) {
    int sel = -1, unsel = -1;

    for (int i = 0; i < prob.C && sel == -1; i++) {
        if (s->selected[rand() % prob.C]) sel = rand() % prob.C;
    }

    for (int i = 0; i < prob.C && unsel == -1; i++) {
        if (!s->selected[rand() % prob.C]) unsel = rand() % prob.C;
    }

    if (sel != -1 && unsel != -1) {
        s->selected[sel] = 0;
        s->selected[unsel] = 1;
        s->fitness = calculate_fitness(s);
    }
}

// Algoritmo evolutivo
Solution evolutionary_algorithm(int pop_size, int generations, double cross_prob,
                                double mut_prob, int selection_type, int crossover_type) {
    Solution *population = malloc(pop_size * sizeof(Solution));
    Solution *new_population = malloc(pop_size * sizeof(Solution));
    Solution best;

    // Inicializa população
    for (int i = 0; i < pop_size; i++) {
        random_solution(&population[i]);
    }

    copy_solution(&best, &population[0]);

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
                    uniform_crossover(&population[p1], &population[p2], &new_population[i]);
                } else {
                    one_point_crossover(&population[p1], &population[p2], &new_population[i]);
                }
            } else {
                copy_solution(&new_population[i], &population[p1]);
            }

            if ((double)rand() / RAND_MAX < mut_prob) {
                swap_mutation(&new_population[i]);
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

// ============= HÍBRIDOS =============

// Híbrido 1: Evolutivo + Hill Climbing nas melhores soluções
Solution hybrid1(int pop_size, int generations) {
    Solution best = evolutionary_algorithm(pop_size, generations, 0.8, 0.1, 1, 1);

    // Aplica hill climbing na melhor solução
    Solution current = best;
    for (int iter = 0; iter < 100; iter++) {
        Solution neighbor;
        neighborhood1(&current, &neighbor);

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
Solution hybrid2(int iterations) {
    Solution best;
    random_solution(&best);

    for (int restart = 0; restart < 5; restart++) {
        Solution current = hill_climbing(iterations / 5, 1);

        if (current.fitness > best.fitness) {
            best = current;
        }

        // Usa evolutivo para diversificar
        Solution evolved = evolutionary_algorithm(20, 50, 0.7, 0.2, 1, 1);
        if (evolved.fitness > best.fitness) {
            best = evolved;
        }
    }

    return best;
}

// ============= MAIN =============

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Uso: %s <ficheiro_entrada>\n", argv[0]);
        return 1;
    }

    if (!read_file(argv[1])) {
        return 1;
    }

    printf("Problema carregado: C=%d, m=%d\n\n", prob.C, prob.m);

    // Teste Hill Climbing
    printf("=== HILL CLIMBING ===\n");
    Solution hc_best = hill_climbing(1000, 1);
    printf("Melhor solução HC:\n");
    print_solution(&hc_best);

    // Teste Evolutivo
    printf("\n=== EVOLUTIVO ===\n");
    Solution ea_best = evolutionary_algorithm(50, 100, 0.8, 0.1, 1, 1);
    printf("Melhor solução EA:\n");
    print_solution(&ea_best);

    // Teste Híbrido 1
    printf("\n=== HÍBRIDO 1 ===\n");
    Solution h1_best = hybrid1(50, 100);
    printf("Melhor solução H1:\n");
    print_solution(&h1_best);

    // Teste Híbrido 2
    printf("\n=== HÍBRIDO 2 ===\n");
    Solution h2_best = hybrid2(1000);
    printf("Melhor solução H2:\n");
    print_solution(&h2_best);

    return 0;
}