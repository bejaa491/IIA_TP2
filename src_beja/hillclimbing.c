#include <stdlib.h>
#include "hillclimbing.h"
#include "utils.h"

// Encontra aleatoriamente um ponto selecionado na solução
// Retorna índice do ponto ou -1 se falhar após 100 tentativas
static int find_random_selected(Solution *s, int C) {
    int attempts = 0;
    while (attempts < 100) {
        int pos = rand() % C;
        if (s->selected[pos]) return pos;
        attempts++;
    }
    return -1;
}

// Encontra aleatoriamente um ponto não selecionado na solução
// Retorna índice do ponto ou -1 se falhar após 100 tentativas
static int find_random_unselected(Solution *s, int C) {
    int attempts = 0;
    while (attempts < 100) {
        int pos = rand() % C;
        if (!s->selected[pos]) return pos;
        attempts++;
    }
    return -1;
}

// Vizinhança 1: Troca 1 ponto selecionado por 1 não selecionado
// Movimento mais fino para busca local detalhada
void neighborhood1(Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    // Encontra um ponto selecionado e um não selecionado
    int sel = find_random_selected(neighbor, prob->C);
    int unsel = find_random_unselected(neighbor, prob->C);

    // Realiza troca se encontrou ambos
    if (sel != -1 && unsel != -1) {
        neighbor->selected[sel] = 0;    // Remove ponto selecionado
        neighbor->selected[unsel] = 1;  // Adiciona ponto não selecionado
        neighbor->fitness = calculate_fitness(neighbor, prob);
    }
}

// Vizinhança 2: Troca 2 pontos selecionados por 2 não selecionados
// Movimento mais amplo para exploração mais agressiva
void neighborhood2(Solution *current, Solution *neighbor, Problem *prob) {
    copy_solution(neighbor, current);

    // Determina número de trocas (2 se m >= 2, senão 1)
    int swap_count = (prob->m >= 2) ? 2 : 1;

    // Realiza swap_count trocas
    for (int tries = 0; tries < swap_count; tries++) {
        int sel = find_random_selected(neighbor, prob->C);
        int unsel = find_random_unselected(neighbor, prob->C);

        if (sel != -1 && unsel != -1) {
            neighbor->selected[sel] = 0;
            neighbor->selected[unsel] = 1;
        }
    }

    neighbor->fitness = calculate_fitness(neighbor, prob);
}

// Hill Climbing: busca local que aceita melhorias ou soluções de igual qualidade
// Estratégia: explora vizinhança até atingir máximo de iterações
Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob) {
    Solution current, best, neighbor;

    // Inicializa com solução aleatória
    random_solution(&current, prob);
    copy_solution(&best, &current);

    // Loop principal de busca
    for (int iter = 0; iter < max_iter; iter++) {
        // Gera vizinho conforme tipo de vizinhança escolhido
        if (neighborhood_type == 1) {
            neighborhood1(&current, &neighbor, prob);
        } else {
            neighborhood2(&current, &neighbor, prob);
        }

        // Aceita melhorias ou soluções iguais (>= ao invés de >)
        // Isso ajuda a escapar de platôs
        if (neighbor.fitness >= current.fitness) {
            copy_solution(&current, &neighbor);

            // Atualiza melhor solução global se necessário
            if (current.fitness > best.fitness) {
                copy_solution(&best, &current);
            }
        }
    }

    return best;
}

// Hill Climbing com restart: executa múltiplas vezes para diversificar busca
// Estratégia: divide iterações entre restarts e mantém melhor solução global
Solution hill_climbing_restart(int max_iter, int neighborhood_type, int num_restarts, Problem *prob) {
    Solution best;
    random_solution(&best, prob);

    // Executa num_restarts buscas independentes
    for (int restart = 0; restart < num_restarts; restart++) {
        // Cada restart recebe max_iter/num_restarts iterações
        Solution current = hill_climbing(max_iter / num_restarts, neighborhood_type, prob);
        
        // Atualiza melhor solução global
        if (current.fitness > best.fitness) {
            copy_solution(&best, &current);
        }
    }

    return best;
}