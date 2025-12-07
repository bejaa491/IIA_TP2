#ifndef HILLCLIMBING_H
#define HILLCLIMBING_H

#include "structures.h"

// Vizinhança 1: Troca aleatoriamente 1 ponto selecionado por 1 não selecionado
void neighborhood1(Solution *current, Solution *neighbor, Problem *prob);

// Vizinhança 2: Troca aleatoriamente 2 pontos selecionados por 2 não selecionados
void neighborhood2(Solution *current, Solution *neighbor, Problem *prob);

// Hill Climbing básico com vizinhança escolhida (aceita melhorias ou iguais)
// max_iter: número máximo de iterações
// neighborhood_type: 1 ou 2 para escolher vizinhança
Solution hill_climbing(int max_iter, int neighborhood_type, Problem *prob);

// Hill Climbing com múltiplos restarts para escapar de ótimos locais
// num_restarts: número de reinicializações
Solution hill_climbing_restart(int max_iter, int neighborhood_type, int num_restarts, Problem *prob);

#endif