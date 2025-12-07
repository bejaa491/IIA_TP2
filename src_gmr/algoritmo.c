#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

// ==========================================================
// 1. VIZINHANÇAS E MUTAÇÕES
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

    int idx_s = random_l_h(0, n_s - 1);
    int idx_ns = random_l_h(0, n_ns - 1);

    vizinho->selected[sel[idx_s]] = 0;
    vizinho->selected[nsel[idx_ns]] = 1;
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
        gera_vizinho_swap(actual, vizinho); // Fallback se não der para trocar 2
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

// Mutações para o Evolutivo (Alteram a solução original)
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
// 2. CROSSOVERS (RECOMBINAÇÃO)
// ==========================================================

// Função auxiliar de reparação (garante sempre m pontos)
void reparar_solucao(Solution *s)
{
    while (s->num_selected < prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (!s->selected[pos])
        {
            s->selected[pos] = 1;
            s->num_selected++;
        }
    }
    while (s->num_selected > prob.m)
    {
        int pos = random_l_h(0, prob.C - 1);
        if (s->selected[pos])
        {
            s->selected[pos] = 0;
            s->num_selected--;
        }
    }
}

void uniform_crossover(Solution *p1, Solution *p2, Solution *child)
{
    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));
    for (int i = 0; i < prob.C; i++)
    {
        if (rand_01() < 0.5)
            child->selected[i] = p1->selected[i];
        else
            child->selected[i] = p2->selected[i];
        if (child->selected[i])
            child->num_selected++;
    }
    reparar_solucao(child);
    child->fitness = calculate_fitness(child);
}

void one_point_crossover(Solution *p1, Solution *p2, Solution *child)
{
    int cut = random_l_h(1, prob.C - 2);
    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));
    for (int i = 0; i < cut; i++)
        child->selected[i] = p1->selected[i];
    for (int i = cut; i < prob.C; i++)
        child->selected[i] = p2->selected[i];

    // Recalcular num_selected manualmente
    for (int i = 0; i < prob.C; i++)
        if (child->selected[i])
            child->num_selected++;

    reparar_solucao(child);
    child->fitness = calculate_fitness(child);
}

void two_point_crossover(Solution *p1, Solution *p2, Solution *child)
{
    int cut1 = random_l_h(0, prob.C - 2);
    int cut2 = random_l_h(0, prob.C - 1);
    if (cut1 > cut2)
    {
        int t = cut1;
        cut1 = cut2;
        cut2 = t;
    }

    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));
    for (int i = 0; i < prob.C; i++)
    {
        if (i < cut1 || i > cut2)
            child->selected[i] = p1->selected[i];
        else
            child->selected[i] = p2->selected[i];
    }

    for (int i = 0; i < prob.C; i++)
        if (child->selected[i])
            child->num_selected++;

    reparar_solucao(child);
    child->fitness = calculate_fitness(child);
}

// ==========================================================
// 3. PESQUISA LOCAL (HC e SA)
// ==========================================================

Solution hill_climbing(int max_iter, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return hill_climbing_from(actual, max_iter, vizinhanca_tipo);
}

Solution hill_climbing_from(Solution start_sol, int max_iter, int vizinhanca_tipo)
{
    Solution actual, vizinho, melhor;
    copy_solution(&actual, &start_sol);
    copy_solution(&melhor, &actual);

    for (int i = 0; i < max_iter; i++)
    {
        if (vizinhanca_tipo == 1)
            gera_vizinho_swap(&actual, &vizinho);
        else
            gera_vizinho_swap2(&actual, &vizinho);

        // Aceita se for melhor OU IGUAL (navegar em plateaus)
        if (vizinho.fitness >= actual.fitness)
        {
            copy_solution(&actual, &vizinho);
            if (actual.fitness > melhor.fitness)
                copy_solution(&melhor, &actual);
        }
    }
    return melhor;
}

Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return simulated_annealing_from(actual, tmax, tmin, alpha, vizinhanca_tipo);
}

Solution simulated_annealing_from(Solution start_sol, double tmax, double tmin, double alpha, int vizinhanca_tipo)
{
    Solution actual, vizinho, melhor;
    copy_solution(&actual, &start_sol);
    copy_solution(&melhor, &actual);

    double T = tmax;
    while (T > tmin)
    {
        for (int i = 0; i < 50; i++)
        {
            if (vizinhanca_tipo == 1)
                gera_vizinho_swap(&actual, &vizinho);
            else
                gera_vizinho_swap2(&actual, &vizinho);

            double delta = vizinho.fitness - actual.fitness;
            if (delta >= 0 || rand_01() < exp(delta / T))
            {
                copy_solution(&actual, &vizinho);
                if (actual.fitness > melhor.fitness)
                    copy_solution(&melhor, &actual);
            }
        }
        T *= alpha;
    }
    return melhor;
}

// ==========================================================
// 4. ALGORITMO EVOLUTIVO
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
        if (pop[i].fitness > 0)
            sum_fit += pop[i].fitness;
    double r = rand_01() * sum_fit;
    double partial = 0.0;
    for (int i = 0; i < pop_size; i++)
    {
        if (pop[i].fitness > 0)
            partial += pop[i].fitness;
        if (partial >= r)
            return i;
    }
    return pop_size - 1;
}


Solution evolutionary_algorithm(int pop_size, int generations, double prob_cross, double prob_mut, int sel_type, int cross_type, int *out_div)
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

    // Ciclo das Gerações
    for (int g = 0; g < generations; g++)
    {

        // 1. Elitismo
        copy_solution(&offspring[0], &global_best);

        // 2. Criar nova população
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

            // Crossover
            if (rand_01() < prob_cross)
            {
                if (cross_type == 1)
                    uniform_crossover(&pop[p1], &pop[p2], &offspring[i]);
                else if (cross_type == 2)
                    one_point_crossover(&pop[p1], &pop[p2], &offspring[i]);
                else
                    two_point_crossover(&pop[p1], &pop[p2], &offspring[i]);
            }
            else
            {
                copy_solution(&offspring[i], &pop[p1]);
            }

            // Mutação
            if (rand_01() < prob_mut)
            {
                if (rand_01() < 0.5)
                    swap_mutation(&offspring[i]);
                else
                    swap2_mutation(&offspring[i]);
            }
        }

        // 3. Atualizar população e Global Best
        for (int i = 0; i < pop_size; i++)
        {
            copy_solution(&pop[i], &offspring[i]);
            if (pop[i].fitness > global_best.fitness)
                copy_solution(&global_best, &pop[i]);
        }

        // Só verificamos a cada 50 gerações para não tornar o código lento
        if (g % 50 == 0)
        {
            int diversidade = count_unique_fitness(pop, pop_size);

            // Opcional: Mostrar progresso no ecrã
            // printf("Gen %d | Best: %.2f | Diversidade: %d/%d\n", g, global_best.fitness, diversidade, pop_size);

            // Opcional: Parar se estagnar (se a diversidade for 1, todos são iguais)
            // Só ativamos isto depois de 10% das gerações para dar tempo de começar
            if (diversidade == 1 && g > generations * 0.1)
            {
                // printf(" >> Convergencia prematura detetada na geracao %d. A parar.\n", g);
                break; // Sai do ciclo for
            }
        }
    }
    if (out_div != NULL)
    {
        *out_div = count_unique_fitness(pop, pop_size);
    }
    free(pop);
    free(offspring);
    return global_best;
}

// ==========================================================
// 5. HÍBRIDOS
// ==========================================================

// HÍBRIDO 1: Evolutivo + Simulated Annealing
Solution hybrid_algorithm_1(int pop_size, int generations, double tmax, double tmin, int sel_type, int cross_type, int viz_type)
{
    // Primeiro executa o algoritmo evolutivo
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type, NULL);
    // Depois aplica Simulated Annealing à melhor solução obtida
    return simulated_annealing_from(best_ga, tmax, tmin, 0.99, viz_type);
}

// HÍBRIDO 2: Evolutivo + Hill Climbing
Solution hybrid_algorithm_2(int pop_size, int generations, int hc_iter, int sel_type, int cross_type, int viz_type)
{
    // Primeiro executa o algoritmo evolutivo
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type, NULL);
    // Depois aplica Hill Climbing à melhor solução obtida
    return hill_climbing_from(best_ga, hc_iter, viz_type);
}
