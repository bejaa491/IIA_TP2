#include <stdio.h>
#include <string.h>   // memset, memcpy
#include "utils.h"    // para prob
#include "funcao.h"

// Calcula a distância média de uma solução
double calculate_fitness_old(Solution *s) {
    if (s->num_selected != prob.m) {
        return -1e9; // Solução inválida (penalização grande)
    }

    double sum = 0.0;
    int count = 0;

    int points[MAX_CANDIDATES];
    int idx = 0;

    // Converte array binário em lista de índices
    for (int i = 0; i < prob.C; i++) {
        if (s->selected[i]) {
            points[idx++] = i;
        }
    }

    // Calcula soma das distâncias em todos os pares
    for (int i = 0; i < prob.m - 1; i++) {
        for (int j = i + 1; j < prob.m; j++) {
            sum += prob.dist[points[i]][points[j]];
            count++;
        }
    }

    return (prob.m > 0) ? sum / prob.m : -1e9;
}

double calculate_fitness(Solution *sol) {
    double sum = 0.0;
    // Percorre todos os pares de pontos selecionados
    for (int i = 0; i < prob.C - 1; i++) {
        if (sol->selected[i]) { // Só se o ponto i estiver na solução
            for (int j = i + 1; j < prob.C; j++) {
                if (sol->selected[j]) { // Só se o ponto j também estiver
                    sum += prob.dist[i][j];
                }
            }
        }
    }
    // CORREÇÃO: Dividir por m (número de locais a construir) [cite: 22]
    return (prob.m > 0) ? sum / prob.m : -1e9;
}

// Cria uma solução aleatória válida
void random_solution(Solution *s) {
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;

    // Seleciona m pontos aleatórios diferentes
    while (s->num_selected < prob.m) {
        int pos = random_l_h(0, prob.C - 1);
        if (!s->selected[pos]) {
            s->selected[pos] = 1;
            s->num_selected++;
        }
    }

    s->fitness = calculate_fitness(s);
}

// Copia uma solução
void copy_solution(Solution *dest, const Solution *src) {
    memcpy(dest->selected, src->selected, sizeof(src->selected));
    dest->num_selected = src->num_selected;
    dest->fitness = src->fitness;
}

// Imprime solução
void print_solution(const Solution *s) {
    printf("Pontos selecionados: ");
    for (int i = 0; i < prob.C; i++) {
        if (s->selected[i]) printf("%d ", i + 1); // +1 para bater com e1..eC
    }
    printf("\nFitness: %.6f\n", s->fitness);
}
