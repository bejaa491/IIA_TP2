#include "experiments.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

// Calcula estatísticas (melhor, pior, média, desvio padrão) de um conjunto de resultados
static Statistics calculate_statistics_local(double *results, int num_runs) {
    Statistics stats;
    stats.num_runs = num_runs;
    stats.best = results[0];
    stats.worst = results[0];
    stats.avg = 0.0;
    
    // Encontra melhor, pior e calcula soma para média
    for (int i = 0; i < num_runs; i++) {
        stats.all_results[i] = results[i];
        if (results[i] > stats.best) stats.best = results[i];
        if (results[i] < stats.worst) stats.worst = results[i];
        stats.avg += results[i];
    }
    
    stats.avg /= num_runs; // Média
    
    // Calcula desvio padrão
    double variance = 0.0;
    for (int i = 0; i < num_runs; i++) {
        double diff = results[i] - stats.avg;
        variance += diff * diff;
    }
    stats.std_dev = sqrt(variance / num_runs);
    
    return stats;
}

// Executa algoritmo num_runs vezes e escreve resultados em arquivos
// Retorna estatísticas agregadas
Statistics run_trials(const char *label, AlgRunner runner, void *args,
                      int num_runs, FILE *output, FILE *csv, Problem *prob) {
    double results[MAX_RUNS];
    
    // Escreve cabeçalho nos arquivos
    fprintf(output, "%s,", label);
    fprintf(csv, "%s,", label);

    // Executa algoritmo num_runs vezes
    for (int run = 0; run < num_runs; run++) {
        Solution sol = runner(args, prob);
        results[run] = sol.fitness;
        
        // Log de cada execução
        fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
        fprintf(csv, "%.4f,", sol.fitness);
    }

    // Calcula estatísticas agregadas
    Statistics stats = calculate_statistics_local(results, num_runs);

    // Escreve resumo estatístico
    fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
            stats.best, stats.worst, stats.avg, stats.std_dev);
    fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n",
            stats.best, stats.worst, stats.avg, stats.std_dev);

    return stats;
}