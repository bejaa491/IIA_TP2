#ifndef HYBRID_H
#define HYBRID_H

#include "structures.h"

// Híbrido 1: Evolutivo + Hill Climbing na melhor solução
Solution hybrid1(int pop_size, int generations, Problem *prob);

// Híbrido 2: Hill Climbing + Evolutivo para diversificar
Solution hybrid2(int iterations, Problem *prob);

// Híbrido 3: Evolutivo com refinamento local em cada geração
Solution hybrid3(int pop_size, int generations, Problem *prob);

#endif