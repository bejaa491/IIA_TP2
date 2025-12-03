#ifndef HYBRID_H
#define HYBRID_H

#include "structures.h"

// Híbrido 1: Evolutivo + Hill Climbing nas melhores soluções
Solution hybrid1(int pop_size, int generations, Problem *prob);

// Híbrido 2: Hill Climbing + Evolutivo para escapar de ótimos locais
Solution hybrid2(int iterations, Problem *prob);

#endif