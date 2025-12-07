#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// Lê ficheiro de entrada com dois formatos possíveis:
// 1. Matriz completa: C m seguido de matriz C×C
// 2. Lista de arestas: C m seguido de linhas "ei ej distancia"
int read_file(char *filename, Problem *prob) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Erro ao abrir ficheiro %s\n", filename);
        return 0;
    }

    // Lê cabeçalho: C (total candidatos) e m (pontos a selecionar)
    if (fscanf(f, "%d %d", &prob->C, &prob->m) != 2) {
        printf("Formato inválido no cabeçalho do ficheiro\n");
        fclose(f);
        return 0;
    }

    // Inicializa matriz de distâncias com zeros
    for (int i = 0; i < prob->C; i++) {
        for (int j = 0; j < prob->C; j++) {
            prob->dist[i][j] = 0.0;
        }
    }

    // Salta espaços em branco após cabeçalho
    int c;
    do {
        c = fgetc(f);
        if (c == EOF) {
            fclose(f);
            return 1;
        }
    } while (isspace(c));
    ungetc(c, f);

    // Detecta formato: número = matriz, letra = lista de arestas
    if (isdigit(c) || c == '+' || c == '-' || c == '.') {
        // Formato matriz: lê C×C valores
        for (int i = 0; i < prob->C; i++) {
            for (int j = 0; j < prob->C; j++) {
                if (fscanf(f, "%lf", &prob->dist[i][j]) != 1) {
                    printf("Erro ao ler matriz de distâncias\n");
                    fclose(f);
                    return 0;
                }
            }
        }
    } else {
        // Formato lista: lê linhas "e1 e2 distancia"
        char a[64], b[64];
        double d;
        while (fscanf(f, "%s %s %lf", a, b, &d) == 3) {
            int ia = -1, ib = -1;
            // Extrai índices (e1 -> 0, e2 -> 1, etc.)
            if (sscanf(a, "e%d", &ia) == 1 && sscanf(b, "e%d", &ib) == 1) {
                ia--; ib--; // Converte para índice 0-based
                if (ia >= 0 && ia < prob->C && ib >= 0 && ib < prob->C) {
                    prob->dist[ia][ib] = d;
                    prob->dist[ib][ia] = d; // Matriz simétrica
                }
            }
        }
    }

    // Garante diagonal zero (distância de um ponto para si mesmo)
    for (int i = 0; i < prob->C; i++) prob->dist[i][i] = 0.0;

    fclose(f);
    return 1;
}

// Calcula fitness como distância média entre pares de pontos selecionados
// Fórmula: soma(dist(i,j)) / m para todos os pares (i,j) selecionados
double calculate_fitness(Solution *s, Problem *prob) {
    // Valida que exatamente m pontos estão selecionados
    if (s->num_selected != prob->m) {
        return -INF; // Solução inválida
    }

    double sum = 0.0;

    // Converte array binário em lista de índices dos pontos selecionados
    int points[MAX_CANDIDATES];
    int idx = 0;
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) {
            points[idx++] = i;
        }
    }

    // Soma distâncias de todos os pares (i,j) com i < j
    for (int i = 0; i < prob->m - 1; i++) {
        for (int j = i + 1; j < prob->m; j++) {
            sum += prob->dist[points[i]][points[j]];
        }
    }

    // Divide por m (conforme especificação do problema)
    return sum / prob->m;
}

// Gera solução aleatória válida selecionando m pontos distintos
void random_solution(Solution *s, Problem *prob) {
    // Inicializa array com todos os pontos não selecionados
    memset(s->selected, 0, sizeof(s->selected));
    s->num_selected = 0;

    // Seleciona m pontos aleatórios distintos
    while (s->num_selected < prob->m) {
        int pos = rand() % prob->C; // Escolhe posição aleatória
        if (!s->selected[pos]) {    // Se ainda não selecionado
            s->selected[pos] = 1;   // Marca como selecionado
            s->num_selected++;
        }
    }

    // Calcula fitness da solução gerada
    s->fitness = calculate_fitness(s, prob);
}

// Copia todos os campos de uma solução para outra
void copy_solution(Solution *dest, Solution *src) {
    memcpy(dest->selected, src->selected, sizeof(src->selected));
    dest->num_selected = src->num_selected;
    dest->fitness = src->fitness;
}

// Imprime solução na consola (formato 1-based para coincidir com "e1, e2, ...")
void print_solution(Solution *s, Problem *prob) {
    printf("Pontos selecionados: ");
    for (int i = 0; i < prob->C; i++) {
        if (s->selected[i]) printf("%d ", i + 1); // Imprime 1-based
    }
    printf("\nFitness: %.2f\n", s->fitness);
}