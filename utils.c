#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// Lê o ficheiro de entrada
int read_file(char *filename, Problem *prob) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Erro ao abrir ficheiro %s\n", filename);
        return 0;
    }

    if (fscanf(f, "%d %d", &prob->C, &prob->m) != 2) {
        printf("Formato inválido no cabeçalho do ficheiro\n");
        fclose(f);
        return 0;
    }

    // Inicializa matriz com zeros
    for (int i = 0; i < prob->C; i++) {
        for (int j = 0; j < prob->C; j++) {
            prob->dist[i][j] = 0.0;
        }
    }

    int c;
    // Avança até ao primeiro carácter não-espaço (pode haver newline depois do header)
    do {
        c = fgetc(f);
        if (c == EOF) {
            fclose(f);
            return 1;
        }
    } while (isspace(c));
    ungetc(c, f);

    if (isdigit(c) || c == '+' || c == '-' || c == '.') {
        // Modo matriz: lê C x C valores
        for (int i = 0; i < prob->C; i++) {
            for (int j = 0; j < prob->C; j++) {
                if (fscanf(f, "%lf", &prob->dist[i][j]) != 1) {
                    printf("Erro ao ler matriz de distâncias (linha %d, coluna %d)\n", i, j);
                    fclose(f);
                    return 0;
                }
            }
        }
    } else {
        // Modo lista de arestas com labels (ex: e1 e2 50.17)
        char a[64], b[64];
        double d;
        while (fscanf(f, "%s %s %lf", a, b, &d) == 3) {
            int ia = -1, ib = -1;
            if (sscanf(a, "e%d", &ia) == 1 && sscanf(b, "e%d", &ib) == 1) {
                ia--; ib--; // converte para 0-based
                if (ia >= 0 && ia < prob->C && ib >= 0 && ib < prob->C) {
                    prob->dist[ia][ib] = d;
                    prob->dist[ib][ia] = d;
                }
            } else {
                // linha não reconhecida — ignora
            }
        }
    }

    // garante diagonal zero
    for (int i = 0; i < prob->C; i++) prob->dist[i][i] = 0.0;

    fclose(f);
    return 1;
}

// Calcula a distância média de uma solução
double calculate_fitness(Solution *s, Problem *prob) {
    if (s->num_selected != prob->m) {
        return -INF; // Solução inválida
    }

    double sum = 0.0;
    int count = 0;

    // Converte array binário em lista de índices
    int points[MAX_CANDIDATES];
    int idx = 0;
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) {
            points[idx++] = i;
        }
    }

    // Calcula soma das distâncias entre pares
    for (int i = 0; i < prob->m - 1; i++) {
        for (int j = i + 1; j < prob->m; j++) {
            sum += prob->dist[points[i]][points[j]];
            count++;
        }
    }

    if (count == 0) return -INF;
    return sum / count; // média por par
}

// Cria uma solução aleatória válida
void random_solution(Solution *s, Problem *prob) {
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;

    // Seleciona m pontos aleatórios diferentes
    while (s->num_selected < prob->m) {
        int pos = rand() % prob->C;
        if (!s->selected[pos]) {
            s->selected[pos] = 1;
            s->num_selected++;
        }
    }

    // Debug: valida e calcula fitness se estiver tudo ok
    if (s->num_selected != prob->m) {
        fprintf(stderr, "DEBUG random_solution: num_selected (%d) != prob->m (%d)\n", s->num_selected, prob->m);
    }
    s->fitness = calculate_fitness(s, prob);
    // opcional: print curto para debug (comente depois)
    // printf("DEBUG random_solution: fitness=%.6f\n", s->fitness);
}

// Copia uma solução
void copy_solution(Solution *dest, Solution *src) {
    memcpy(dest->selected, src->selected, sizeof(src->selected));
    dest->num_selected = src->num_selected;
    dest->fitness = src->fitness;
}

// Imprime solução
void print_solution(Solution *s, Problem *prob) {
    printf("Pontos selecionados: ");
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) printf("%d ", i + 1); // imprime 1-based para bater com "e1..eC"
    }
    printf("\nFitness: %.2f\n", s->fitness);
}