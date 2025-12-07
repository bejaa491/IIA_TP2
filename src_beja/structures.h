#ifndef STRUCTURES_H
#define STRUCTURES_H

// Constantes do sistema
#define MAX_CANDIDATES 500  // Número máximo de candidatos suportadoTES 500  // Número máximo de candidatos suportado
#define MAX_RUNS 10         // Número máximo de execuções para testesS 10         // Número máximo de execuções para testes
#define INF 1e9             // Valor infinito para comparações#define INF 1e9             // Valor infinito para comparações

// Estrutura para representar uma solução do problemaa representar uma solução do problema
typedef struct {
    int selected[MAX_CANDIDATES];  // Array binário: 1 se ponto está selecionado, 0 caso contrárioá selecionado, 0 caso contrário
    int num_selected;              // Contador de pontos selecionados (deve ser igual a m)ionados (deve ser igual a m)
    double fitness;                // Valor da função objetivo (distância média)fitness;                // Valor da função objetivo (distância média)
} Solution;} Solution;

// Estrutura para os dados do problema de diversidade máximaa os dados do problema de diversidade máxima
typedef struct {
    int C;                                    // Total de candidatos disponíveisis
    int m;                                    // Número de pontos a selecionar (m <= C)cionar (m <= C)
    double dist[MAX_CANDIDATES][MAX_CANDIDATES]; // Matriz simétrica de distâncias entre candidatos dist[MAX_CANDIDATES][MAX_CANDIDATES]; // Matriz simétrica de distâncias entre candidatos
} Problem;} Problem;


#endif#endif