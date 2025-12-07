#ifndef UTILS_H
#define UTILS_H

#include "structures.h"

// Lê ficheiro de entrada e preenche estrutura Problem
// Retorna 1 em sucesso, 0 em caso de erro
int read_file(char *filename, Problem *prob);

// Calcula fitness (distância média) de uma solução
// Retorna -INF se solução for inválida (num_selected != m)
double calculate_fitness(Solution *s, Problem *prob);

// Gera solução aleatória válida com exatamente m pontos
void random_solution(Solution *s, Problem *prob);

// Copia solução src para dest
void copy_solution(Solution *dest, Solution *src);

// Imprime solução na consola (pontos selecionados e fitness)
void print_solution(Solution *s, Problem *prob);

#endif