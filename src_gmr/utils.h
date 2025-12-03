#ifndef UTILS_H
#define UTILS_H

#define MAX_CANDIDATES 100

typedef struct {
    int C;
    int m;
    double dist[MAX_CANDIDATES][MAX_CANDIDATES];
} Problem;

extern Problem prob;

// Leitura dos dados do problema
int init_data(const char *filename);

// Funções utilitárias
void init_rand(void);
int  flip(void);
int  random_l_h(int min, int max);
float rand_01(void);

#endif
