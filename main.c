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
    double all_results[MAX_RUNS];
    int num_runs;
} Statistics;

// Calcula estatísticas de múltiplas execuções
Statistics calculate_statistics(double *results, int num_runs) {
    Statistics stats;
    stats.num_runs = num_runs;
    
    // Inicializa com primeiro valor
    stats.best = results[0];
    stats.worst = results[0];
    stats.avg = 0.0;

    // Copia todos os resultados
    for (int i = 0; i < num_runs; i++) {
        stats.all_results[i] = results[i];
        
        // Atualiza melhor e pior
        if (results[i] > stats.best) stats.best = results[i];
        if (results[i] < stats.worst) stats.worst = results[i];
        
        // Soma para média
        stats.avg += results[i];
    }
    
    // Calcula média
    stats.avg /= num_runs;

    // Calcula desvio padrão
    double variance = 0.0;
    for (int i = 0; i < num_runs; i++) {
        double diff = results[i] - stats.avg;
        variance += diff * diff;
    }
    stats.std_dev = sqrt(variance / num_runs);

    return stats;
}

// Estrutura para guardar resultados dos melhores algoritmos
typedef struct {
    char name[100];
    Statistics stats;
} AlgorithmResult;

// Testa Hill Climbing e retorna melhor configuração
AlgorithmResult test_hill_climbing(Problem *prob, int num_runs, FILE *output, FILE *csv) {
    printf("\n=== TESTE HILL CLIMBING ===\n");
    fprintf(output, "\n=== HILL CLIMBING ===\n");
    fprintf(csv, "\n=== HILL CLIMBING ===\n");
    fprintf(csv, "Configuracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    int iterations[] = {500, 1000, 2000};
    int neighborhoods[] = {1, 2};

    AlgorithmResult best_config;
    best_config.stats.best = -INF;
    strcpy(best_config.name, "HC");

    for (int n = 0; n < 2; n++) {
        for (int it = 0; it < 3; it++) {
            double results[MAX_RUNS];
            char config_name[100];
            snprintf(config_name, 100, "Viz%d_Iter%d", neighborhoods[n], iterations[it]);

            printf("Vizinhanca %d, Iteracoes %d: ", neighborhoods[n], iterations[it]);
            fprintf(output, "\nVizinhanca %d, Iteracoes %d\n", neighborhoods[n], iterations[it]);
            fprintf(csv, "%s,", config_name);

            for (int run = 0; run < num_runs; run++) {
                Solution sol = hill_climbing(iterations[it], neighborhoods[n], prob);
                results[run] = sol.fitness;
                fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
                fprintf(csv, "%.4f,", sol.fitness);
            }

            Statistics stats = calculate_statistics(results, num_runs);
            printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
                   stats.best, stats.avg, stats.std_dev);
            fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
                    stats.best, stats.worst, stats.avg, stats.std_dev);
            fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
                    stats.best, stats.worst, stats.avg, stats.std_dev);

            // Atualiza melhor configuração (baseado na média)
            if (stats.avg > best_config.stats.avg) {
                best_config.stats = stats;
                snprintf(best_config.name, 100, "HC_%s", config_name);
            }
        }
    }

    printf("\n✓ Melhor HC: %s (Media: %.4f)\n", best_config.name, best_config.stats.avg);
    return best_config;
}

// Testa Algoritmo Evolutivo e retorna melhor configuração
AlgorithmResult test_evolutionary(Problem *prob, int num_runs, FILE *output, FILE *csv) {
    printf("\n=== TESTE EVOLUTIVO ===\n");
    fprintf(output, "\n\n=== EVOLUTIVO ===\n");
    fprintf(csv, "\n\n=== EVOLUTIVO ===\n");
    fprintf(csv, "Configuracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    AlgorithmResult best_config;
    best_config.stats.avg = -INF;
    strcpy(best_config.name, "EA");

    // Teste 1: Variando população
    printf("\n--- Variando Populacao ---\n");
    fprintf(output, "\n--- Variando Populacao ---\n");
    fprintf(csv, "\n# Variando Populacao\n");
    
    int pop_sizes[] = {30, 50, 100};
    for (int p = 0; p < 3; p++) {
        double results[MAX_RUNS];
        char config[100];
        snprintf(config, 100, "Pop%d_Gen100", pop_sizes[p]);

        printf("Pop: %d: ", pop_sizes[p]);
        fprintf(output, "\nPopulacao: %d, Geracoes: 100\n", pop_sizes[p]);
        fprintf(csv, "%s,", config);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(pop_sizes[p], 100, 0.8, 0.1, 1, 1, prob);
            results[run] = sol.fitness;
            fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
            fprintf(csv, "%.4f,", sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
                stats.best, stats.avg, stats.std_dev);
        fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
        fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
                stats.best, stats.worst, stats.avg, stats.std_dev);

        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    // Teste 2: Variando probabilidades
    printf("\n--- Variando Probabilidades ---\n");
    fprintf(output, "\n--- Variando Probabilidades ---\n");
    fprintf(csv, "\n# Variando Probabilidades\n");
    
    double cross_probs[] = {0.7, 0.8, 0.9};
    double mut_probs[] = {0.05, 0.1, 0.2};
    
    for (int c = 0; c < 3; c++) {
        for (int m = 0; m < 3; m++) {
            double results[MAX_RUNS];
            char config[100];
            snprintf(config, 100, "Cx%.2f_Mut%.2f", cross_probs[c], mut_probs[m]);

            printf("Cross: %.2f, Mut: %.2f: ", cross_probs[c], mut_probs[m]);
            fprintf(output, "\nCrossover: %.2f, Mutacao: %.2f\n", cross_probs[c], mut_probs[m]);
            fprintf(csv, "%s,", config);

            for (int run = 0; run < num_runs; run++) {
                Solution sol = evolutionary_algorithm(50, 100, cross_probs[c], mut_probs[m], 1, 1, prob);
                results[run] = sol.fitness;
                fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
                fprintf(csv, "%.4f,", sol.fitness);
            }

            Statistics stats = calculate_statistics(results, num_runs);
            printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
                    stats.best, stats.avg, stats.std_dev);
            fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
                     stats.best, stats.worst, stats.avg, stats.std_dev);
            fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
                    stats.best, stats.worst, stats.avg, stats.std_dev);

            if (stats.avg > best_config.stats.avg) {
                best_config.stats = stats;
                snprintf(best_config.name, 100, "EA_%s", config);
            }
        }
    }

    // Teste 3: Comparando seleções
    printf("\n--- Comparando Selecoes ---\n");
    fprintf(output, "\n--- Comparando Selecoes ---\n");
    fprintf(csv, "\n# Comparando Selecoes\n");
    
    char *sel_names[] = {"Torneio", "Roleta"};
    int selections[] = {1, 2};
    
    for (int s = 0; s < 2; s++) {
        double results[MAX_RUNS];
        char config[100];
        snprintf(config, 100, "Sel_%s", sel_names[s]);

        printf("%s: ", sel_names[s]);
        fprintf(output, "\nSelecao: %s\n", sel_names[s]);
        fprintf(csv, "%s,", config);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(50, 100, 0.8, 0.1, selections[s], 1, prob);
            results[run] = sol.fitness;
            fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
            fprintf(csv, "%.4f,", sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
               stats.best, stats.avg, stats.std_dev);
        fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
        fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
                stats.best, stats.worst, stats.avg, stats.std_dev);

        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    // Teste 4: Comparando crossovers
    printf("\n--- Comparando Crossovers ---\n");
    fprintf(output, "\n--- Comparando Crossovers ---\n");
    fprintf(csv, "\n# Comparando Crossovers\n");
    
    char *cross_names[] = {"Uniforme", "UmPonto"};
    int crossovers[] = {1, 2};
    
    for (int c = 0; c < 2; c++) {
        double results[MAX_RUNS];
        char config[100];
        snprintf(config, 100, "Cross_%s", cross_names[c]);

        printf("%s: ", cross_names[c]);
        fprintf(output, "\nCrossover: %s\n", cross_names[c]);
        fprintf(csv, "%s,", config);

        for (int run = 0; run < num_runs; run++) {
            Solution sol = evolutionary_algorithm(50, 100, 0.8, 0.1, 1, crossovers[c], prob);
            results[run] = sol.fitness;
            fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
            fprintf(csv, "%.4f,", sol.fitness);
        }

        Statistics stats = calculate_statistics(results, num_runs);
        printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
               stats.best, stats.avg, stats.std_dev);
        fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
                 stats.best, stats.worst, stats.avg, stats.std_dev);
        fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
                stats.best, stats.worst, stats.avg, stats.std_dev);

        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    printf("\n✓ Melhor EA: %s (Media: %.4f)\n", best_config.name, best_config.stats.avg);
    return best_config;
}

// Testa Híbridos e retorna melhores
void test_hybrids(Problem *prob, int num_runs, FILE *output, FILE *csv,
                  AlgorithmResult *hybrid1_result, AlgorithmResult *hybrid2_result) {
    printf("\n=== TESTE HIBRIDOS ===\n");
    fprintf(output, "\n\n=== HIBRIDOS ===\n");
    fprintf(csv, "\n\n=== HIBRIDOS ===\n");
    fprintf(csv, "Configuracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    // Híbrido 1
    printf("\n--- Hibrido 1 (Evolutivo + HC) ---\n");
    fprintf(output, "\n--- Hibrido 1 ---\n");
    fprintf(csv, "Hibrido1_EA+HC,");
    
    double results_h1[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid1(50, 100, prob);
        results_h1[run] = sol.fitness;
        fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
        fprintf(csv, "%.4f,", sol.fitness);
    }
    
    Statistics stats_h1 = calculate_statistics(results_h1, num_runs);
    printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
           stats_h1.best, stats_h1.avg, stats_h1.std_dev);
    fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
            stats_h1.best, stats_h1.worst, stats_h1.avg, stats_h1.std_dev);
    fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
            stats_h1.best, stats_h1.worst, stats_h1.avg, stats_h1.std_dev);

    strcpy(hybrid1_result->name, "Hibrido1_EA+HC");
    hybrid1_result->stats = stats_h1;

    // Híbrido 2
    printf("\n--- Hibrido 2 (HC + Evolutivo) ---\n");
    fprintf(output, "\n--- Hibrido 2 ---\n");
    fprintf(csv, "Hibrido2_HC+EA,");
    
    double results_h2[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid2(1000, prob);
        results_h2[run] = sol.fitness;
        fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
        fprintf(csv, "%.4f,", sol.fitness);
    }
    
    Statistics stats_h2 = calculate_statistics(results_h2, num_runs);
    printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
           stats_h2.best, stats_h2.avg, stats_h2.std_dev);
    fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
            stats_h2.best, stats_h2.worst, stats_h2.avg, stats_h2.std_dev);
    fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
            stats_h2.best, stats_h2.worst, stats_h2.avg, stats_h2.std_dev);

    strcpy(hybrid2_result->name, "Hibrido2_HC+EA");
    hybrid2_result->stats = stats_h2;

    // Híbrido 3
    printf("\n--- Hibrido 3 (EA com refinamento local) ---\n");
    fprintf(output, "\n--- Hibrido 3 ---\n");
    fprintf(csv, "Hibrido3_EA_Refinado,");
    
    double results_h3[MAX_RUNS];
    for (int run = 0; run < num_runs; run++) {
        Solution sol = hybrid3(50, 100, prob);
        results_h3[run] = sol.fitness;
        fprintf(output, "  Run %d: %.4f\n", run + 1, sol.fitness);
        fprintf(csv, "%.4f,", sol.fitness);
    }
    
    Statistics stats_h3 = calculate_statistics(results_h3, num_runs);
    printf("Melhor: %.4f, Media: %.4f (±%.4f)\n", 
           stats_h3.best, stats_h3.avg, stats_h3.std_dev);
    fprintf(output, "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n",
            stats_h3.best, stats_h3.worst, stats_h3.avg, stats_h3.std_dev);
    fprintf(csv, "%.4f,%.4f,%.4f,%.4f\n", 
            stats_h3.best, stats_h3.worst, stats_h3.avg, stats_h3.std_dev);
}

// Gera tabela comparativa final
void generate_final_comparison(AlgorithmResult hc, AlgorithmResult ea,
                                AlgorithmResult h1, AlgorithmResult h2,
                                FILE *output, FILE *csv) {
    printf("\n\n╔════════════════════════════════════════════════════════╗\n");
    printf("║          COMPARAÇÃO FINAL DOS ALGORITMOS              ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    fprintf(output, "\n\n");
    fprintf(output, "=====================================\n");
    fprintf(output, "    COMPARAÇÃO FINAL DOS ALGORITMOS\n");
    fprintf(output, "=====================================\n\n");

    fprintf(csv, "\n\n=== COMPARACAO FINAL ===\n");
    fprintf(csv, "Algoritmo,Melhor,Pior,Media,Desvio\n");

    AlgorithmResult results[] = {hc, ea, h1, h2};
    char *labels[] = {"Melhor HC", "Melhor EA", "Hibrido 1", "Hibrido 2"};

    printf("%-20s | %10s | %10s | %10s | %10s\n", 
           "Algoritmo", "Melhor", "Pior", "Média", "Desvio");
    printf("-----|------------|------------|------------|------------|\n");

    fprintf(output, "%-20s | %10s | %10s | %10s | %10s\n", 
           "Algoritmo", "Melhor", "Pior", "Media", "Desvio");
    fprintf(output, "-----+------------+------------+------------+------------+\n");

    for (int i = 0; i < 4; i++) {
        printf("%-20s | %10.4f | %10.4f | %10.4f | %10.4f\n",
               labels[i], results[i].stats.best, results[i].stats.worst,
               results[i].stats.avg, results[i].stats.std_dev);

        fprintf(output, "%-20s | %10.4f | %10.4f | %10.4f | %10.4f\n",
               labels[i], results[i].stats.best, results[i].stats.worst,
               results[i].stats.avg, results[i].stats.std_dev);

        fprintf(csv, "%s,%.4f,%.4f,%.4f,%.4f\n",
               labels[i], results[i].stats.best, results[i].stats.worst,
               results[i].stats.avg, results[i].stats.std_dev);
    }

    printf("\n");
    fprintf(output, "\n");

    // Encontra melhor algoritmo
    int best_idx = 0;
    for (int i = 1; i < 4; i++) {
        if (results[i].stats.avg > results[best_idx].stats.avg) {
            best_idx = i;
        }
    }

    printf("🏆 MELHOR ALGORITMO: %s (Média: %.4f)\n", 
           labels[best_idx], results[best_idx].stats.avg);
    fprintf(output, "\nMELHOR ALGORITMO: %s (Media: %.4f)\n", 
           labels[best_idx], results[best_idx].stats.avg);
}

int main(int argc, char *argv[]) {
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

    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║     OTIMIZAÇÃO - PROBLEMA DE DIVERSIDADE MÁXIMA        ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
    printf("Ficheiro: %s\n", argv[1]);
    printf("Problema: C=%d candidaturas, m=%d pontos a selecionar\n", prob.C, prob.m);
    printf("Execuções por configuração: %d\n", num_runs);

    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    printf("Seed aleatória: %u\n", seed);

    // Cria ficheiros de saída
    char output_filename[256], csv_filename[256];
    snprintf(output_filename, sizeof(output_filename), 
             "resultados_%s_%druns.txt", argv[1], num_runs);
    snprintf(csv_filename, sizeof(csv_filename), 
             "resultados_%s_%druns.csv", argv[1], num_runs);

    FILE *output = fopen(output_filename, "w");
    FILE *csv = fopen(csv_filename, "w");
    
    if (!output || !csv) {
        printf("Erro ao criar ficheiros de resultados\n");
        return 1;
    }

    fprintf(output, "RESULTADOS - PROBLEMA DE DIVERSIDADE MAXIMA\n");
    fprintf(output, "Ficheiro: %s\n", argv[1]);
    fprintf(output, "C=%d, m=%d\n", prob.C, prob.m);
    fprintf(output, "Execucoes: %d\n", num_runs);
    fprintf(output, "Seed: %u\n", seed);
    fprintf(output, "=====================================\n");

    fprintf(csv, "RESULTADOS,%s\n", argv[1]);
    fprintf(csv, "C,%d,m,%d,Runs,%d,Seed,%u\n", prob.C, prob.m, num_runs, seed);

    // Executa testes
    printf("\n");
    AlgorithmResult best_hc = test_hill_climbing(&prob, num_runs, output, csv);
    AlgorithmResult best_ea = test_evolutionary(&prob, num_runs, output, csv);
    
    AlgorithmResult hybrid1_result, hybrid2_result;
    test_hybrids(&prob, num_runs, output, csv, &hybrid1_result, &hybrid2_result);

    // Gera comparação final
    generate_final_comparison(best_hc, best_ea, hybrid1_result, hybrid2_result, 
                             output, csv);

    fclose(output);
    fclose(csv);

    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║                  TESTES CONCLUÍDOS!                    ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
    printf("📄 Relatório texto: %s\n", output_filename);
    printf("📊 Dados CSV: %s\n", csv_filename);
    printf("\n💡 Importa o CSV no Excel para análise completa!\n\n");

    return 0;
}