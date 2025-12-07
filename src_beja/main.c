#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "structures.h"
#include "utils.h"
#include "hillclimbing.h"
#include "evolutionary.h"
#include "hybrid.h"
#include "experiments.h"

/* Estruturas auxiliares para passar argumentos aos runners e guardar resultados */
typedef struct {
    char name[100];
    Statistics stats;
} AlgorithmResult;

typedef struct { int iterations; int neighborhood; } HCArgs;
typedef struct {
    int pop_size;
    int generations;
    double cross_prob;
    double mut_prob;
    int selection_type;
    int crossover_type;
} EAArgs;
typedef struct { int type; int param1; int param2; } HybridArgs;

/* Wrappers usados por experiments.run_trials:
   args é um ponteiro para a estrutura específica do algoritmo. */

/* Executa hill climbing com argumentos passados em args */
static Solution run_hill_climbing(void *args, Problem *prob) {
    HCArgs *a = (HCArgs*)args;
    return hill_climbing(a->iterations, a->neighborhood, prob);
}

/* Executa algoritmo evolutivo com parâmetros fornecidos em args */
static Solution run_ea(void *args, Problem *prob) {
    EAArgs *a = (EAArgs*)args;
    return evolutionary_algorithm(a->pop_size, a->generations,
                                  a->cross_prob, a->mut_prob,
                                  a->selection_type, a->crossover_type, prob);
}

/* Dispara um dos híbridos conforme tipo em args */
static Solution run_hybrid(void *args, Problem *prob) {
    HybridArgs *a = (HybridArgs*)args;
    if (a->type == 1) return hybrid1(a->param1, a->param2, prob);
    if (a->type == 2) return hybrid2(a->param1, prob);
    return hybrid3(a->param1, a->param2, prob);
}

/* Testes de Hill Climbing: varre configurações e retorna melhor configuração */
AlgorithmResult test_hill_climbing(Problem *prob, int num_runs, FILE *output, FILE *csv) {
    printf("\n=== HILL CLIMBING ===\n");
    fprintf(output, "\n=== HILL CLIMBING ===\n");
    fprintf(csv, "\n=== HILL CLIMBING ===\nConfiguracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    int iterations[] = {500, 1000, 2000};
    int neighborhoods[] = {1, 2};

    AlgorithmResult best_config;
    best_config.stats.avg = -INF;
    best_config.name[0] = '\0'; /* inicializa string para evitar lixo */

    for (int n = 0; n < 2; n++) {
        for (int it = 0; it < 3; it++) {
            char config_name[100];
            snprintf(config_name, 100, "Viz%d_Iter%d", neighborhoods[n], iterations[it]);
            
            HCArgs args = { iterations[it], neighborhoods[n] };
            Statistics stats = run_trials(config_name, run_hill_climbing, &args, num_runs, output, csv, prob);

            printf("%s: Melhor: %.4f, Media: %.4f\n", config_name, stats.best, stats.avg);

            if (stats.avg > best_config.stats.avg) {
                best_config.stats = stats;
                snprintf(best_config.name, 100, "HC_%s", config_name);
            }
        }
    }

    printf(">> Melhor HC: %s (Media: %.4f)\n", best_config.name, best_config.stats.avg);
    return best_config;
}

/* Testes do algoritmo evolutivo: varre parâmetros e retorna melhor configuração */
AlgorithmResult test_evolutionary(Problem *prob, int num_runs, FILE *output, FILE *csv) {
    printf("\n=== EVOLUTIVO ===\n");
    fprintf(output, "\n\n=== EVOLUTIVO ===\n");
    fprintf(csv, "\n\n=== EVOLUTIVO ===\nConfiguracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    AlgorithmResult best_config;
    best_config.stats.avg = -INF;
    best_config.name[0] = '\0';

    int pop_sizes[] = {30, 50, 100};
    double cross_probs[] = {0.7, 0.8, 0.9};
    double mut_probs[] = {0.05, 0.1, 0.2};
    char *sel_names[] = {"Torneio", "Roleta"};
    int selections[] = {1, 2};
    char *cross_names[] = {"Uniforme", "UmPonto"};
    int crossovers[] = {1, 2};

    /* Varia tamanho de população */
    for (int p = 0; p < 3; p++) {
        char config[100];
        snprintf(config, 100, "Pop%d", pop_sizes[p]);
        EAArgs args = { pop_sizes[p], 100, 0.8, 0.1, 1, 1 };
        Statistics stats = run_trials(config, run_ea, &args, num_runs, output, csv, prob);
        printf("%s: %.4f\n", config, stats.avg);
        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    /* Varia probabilidades de crossover e mutação */
    for (int c = 0; c < 3; c++) {
        for (int m = 0; m < 3; m++) {
            char config[100];
            snprintf(config, 100, "Cx%.2f_Mut%.2f", cross_probs[c], mut_probs[m]);
            EAArgs args = { 50, 100, cross_probs[c], mut_probs[m], 1, 1 };
            Statistics stats = run_trials(config, run_ea, &args, num_runs, output, csv, prob);
            if (stats.avg > best_config.stats.avg) {
                best_config.stats = stats;
                snprintf(best_config.name, 100, "EA_%s", config);
            }
        }
    }

    /* Varia método de seleção */
    for (int s = 0; s < 2; s++) {
        char config[100];
        snprintf(config, 100, "Sel_%s", sel_names[s]);
        EAArgs args = { 50, 100, 0.8, 0.1, selections[s], 1 };
        Statistics stats = run_trials(config, run_ea, &args, num_runs, output, csv, prob);
        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    /* Varia operador de crossover */
    for (int c = 0; c < 2; c++) {
        char config[100];
        snprintf(config, 100, "Cross_%s", cross_names[c]);
        EAArgs args = { 50, 100, 0.8, 0.1, 1, crossovers[c] };
        Statistics stats = run_trials(config, run_ea, &args, num_runs, output, csv, prob);
        if (stats.avg > best_config.stats.avg) {
            best_config.stats = stats;
            snprintf(best_config.name, 100, "EA_%s", config);
        }
    }

    printf(">> Melhor EA: %s (Media: %.4f)\n", best_config.name, best_config.stats.avg);
    return best_config;
}

/* Testes dos híbridos: preenche dois AlgorithmResult de saída e loga terceiro */
void test_hybrids(Problem *prob, int num_runs, FILE *output, FILE *csv,
                  AlgorithmResult *h1_result, AlgorithmResult *h2_result) {
    printf("\n=== HIBRIDOS ===\n");
    fprintf(output, "\n\n=== HIBRIDOS ===\n");
    fprintf(csv, "\n\n=== HIBRIDOS ===\nConfiguracao,");
    for (int i = 0; i < num_runs; i++) fprintf(csv, "Run%d,", i+1);
    fprintf(csv, "Melhor,Pior,Media,Desvio\n");

    HybridArgs h1 = {1, 50, 100};
    Statistics stats_h1 = run_trials("Hibrido1_EA+HC", run_hybrid, &h1, num_runs, output, csv, prob);
    strcpy(h1_result->name, "Hibrido1_EA+HC");
    h1_result->stats = stats_h1;

    HybridArgs h2 = {2, 1000, 0};
    Statistics stats_h2 = run_trials("Hibrido2_HC+EA", run_hybrid, &h2, num_runs, output, csv, prob);
    strcpy(h2_result->name, "Hibrido2_HC+EA");
    h2_result->stats = stats_h2;

    /* Executa Hibrido3 apenas para log (não guardamos resultado agregado aqui) */
    HybridArgs h3 = {3, 50, 100};
    run_trials("Hibrido3_EA_Refinado", run_hybrid, &h3, num_runs, output, csv, prob);
}

/* Gera comparação final entre melhores configurações encontradas */
void generate_final_comparison(AlgorithmResult hc, AlgorithmResult ea,
                                AlgorithmResult h1, AlgorithmResult h2,
                                FILE *output, FILE *csv) {
    printf("\n========================================\n");
    printf("   COMPARACAO FINAL DOS ALGORITMOS\n");
    printf("========================================\n\n");

    fprintf(output, "\n\n=====================================\n");
    fprintf(output, "    COMPARACAO FINAL DOS ALGORITMOS\n");
    fprintf(output, "=====================================\n\n");

    fprintf(csv, "\n\n=== COMPARACAO FINAL ===\n");
    fprintf(csv, "Algoritmo,Melhor,Pior,Media,Desvio\n");

    AlgorithmResult results[] = {hc, ea, h1, h2};
    char *labels[] = {"Melhor HC", "Melhor EA", "Hibrido 1", "Hibrido 2"};

    printf("%-20s | %10s | %10s | %10s | %10s\n", 
           "Algoritmo", "Melhor", "Pior", "Media", "Desvio");
    printf("---------------------|------------|------------|------------|------------|\n");

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

    int best_idx = 0;
    for (int i = 1; i < 4; i++) {
        if (results[i].stats.avg > results[best_idx].stats.avg) {
            best_idx = i;
        }
    }

    printf("\n>> MELHOR ALGORITMO: %s (Media: %.4f)\n", 
           labels[best_idx], results[best_idx].stats.avg);
    fprintf(output, "\nMELHOR ALGORITMO: %s (Media: %.4f)\n", 
           labels[best_idx], results[best_idx].stats.avg);
}

/* Função main: valida argumentos, carrega problema, executa séries de testes e grava resultados */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <ficheiro_entrada> [num_runs]\n", argv[0]);
        return 1;
    }

    int num_runs = (argc >= 3) ? atoi(argv[2]) : 10;
    if (num_runs < 1 || num_runs > MAX_RUNS) num_runs = MAX_RUNS;

    Problem prob;
    if (!read_file(argv[1], &prob)) return 1;

    printf("========================================\n");
    printf("  PROBLEMA DE DIVERSIDADE MAXIMA\n");
    printf("========================================\n");
    printf("Ficheiro: %s\n", argv[1]);
    printf("C=%d, m=%d, Runs=%d\n", prob.C, prob.m, num_runs);

    /* Semente baseada no tempo (registar seed no output para reprodutibilidade) */
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);

    char output_filename[256], csv_filename[256];
    snprintf(output_filename, 256, "resultados_%s_%druns.txt", argv[1], num_runs);
    snprintf(csv_filename, 256, "resultados_%s_%druns.csv", argv[1], num_runs);

    FILE *output = fopen(output_filename, "w");
    FILE *csv = fopen(csv_filename, "w");
    
    if (!output || !csv) {
        printf("Erro ao criar ficheiros de resultados\n");
        return 1;
    }

    fprintf(output, "RESULTADOS - C=%d, m=%d, Runs=%d, Seed=%u\n", 
            prob.C, prob.m, num_runs, seed);
    fprintf(csv, "C,%d,m,%d,Runs,%d,Seed,%u\n", prob.C, prob.m, num_runs, seed);

    /* Executa conjuntos de testes e coleta melhores configurações */
    AlgorithmResult best_hc = test_hill_climbing(&prob, num_runs, output, csv);
    AlgorithmResult best_ea = test_evolutionary(&prob, num_runs, output, csv);
    AlgorithmResult h1, h2;
    test_hybrids(&prob, num_runs, output, csv, &h1, &h2);
    generate_final_comparison(best_hc, best_ea, h1, h2, output, csv);

    fclose(output);
    fclose(csv);

    printf("\n========================================\n");
    printf("Resultados: %s e %s\n", output_filename, csv_filename);
    printf("========================================\n\n");

    return 0;
}