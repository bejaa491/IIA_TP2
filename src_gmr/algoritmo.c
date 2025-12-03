#include <stdio.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

// Gera vizinho trocando um ponto selecionado por um não selecionado
void gera_vizinho_swap(const Solution *actual, Solution *vizinho)
{
    // começa por copiar a solução actual
    copy_solution((Solution *)vizinho, actual);

    int selecionados[MAX_CANDIDATES];
    int nao_selecionados[MAX_CANDIDATES];
    int n_sel = 0, n_nsel = 0;

    // separar índices selecionados e não selecionados
    for (int i = 0; i < prob.C; i++) {
        if (vizinho->selected[i])
            selecionados[n_sel++] = i;
        else
            nao_selecionados[n_nsel++] = i;
    }

    // escolher um índice aleatório de cada grupo
    int idx_sel  = random_l_h(0, n_sel  - 1);
    int idx_nsel = random_l_h(0, n_nsel - 1);

    int i_sel  = selecionados[idx_sel];
    int i_nsel = nao_selecionados[idx_nsel];

    // fazer a troca
    vizinho->selected[i_sel]  = 0;
    vizinho->selected[i_nsel] = 1;

    // num_selected continua a ser m
    // (mas vamos garantir isso explicitamente)
    vizinho->num_selected = actual->num_selected;

    // recalcular fitness
    vizinho->fitness = calculate_fitness((Solution *)vizinho);
}

Solution hill_climbing(int max_iter)
{
    Solution actual;
    Solution vizinho;
    Solution melhor;

    // solução inicial aleatória
    random_solution(&actual);
    copy_solution(&melhor, &actual);

    for (int it = 0; it < max_iter; it++) {
        // gera vizinho
        gera_vizinho_swap(&actual, &vizinho);

        // se vizinho for melhor, aceita
        if (vizinho.fitness > actual.fitness) {
            copy_solution(&actual, &vizinho);
            if (actual.fitness > melhor.fitness) {
                copy_solution(&melhor, &actual);
            }
        }
        // se não for melhor, ignoramos (trepa-colinas “ganancioso”)
    }

    return melhor;
}

// algoritmo.c
Solution hill_climbing_from(Solution actual, int max_iter)
{
    Solution vizinho;
    Solution melhor;

    copy_solution(&melhor, &actual);

    for (int it = 0; it < max_iter; it++) {
        gera_vizinho_swap(&actual, &vizinho);

        if (vizinho.fitness > actual.fitness) {
            copy_solution(&actual, &vizinho);
            if (actual.fitness > melhor.fitness) {
                copy_solution(&melhor, &actual);
            }
        }
    }

    return melhor;
}
