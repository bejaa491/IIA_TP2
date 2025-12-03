// funcao.h
#ifndef FUNCAO_H
#define FUNCAO_H

#include "utils.h"   // se precisares de constantes como MAX_CANDIDATES

typedef struct {
    int selected[MAX_CANDIDATES];  // genes: 1 se o ponto é escolhido
    int num_selected;              // quantos pontos estão a 1
    double fitness;                // valor da função objetivo (DM)
    int valido;                    // 1 se tem exatamente m pontos, 0 se inválida
} Solution;

// Aqui só pões os PROTÓTIPOS (sem corpo)
double calculate_fitness(Solution *s);
void random_solution(Solution *s);
void copy_solution(Solution *dest, const Solution *src);
void print_solution(const Solution *s);

#endif
// funcao.h