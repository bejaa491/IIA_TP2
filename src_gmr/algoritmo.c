#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

// ==========================================================
// VIZINHANÇAS E MUTAÇÕES
// ==========================================================

// Swap 1: Troca 1 ponto IN por 1 ponto OUT
void gera_vizinho_swap(const Solution *actual, Solution *vizinho)
{
    copy_solution(vizinho, actual);
    int sel[MAX_CANDIDATES], nsel[MAX_CANDIDATES];
    int n_s = 0, n_ns = 0;

    for (int i = 0; i < prob.C; i++)
    {
        if (vizinho->selected[i])
            sel[n_s++] = i;
        else
            nsel[n_ns++] = i;
    }

    if (n_s == 0 || n_ns == 0)
        return;

    int id_s = random_l_h(0, n_s - 1);
    int id_ns = random_l_h(0, n_ns - 1);

    vizinho->selected[sel[id_s]] = 0;
    vizinho->selected[nsel[id_ns]] = 1;
    vizinho->fitness = calculate_fitness(vizinho);
}

// Swap 2: Troca 2 pontos IN por 2 pontos OUT
void gera_vizinho_swap2(const Solution *actual, Solution *vizinho)
{
    copy_solution(vizinho, actual);
    int sel[MAX_CANDIDATES], nsel[MAX_CANDIDATES];
    int n_s = 0, n_ns = 0;

    for (int i = 0; i < prob.C; i++)
    {
        if (vizinho->selected[i])
            sel[n_s++] = i;
        else
            nsel[n_ns++] = i;
    }

    if (n_s < 2 || n_ns < 2)
    {
        gera_vizinho_swap(actual, vizinho); // Fallback
        return;
    }

    int i1 = random_l_h(0, n_s - 1);
    int i2 = random_l_h(0, n_s - 1);
    while (i1 == i2)
        i2 = random_l_h(0, n_s - 1);

    int j1 = random_l_h(0, n_ns - 1);
    int j2 = random_l_h(0, n_ns - 1);
    while (j1 == j2)
        j2 = random_l_h(0, n_ns - 1);

    vizinho->selected[sel[i1]] = 0;
    vizinho->selected[sel[i2]] = 0;
    vizinho->selected[nsel[j1]] = 1;
    vizinho->selected[nsel[j2]] = 1;
    vizinho->fitness = calculate_fitness(vizinho);
}

// Mutação para o Evolutivo (Alteração direta)
void swap_mutation(Solution *s)
{
    Solution temp;
    gera_vizinho_swap(s, &temp);
    copy_solution(s, &temp);
}

void swap2_mutation(Solution *s)
{
    Solution temp;
    gera_vizinho_swap2(s, &temp);
    copy_solution(s, &temp);
}

// ==========================================================
// PESQUISA LOCAL
// ==========================================================

// ==========================================================
// TREPA-COLINAS (Hill Climbing)
// ==========================================================

// Função "Normal" (começa do zero)
Solution hill_climbing(int max_iter, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return hill_climbing_from(actual, max_iter, vizinhanca_tipo);
}

// Função que aceita Solução Inicial (Essencial para o Híbrido)
// Trepa-Colinas que aceita uma solução inicial (Para o Híbrido e Testes)
Solution hill_climbing_from(Solution start_sol, int max_iter, int vizinhanca_tipo)
{
    Solution actual, vizinho, melhor;

    // 1. Inicializar com a solução recebida (ex: vinda do Evolutivo)
    copy_solution(&actual, &start_sol);
    copy_solution(&melhor, &actual);

    for (int it = 0; it < max_iter; it++)
    {
        // 2. Escolher a vizinhança dinamicamente
        if (vizinhanca_tipo == 1)
            gera_vizinho_swap(&actual, &vizinho);
        else
            gera_vizinho_swap2(&actual, &vizinho);

        // 3. Aceitar se for melhor OU IGUAL (>=)
        // Isso permite atravessar "zonas planas" para tentar subir noutro lugar
        if (vizinho.fitness >= actual.fitness)
        {
            copy_solution(&actual, &vizinho);

            // Só atualizamos o melhor global se for estritamente maior
            if (actual.fitness > melhor.fitness)
            {
                copy_solution(&melhor, &actual);
            }
        }
    }

    return melhor;
}
// ==========================================================
// SIMULATED ANNEALING
// ==========================================================

Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return simulated_annealing_from(actual, tmax, tmin, alpha, vizinhanca_tipo);
}

// Versão que aceita ponto de partida (essencial para o Híbrido)
Solution simulated_annealing_from(Solution start_sol, double tmax, double tmin, double alpha, int vizinhanca_tipo)
{
    Solution actual, vizinho, melhor;
    copy_solution(&actual, &start_sol);
    copy_solution(&melhor, &actual);

    double T = tmax;
    while (T > tmin)
    {
        for (int i = 0; i < 50; i++)
        { // Iterações por temperatura
            if (vizinhanca_tipo == 1)
                gera_vizinho_swap(&actual, &vizinho);
            else
                gera_vizinho_swap2(&actual, &vizinho);

            double delta = vizinho.fitness - actual.fitness;

            // Maximização: delta > 0 é bom
            if (delta > 0 || rand_01() < exp(delta / T))
            {
                copy_solution(&actual, &vizinho);
                if (actual.fitness > melhor.fitness)
                {
                    copy_solution(&melhor, &actual);
                }
            }
        }
        T *= alpha;
    }
    return melhor;
}

// ==========================================================
// ALGORITMO EVOLUTIVO (Componentes)
// ==========================================================

int tournament_selection(Solution *pop, int pop_size, int k)
{
    int best = random_l_h(0, pop_size - 1);
    for (int i = 1; i < k; i++)
    {
        int cand = random_l_h(0, pop_size - 1);
        if (pop[cand].fitness > pop[best].fitness)
            best = cand;
    }
    return best;
}

int roulette_selection(Solution *pop, int pop_size)
{
    double sum_fit = 0.0;
    for (int i = 0; i < pop_size; i++)
        sum_fit += pop[i].fitness;
    double r = rand_01() * sum_fit;
    double partial = 0.0;
    for (int i = 0; i < pop_size; i++)
    {
        partial += pop[i].fitness;
        if (partial >= r)
            return i;
    }
    return pop_size - 1;
}

void uniform_crossover(Solution *p1, Solution *p2, Solution *child)
{
    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));

    // Herança
    for (int i = 0; i < prob.C; i++)
    {
        if (rand_01() < 0.5)
            child->selected[i] = p1->selected[i];
        else
            child->selected[i] = p2->selected[i];

        if (child->selected[i])
            child->num_selected++;
    }

    // Reparação (Adicionar ou Remover até ter m)
    while (child->num_selected < prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (!child->selected[pos])
        {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }
    while (child->num_selected > prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (child->selected[pos])
        {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }
    child->fitness = calculate_fitness(child);
}

// Crossover de Um Ponto (com Reparação Obrigatória)
void one_point_crossover(Solution *p1, Solution *p2, Solution *child)
{
    // 1. Escolher ponto de corte aleatório (evitando as pontas)
    int cut = random_l_h(1, prob.C - 2);

    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));

    // 2. Copiar partes
    // Parte esquerda do Pai 1
    for (int i = 0; i < cut; i++)
    {
        child->selected[i] = p1->selected[i];
        if (child->selected[i])
            child->num_selected++;
    }
    // Parte direita do Pai 2
    for (int i = cut; i < prob.C; i++)
    {
        child->selected[i] = p2->selected[i];
        if (child->selected[i])
            child->num_selected++;
    }

    // 3. REPARAÇÃO (Crítica para manter m pontos)
    // Se tiver a menos, adiciona aleatoriamente
    while (child->num_selected < prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (child->selected[pos] == 0)
        {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }
    // Se tiver a mais, remove aleatoriamente
    while (child->num_selected > prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (child->selected[pos] == 1)
        {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child);
}

// Crossover de Dois Pontos (com Reparação Obrigatória)
void two_point_crossover(Solution *p1, Solution *p2, Solution *child)
{
    // 1. Escolher 2 pontos de corte distintos
    int cut1 = random_l_h(0, prob.C - 2);
    int cut2 = random_l_h(0, prob.C - 1);

    // Garantir que cut1 é menor que cut2
    if (cut1 > cut2)
    {
        int temp = cut1;
        cut1 = cut2;
        cut2 = temp;
    }

    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));

    // 2. Copiar partes
    for (int i = 0; i < prob.C; i++)
    {
        if (i < cut1 || i > cut2)
        {
            // Pontas vêm do Pai 1
            child->selected[i] = p1->selected[i];
        }
        else
        {
            // O meio vem do Pai 2
            child->selected[i] = p2->selected[i];
        }

        if (child->selected[i])
            child->num_selected++;
    }

    // 3. REPARAÇÃO (Igual à anterior)
    while (child->num_selected < prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (child->selected[pos] == 0)
        {
            child->selected[pos] = 1;
            child->num_selected++;
        }
    }
    while (child->num_selected > prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (child->selected[pos] == 1)
        {
            child->selected[pos] = 0;
            child->num_selected--;
        }
    }

    child->fitness = calculate_fitness(child);
}

// ==========================================================
// ALGORITMOS PRINCIPAIS (Evolutivo e Híbrido)
// ==========================================================

Solution evolutionary_algorithm(int pop_size, int generations, double prob_cross, double prob_mut, int sel_type, int cross_type)
{
    Solution *pop = malloc(sizeof(Solution) * pop_size);
    Solution *offspring = malloc(sizeof(Solution) * pop_size);
    Solution global_best;
    global_best.fitness = -1.0;

    // Inicialização
    for (int i = 0; i < pop_size; i++)
    {
        random_solution(&pop[i]);
        if (pop[i].fitness > global_best.fitness)
            copy_solution(&global_best, &pop[i]);
    }

    // Gerações
    for (int g = 0; g < generations; g++)
    {
        // Elitismo: Melhor passa direto
        copy_solution(&offspring[0], &global_best);

        for (int i = 1; i < pop_size; i++)
        {
            // Seleção
            int p1, p2;
            if (sel_type == 1)
            {
                p1 = tournament_selection(pop, pop_size, 3);
                p2 = tournament_selection(pop, pop_size, 3);
            }
            else
            {
                p1 = roulette_selection(pop, pop_size);
                p2 = roulette_selection(pop, pop_size);
            }

            if (rand_01() < prob_cross)
            {
                // Crossover Uniforme
                if (cross_type == 1)
                    uniform_crossover(&pop[p1], &pop[p2], &offspring[i]);
                // Crossover de Um Ponto
                else if (cross_type == 2)
                    one_point_crossover(&pop[p1], &pop[p2], &offspring[i]);
                // Crossover de Dois Pontos
                else
                    two_point_crossover(&pop[p1], &pop[p2], &offspring[i]);
            }
            else
            {
                // Sem crossover: Cópia direta
                copy_solution(&offspring[i], &pop[p1]);
            }

            // C. Mutação
            // Nota: Mesmo que seja um clone (do else acima), ele ainda pode sofrer mutação.
            // Isso é correto e desejável!
            if (rand_01() < prob_mut)
            {
                if (rand_01() < 0.5)
                    swap_mutation(&offspring[i]);
                else
                    swap2_mutation(&offspring[i]);
            }
        }

        // Atualizar população
        for (int i = 0; i < pop_size; i++)
        {
            copy_solution(&pop[i], &offspring[i]);
            if (pop[i].fitness > global_best.fitness)
                copy_solution(&global_best, &pop[i]);
        }
    }

    free(pop);
    free(offspring);
    return global_best;
}

// ==========================================================
// HÍBRIDO 1: Evolutivo + Simulated Annealing
// ==========================================================

Solution hybrid_algorithm_1(int pop_size, int generations, double tmax, double tmin, 
                          int sel_type, int cross_type, int viz_type) 
{
    // Fase 1: Evolutivo (com as tuas escolhas de seleção e crossover)
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type);
    
    // Fase 2: SA (com a tua escolha de vizinhança)
    return simulated_annealing_from(best_ga, tmax, tmin, 0.99, viz_type);
}

// ==========================================================
// HÍBRIDO 2: Evolutivo + Hill Climbing
// ==========================================================
Solution hybrid_algorithm_2(int pop_size, int generations, int hc_iter, 
                          int sel_type, int cross_type, int viz_type) 
{
    // Fase 1: Evolutivo
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type);
    
    // Fase 2: HC
    return hill_climbing_from(best_ga, hc_iter, viz_type);
}