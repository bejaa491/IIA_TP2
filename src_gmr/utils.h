#ifndef UTILS_H
#define UTILS_H

// Estrutura para parâmetros do EA (por agora pode ser “dummy” e afinamos depois)
struct info {
    int numGenes;   // nº de genes (depois ligamos a prob.C)
    int popsize;    // tamanho da população
    int max_gen;    // nº máx. de gerações
    float pm;       // prob. mutação
    float pr;       // prob. crossover
    int tsize;      // tamanho do torneio, se usares tournament selection
};

// Protótipos das funções definidas em utils.c
struct info init_data(char *s, int mat[][2]);
void init_rand(void);
int  flip(void);
int  random_l_h(int min, int max);
float rand_01(void);

#endif // UTILS_H