#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

void gera_vizinho_swap2(const Solution *actual, Solution *vizinho)
{
    copy_solution((Solution *)vizinho, actual);
    
    int selecionados[MAX_CANDIDATES];
    int nao_selecionados[MAX_CANDIDATES];
    int n_sel = 0, n_nsel = 0;

    // Identificar listas
    for (int i = 0; i < prob.C; i++) {
        if (vizinho->selected[i]) selecionados[n_sel++] = i;
        else nao_selecionados[n_nsel++] = i;
    }

    // Só é possível trocar 2 se houver pelo menos 2 selecionados e 2 livres
    if (n_sel < 2 || n_nsel < 2) {
        // Se não der, faz apenas swap normal ou retorna sem mudar
        gera_vizinho_swap(actual, vizinho); 
        return;
    }

    // Escolher 2 índices diferentes para retirar
    int idx_s1 = random_l_h(0, n_sel - 1);
    int idx_s2 = random_l_h(0, n_sel - 1);
    while (idx_s1 == idx_s2) idx_s2 = random_l_h(0, n_sel - 1); // garantir diferentes

    // Escolher 2 índices diferentes para entrar
    int idx_ns1 = random_l_h(0, n_nsel - 1);
    int idx_ns2 = random_l_h(0, n_nsel - 1);
    while (idx_ns1 == idx_ns2) idx_ns2 = random_l_h(0, n_nsel - 1); // garantir diferentes

    // Efetuar as trocas
    vizinho->selected[selecionados[idx_s1]] = 0;
    vizinho->selected[selecionados[idx_s2]] = 0;

    vizinho->selected[nao_selecionados[idx_ns1]] = 1;
    vizinho->selected[nao_selecionados[idx_ns2]] = 1;

    // Recalcular fitness
    vizinho->fitness = calculate_fitness((Solution *)vizinho);
}

Solution hill_climbing(int max_iter, int vizinhanca_tipo)
{
    Solution actual, vizinho, melhor;
    random_solution(&actual);
    copy_solution(&melhor, &actual);

    for (int it = 0; it < max_iter; it++) {
        
        // Escolhe a vizinhança baseada no parâmetro
        if (vizinhanca_tipo == 1)
            gera_vizinho_swap(&actual, &vizinho);
        else
            gera_vizinho_swap2(&actual, &vizinho);

        // Lógica de aceitação:
        // O enunciado sugere testar "Aceitar soluções de custo igual".
        // Podes mudar para >= para permitir navegar em "plateaus"
        if (vizinho.fitness >= actual.fitness) { 
            copy_solution(&actual, &vizinho);
            
            if (actual.fitness > melhor.fitness) {
                copy_solution(&melhor, &actual);
            }
        }
    }
    return melhor;
}

Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo){
    Solution actual, vizinho, melhor;
    random_solution(&actual);
    copy_solution(&melhor, &actual);

    double T = tmax;

    while (T > tmin) {
        for (int i = 0; i < 100; i++) { // 100 iterações por temperatura
            // Escolhe a vizinhança baseada no parâmetro
            if (vizinhanca_tipo == 1)
                gera_vizinho_swap(&actual, &vizinho);
            else
                gera_vizinho_swap2(&actual, &vizinho);

            double delta = vizinho.fitness - actual.fitness;

            if (delta >= 0) {
                copy_solution(&actual, &vizinho);
                if (actual.fitness > melhor.fitness) {
                    copy_solution(&melhor, &actual);
                }
            } else {
                double prob = exp(delta / T);
                if (rand_01() < prob) {
                    copy_solution(&actual, &vizinho);
                }
            }
        }
        T *= alpha; // reduzir temperatura
    }

    return melhor;
}

Solution hill_climbing_old(int max_iter)
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
