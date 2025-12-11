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

// Swap 1: troca 1 candidato SELECIONADO por 1 candidato NÃO SELECIONADO
void gera_vizinho_swap(const Solution *actual, Solution *vizinho)
{
    copy_solution(vizinho, actual);
    int sel[MAX_CANDIDATES], nsel[MAX_CANDIDATES];
    int n_s = 0, n_ns = 0;

    // constroi listas de índices seleccionados e não seleccionados
    for (int i = 0; i < prob.C; i++)
    {
        if (vizinho->selected[i])
            sel[n_s++] = i;
        else
            nsel[n_ns++] = i;
    }

    // se um dos grupos estiver vazio não é possível efectuar swap
    if (n_s == 0 || n_ns == 0)
        return;

    // escolhe aleatoriamente um índice de cada grupo e troca-os
    int idx_s = random_l_h(0, n_s - 1);
    int idx_ns = random_l_h(0, n_ns - 1);

    vizinho->selected[sel[idx_s]] = 0;
    vizinho->selected[nsel[idx_ns]] = 1;
    vizinho->fitness = calculate_fitness(vizinho);
}

// Swap 2: troca 2 candidatos SELECIONADOS por 2 NÃO SELECIONADOS
void gera_vizinho_swap2(const Solution *actual, Solution *vizinho)
{
    copy_solution(vizinho, actual);
    int sel[MAX_CANDIDATES], nsel[MAX_CANDIDATES];
    int n_s = 0, n_ns = 0;

    // constroi listas de índices
    for (int i = 0; i < prob.C; i++)
    {
        if (vizinho->selected[i])
            sel[n_s++] = i;
        else
            nsel[n_ns++] = i;
    }

    // se não houver pelo menos 2 em cada grupo, recai para swap simples
    if (n_s < 2 || n_ns < 2)
    {
        gera_vizinho_swap(actual, vizinho); // alternativa simples
        return;
    }

    // escolhe dois índices distintos em cada grupo
    int i1 = random_l_h(0, n_s - 1);
    int i2 = random_l_h(0, n_s - 1);
    while (i1 == i2)
        i2 = random_l_h(0, n_s - 1);

    int j1 = random_l_h(0, n_ns - 1);
    int j2 = random_l_h(0, n_ns - 1);
    while (j1 == j2)
        j2 = random_l_h(0, n_ns - 1);

    // aplica as trocas
    vizinho->selected[sel[i1]] = 0;
    vizinho->selected[sel[i2]] = 0;
    vizinho->selected[nsel[j1]] = 1;
    vizinho->selected[nsel[j2]] = 1;
    vizinho->fitness = calculate_fitness(vizinho);
}

// Mutações para o algoritmo evolutivo (alteram a solução fornecida)
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

// Função de reparação: garante exactamente prob.m candidatos seleccionados.
// Adiciona ou remove candidatos aleatoriamente até o contador estar correcto.
// Nota: método simples e estocástico; pode ser optimizado se necessário.
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

// Crossover uniforme: para cada posição escolhe o bit de p1 ou p2 com prob. 0.5.
// Depois repara para garantir a restrição de m seleccionados e recalcula fitness.
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

/* One-point crossover:
   - Escolhe um ponto de corte cut em [1, prob.C-2].
   - A criança recebe o prefixo (0..cut-1) de p1 e o sufixo (cut..C-1) de p2.
   - Recalcula num_selected e repara se necessário (ver reparar_solucao).
   Exemplo breve (prob.C = 8, cut = 3):
     p1 = [1,0,1,0,1,0,0,0]
     p2 = [0,1,0,1,0,1,1,0]
     child (antes de reparar) = [1,0,1,1,0,1,1,0]
*/
void one_point_crossover(Solution *p1, Solution *p2, Solution *child)
{
    int cut = random_l_h(1, prob.C - 2);
    child->num_selected = 0;
    memset(child->selected, 0, sizeof(child->selected));
    for (int i = 0; i < cut; i++)
        child->selected[i] = p1->selected[i];
    for (int i = cut; i < prob.C; i++)
        child->selected[i] = p2->selected[i];

    // Recalcular número de seleccionados
    for (int i = 0; i < prob.C; i++)
        if (child->selected[i])
            child->num_selected++;

    reparar_solucao(child);
    child->fitness = calculate_fitness(child);
}

// Two-point crossover: escolhe dois cortes e troca a secção entre eles.
// Mantém a mesma lógica de reparação e cálculo de fitness.
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

// Hill Climbing: gera solução aleatória e chama a versão "from"
Solution hill_climbing(int max_iter, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return hill_climbing_from(actual, max_iter, vizinhanca_tipo);
}

// Hill Climbing a partir de uma solução inicial:
// - Gera vizinhos (swap ou swap2) e aceita se fitness >= actual.
// - Mantém e devolve a melhor solução encontrada.
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

        // Aceita se for melhor OU IGUAL (permite navegar em plateaus)
        if (vizinho.fitness >= actual.fitness)
        {
            copy_solution(&actual, &vizinho);
            if (actual.fitness > melhor.fitness)
                copy_solution(&melhor, &actual);
        }
    }
    return melhor;
}

// Simulated Annealing: gera solução aleatória e chama a versão "from"
Solution simulated_annealing(double tmax, double tmin, double alpha, int vizinhanca_tipo)
{
    Solution actual;
    random_solution(&actual);
    return simulated_annealing_from(actual, tmax, tmin, alpha, vizinhanca_tipo);
}

// Simulated Annealing a partir de start_sol:
// - T começa em tmax e decresce multiplicativamente até tmin.
// - Para cada temperatura executa várias tentativas e aceita piores
//   de acordo com a regra de Metropolis (probabilidade exp(delta/T)).
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
        T *= alpha; // diminuir temperatura
    }
    return melhor;
}

// ==========================================================
// 4. ALGORITMO EVOLUTIVO
// ==========================================================

// Seleção por torneio: escolhe k aleatórios e devolve o melhor
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

// Seleção por roleta (fitness proporcional)
// Nota: ignora fitness <= 0 na soma (assume fitness não-negativa para roleta)
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
    // fallback: devolve o último indivíduo
    return pop_size - 1;
}

// Algoritmo evolutivo completo:
// - inicialização aleatória, seleção, recombinação, mutação,
//   elitismo e passo memético (HC curto) opcional.
// - devolve a melhor solução global encontrada.
Solution evolutionary_algorithm(int pop_size, int generations, double prob_cross, double prob_mut, int sel_type, int cross_type, int viz_type, double prob_ls, int *out_div)
{
    Solution *pop = malloc(sizeof(Solution) * pop_size);
    Solution *offspring = malloc(sizeof(Solution) * pop_size);
    Solution global_best;
    global_best.fitness = -1e9; // valor muito baixo inicial

    // 1. Inicialização: popula aleatoriamente e guarda o melhor
    for (int i = 0; i < pop_size; i++)
    {
        random_solution(&pop[i]);
        if (pop[i].fitness > global_best.fitness)
            copy_solution(&global_best, &pop[i]);
    }

    // 2. Ciclo evolutivo
    for (int g = 0; g < generations; g++)
    {
        copy_solution(&offspring[0], &global_best); // elitismo: preserva o melhor

        for (int i = 1; i < pop_size; i++)
        {
            // A. Seleção
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

            // B. Crossover (com probabilidade prob_cross)
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
                copy_solution(&offspring[i], &pop[p1]); // sem crossover: copia p1
            }

            // C. Mutação
            if (rand_01() < prob_mut)
            {
                if (rand_01() < 0.5)
                    swap_mutation(&offspring[i]);
                else
                    swap2_mutation(&offspring[i]);
            }

            // D. Passo memético opcional: aplica HC curto com probabilidade prob_ls
            if (prob_ls > 0.0 && rand_01() < prob_ls)
            {
                Solution melhorada = hill_climbing_from(offspring[i], 50, viz_type);
                if (melhorada.fitness > offspring[i].fitness)
                {
                    copy_solution(&offspring[i], &melhorada);
                }
            }
        }

        // 3. Atualizar população e global_best
        for (int i = 0; i < pop_size; i++)
        {
            copy_solution(&pop[i], &offspring[i]);
            if (pop[i].fitness > global_best.fitness)
                copy_solution(&global_best, &pop[i]);
        }

        // Monitorização de diversidade para detectar estagnação
        if (g % 50 == 0)
        {
            int diversidade = count_unique_fitness(pop, pop_size);
            // se diversidade == 1 e já tiver passado 10% das gerações, interrompe
            if (diversidade == 1 && g > generations * 0.1)
            {
                break;
            }
        }
    }

    if (out_div != NULL)
        *out_div = count_unique_fitness(pop, pop_size);

    free(pop);
    free(offspring);
    return global_best;
}

// ==========================================================
// 5. HÍBRIDOS
// ==========================================================

// HÍBRIDO 1: GA seguido de Simulated Annealing para refinamento
Solution hybrid_algorithm_1(int pop_size, int generations, double tmax, double tmin, int sel_type, int cross_type, int viz_type)
{
    // executa primeiro o algoritmo evolutivo
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type, viz_type, 0.0, NULL);
    // depois refina com Simulated Annealing
    return simulated_annealing_from(best_ga, tmax, tmin, 0.99, viz_type);
}

// HÍBRIDO 2: GA seguido de Hill Climbing para refinamento local
Solution hybrid_algorithm_2(int pop_size, int generations, int hc_iter, int sel_type, int cross_type, int viz_type)
{
    Solution best_ga = evolutionary_algorithm(pop_size, generations, 0.7, 0.1, sel_type, cross_type, viz_type, 0.0, NULL);
    return hill_climbing_from(best_ga, hc_iter, viz_type);
}
