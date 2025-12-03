#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "funcao.h"   // para usar Solution

// Gera um vizinho da solução actual (troca 1 ponto escolhido com 1 não escolhido)
void gera_vizinho_swap(const Solution *actual, Solution *vizinho);

// Trepa-colinas simples: começa numa solução aleatória e faz max_iter passos
Solution hill_climbing(int max_iter);

Solution hill_climbing_from(Solution start, int max_iter);

#endif
