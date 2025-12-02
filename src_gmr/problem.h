// problem.h
#ifndef PROBLEM_H
#define PROBLEM_H

#define MAX_CANDIDATES 500

typedef struct {
    int C;
    int m;
    double dist[MAX_CANDIDATES][MAX_CANDIDATES];
} Problem;

// Diz ao resto do código que *existe* um Problem prob em algum lado
extern Problem prob;

// Protótipo da função (só a assinatura)
int read_file(const char *filename);

#endif
