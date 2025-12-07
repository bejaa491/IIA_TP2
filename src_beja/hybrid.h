#ifndef HYBRID_H
#define HYBRID_H

#include "structures.h"

// Híbrido 1: Evolutivo seguido de Hill Climbing na melhor solução
// Estratégia: exploração global (EA) + refinamento local (HC)
Solution hybrid1(int pop_size, int generations, Problem *prob);

// Híbrido 2: Múltiplos ciclos de Hill Climbing + Evolutivo
// Estratégia: intensificação (HC) alternada com diversificação (EA)
Solution hybrid2(int iterations, Problem *prob);

// Híbrido 3: Evolutivo com refinamento local em melhores indivíduos de cada geração
// Estratégia: memético (EA com busca local integrada)
Solution hybrid3(int pop_size, int generations, Problem *prob);

#endif