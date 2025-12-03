#ifndef HILLCLIMBING_H
#define HILLCLIMBING_H

#include "structures.h"

// Vizinhança 1: Troca um ponto selecionado por um não selecionado
void neighborhood1(Solution *current, Solution *neighbor, Problem *prob);

// Vizinhança 2: Troca dois pontos selecionados por dois não selecionados
void neighborhood2(Solution *current, Solution *neighbor, Problem *prob);

// Hill Climbing
Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob);

#endif