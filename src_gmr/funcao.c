#include <stdio.h>
#include "funcao.h"
#include "utils.h"

#include <string.h>


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
