#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "structures.h"
#include "utils.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "hybrid.h"

// Estrutura para armazenar resultados
typedef struct {
    double best;
    double worst;
    double avg;
    double std_dev;
} Statistics;

// Calcula estatísticas de múltiplas execuções
Statistics calculate_statistics(double *results, int num_runs) {
    Statistics stats;
    stats.best = results[0];
    stats.worst = results[0];
    stats.avg = 0.0;

    for (int i = 0; i < num_runs; i++) {
        if (results[i] > stats.best) stats.best = results[i];
        if (results[i] < stats.worst) stats.worst = results[i];
        stats.avg += results[i];
    }
    stats.avg /= num_runs;

    // Calcula desvio padrão
    double variance = 0.0;
    for (int i = 0; i < num_runs; i++) {
        variance += (results[i] - stats.avg) * (results[i] - stats.avg);
    }
    stats.std_dev = sqrt(variance / num_runs);

    return stats;
}

// Teste de Hill Climbing com múltiplas execuções
void test_hill_climbing(Problem *prob, int num_runs, FILE *output) {
    printf("\n=== TESTE HILL CLIMBING ===\n");
    fprintf(output, "\n=== HILL CLIMBING ===\n");

    int iterations[] = {500, 1000, 2000};
    int neighborhoods[] = {1, 2};

    for (int n = 0; n < 2; n++) {
        for (int it = 0; it < 3; it++) {
            double results[MAX_RUNS];

            printf("Vizinanca %d, Iteracoes %d: ", neighborhoods[n], iterations[it]);
            fprintf(output, "Vizinanca %d, Iteracoes %d\n", neighborhoods[n], iterations[it]);

            for (int run = 0; run < num_runs; run++) {
                Solution sol = hill_climbing(iterations[it], neighborhoods[n], prob);
                results[run] = sol.fitness;
                fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
            }

            Statistics stats = calculate_statistics(results, num_runs);
            printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
                   stats.best, stats.avg, stats.std_dev);
            fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
                    stats.best, stats.worst, stats.avg, stats.std_dev);
        }
    }
}

// Teste de Algoritmo Evolutivo com múltiplas execuções
void test_evolutionary(Problem *prob, int num_runs, FILE *output) {
    printf("\n=== TESTE EVOLUTIVO ===\n");
    fprintf(output, "\n=== EVOLUTIVO ===\n");

    int pop_sizes[] = {30, 50, 100};
    int generations[] = {50, 100, 200};
    double cross_probs[] = {0.7, 0.8, 0.9};
    double mut_probs[] = {0.05, 0.1, 0.2};
    int selections[] = {1, 2}; // 1=torneio, 2=roleta
    int crossovers[] = {1, 2}; // 1=uniforme, 2=um ponto

    // Teste variando população
    printf("\n--- Variando Populacao ---\n");
    fprintf(output, "\n--- Variando Populacao ---\n");
    for (int p = 0; p < 3; p++) {
        double results[MAX_RUNS];
        printf("Pop: %d, Gen: 100: ", pop_sizes[p]);
        fprintf(output, "Populacao: %d, Geracoes: 100\n", pop_sizes[p]);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(pop_sizes[p], 100, 0.8, 0.1, 1, 1, prob);
            results[run] = sol.fitness;
            fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
                stats.best, stats.avg, stats.std_dev);
        fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
    }

    // Teste variando probabilidades
    printf("\n--- Variando Probabilidades ---\n");
    fprintf(output, "\n--- Variando Probabilidades ---\n");
    for (int c = 0; c < 3; c++) {
        for (int m = 0; m < 3; m++) {
            double results[MAX_RUNS];
            printf("Cross: %.2f, Mut: %.2f: ", cross_probs[c], mut_probs[m]);
            fprintf(output, "Crossover: %.2f, Mutacao: %.2f\n", cross_probs[c], mut_probs[m]);

            for (int run = 0; run < num_runs; run++) {
                Solution sol = evolutionary_algorithm(50, 100, cross_probs[c], mut_probs[m], 1, 1, prob);
                results[run] = sol.fitness;
                fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
            }

            Statistics stats = calculate_statistics(results, num_runs);
            printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
                    stats.best, stats.avg, stats.std_dev);
            fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
                     stats.best, stats.worst, stats.avg, stats.std_dev);
        }
    }

    // Teste comparando seleções
    printf("\n--- Comparando Metodos de Selecao ---\n");
    fprintf(output, "\n--- Comparando Metodos de Selecao ---\n");
    for (int s = 0; s < 2; s++) {
        double results[MAX_RUNS];
        char *sel_name = (selections[s] == 1) ? "Torneio" : "Roleta";
        printf("%s: ", sel_name);
        fprintf(output, "Selecao: %s\n", sel_name);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(50, 100, 0.8, 0.1, selections[s], 1, prob);
            results[run] = sol.fitness;
            fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
               stats.best, stats.avg, stats.std_dev);
        fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
    }

    // Teste comparando crossovers
    printf("\n--- Comparando Operadores de Recombinacao ---\n");
    fprintf(output, "\n--- Comparando Operadores de Recombinacao ---\n");
    for (int c = 0; c < 2; c++) {
        double results[MAX_RUNS];
        char *cross_name = (crossovers[c] == 1) ? "Uniforme" : "Um Ponto";
        printf("%s: ", cross_name);
        fprintf(output, "Recombinacao: %s\n", cross_name);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(50, 100, 0.8, 0.1, 1, crossovers[c], prob);
            results[run] = sol.fitness;
            fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
               stats.best, stats.avg, stats.std_dev);
        fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
    }
}

// Teste de Híbridos com múltiplas execuções
void test_hybrids(Problem *prob, int num_runs, FILE *output) {
    printf("\n=== TESTE HIBRIDOS ===\n");
    fprintf(output, "\n=== HIBRIDOS ===\n");

    // Híbrido 1
    printf("\n--- Hibrido 1 (Evolutivo + HC) ---\n");
    fprintf(output, "\n--- Hibrido 1 ---\n");
    double results_h1[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid1(50, 100, prob);
        results_h1[run] = sol.fitness;
        fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
    }
    Statistics stats_h1 = calculate_statistics(results_h1, num_runs);
    printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
           stats_h1.best, stats_h1.avg, stats_h1.std_dev);
    fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
            stats_h1.best, stats_h1.worst, stats_h1.avg, stats_h1.std_dev);

    // Híbrido 2
    printf("\n--- Hibrido 2 (HC + Evolutivo) ---\n");
    fprintf(output, "\n--- Hibrido 2 ---\n");
    double results_h2[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid2(1000, prob);
        results_h2[run] = sol.fitness;
        fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
    }
    Statistics stats_h2 = calculate_statistics(results_h2, num_runs);
    printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
           stats_h2.best, stats_h2.avg, stats_h2.std_dev);
    fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
            stats_h2.best, stats_h2.worst, stats_h2.avg, stats_h2.std_dev);

    // Híbrido 3
    printf("\n--- Hibrido 3 (Evolutivo com refinamento) ---\n");
    fprintf(output, "\n--- Hibrido 3 ---\n");
    double results_h3[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid3(50, 100, prob);
        results_h3[run] = sol.fitness;
        fprintf(output, "Run %d: %.2f\n", run + 1, sol.fitness);
    }
    Statistics stats_h3 = calculate_statistics(results_h3, num_runs);
    printf("Melhor: %.2f, Media: %.2f (+-%.2f)\n", 
           stats_h3.best, stats_h3.avg, stats_h3.std_dev);
    fprintf(output, "Melhor: %.2f, Pior: %.2f, Media: %.2f, Desvio: %.2f\n\n",
            stats_h3.best, stats_h3.worst, stats_h3.avg, stats_h3.std_dev);
}

int main(int argc, char *argv[]) {
    // seed e debug serão APÓS ler o problema (evita usar 'prob' antes de declarar)
    if (argc < 2) {
        printf("Uso: %s <ficheiro_entrada> [num_runs]\n", argv[0]);
        printf("Exemplo: %s tourism_5.txt 10\n", argv[0]);
        return 1;
    }

    int num_runs = 10;
    if (argc >= 3) {
        num_runs = atoi(argv[2]);
        if (num_runs < 1 || num_runs > MAX_RUNS) {
            printf("Numero de execucoes deve estar entre 1 e %d\n", MAX_RUNS);
            return 1;
        }
    }

    Problem prob;
    if (!read_file(argv[1], &prob)) {
        return 1;
    }
    printf("Ficheiro %s lido com sucesso.\n", argv[1]);
    printf("Problema: C=%d, m=%d\n", prob.C, prob.m);
    printf("Numero de execucoes: %d\n", num_runs);
 
    // Debug: imprime seed e gera algumas solucoes aleatorias para checar diversidade
    unsigned int debug_seed = (unsigned int)time(NULL);
    printf("DEBUG seed: %u\n", debug_seed);
    srand(debug_seed);
    for (int i = 0; i < 3; ++i) {
        Solution s;
        random_solution(&s, &prob);
        printf("DEBUG random sol %d: num_selected=%d fitness=%.4f\n", i+1, s.num_selected, s.fitness);
    }

    // Cria ficheiro de saída
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "resultados_%s_%d_runs.txt", 
             argv[1], num_runs);
    FILE *output = fopen(output_filename, "w");
    if (!output) {
        printf("Erro ao criar ficheiro de resultados\n");
        return 1;
    }

    fprintf(output, "RESULTADOS - Ficheiro: %s\n", argv[1]);
    fprintf(output, "C=%d, m=%d\n", prob.C, prob.m);
    fprintf(output, "Numero de execucoes: %d\n", num_runs);
    fprintf(output, "=====================================\n");

    // Executa testes
    test_hill_climbing(&prob, num_runs, output);
    test_evolutionary(&prob, num_runs, output);
    test_hybrids(&prob, num_runs, output);

    fclose(output);
    printf("\n\nResultados guardados em: %s\n", output_filename);

    return 0;
}