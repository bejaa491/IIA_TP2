#ifndef UTILS_H
#define UTILS_H

#include "structures.h"

// Lê o ficheiro de entrada
int read_file(char *filename, Problem *prob);

// Calcula a distância média de uma solução
double calculate_fitness(Solution *s, Problem *prob);

// Cria uma solução aleatória válida
void random_solution(Solution *s, Problem *prob);

// Copia uma solução
void copy_solution(Solution *dest, Solution *src);

// Imprime solução
void print_solution(Solution *s, Problem *prob);

#endif