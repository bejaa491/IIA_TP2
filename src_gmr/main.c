#include <stdio.h>
#include <math.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

int main_1(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <ficheiro_tourism>\n", argv[0]);
        return 1;
    }

    if (!init_data(argv[1]))
    {
        return 1;
    }

    init_rand();

    printf("Problema carregado: C = %d, m = %d\n", prob.C, prob.m);

    int runs = 10;
    int iteracoes = 1000;
    double soma_fitness = 0.0;
    Solution best_global;
    best_global.fitness = -1.0;

    printf("=== Teste Vizinhança 1 (SWAP 1) ===\n");
    for (int i = 0; i < runs; i++)
    {
        // Nota: atualizei a chamada para incluir o tipo de vizinhança
        Solution s = hill_climbing(iteracoes, 2);

        printf("Run %d: %.2f\n", i + 1, s.fitness);
        soma_fitness += s.fitness;

        if (s.fitness > best_global.fitness)
            copy_solution(&best_global, &s);
    }

    printf("\nMedia: %.2f\n", soma_fitness / runs);
    printf("Melhor encontrada:\n");
    print_solution(&best_global);

    /*  Solution s;
     random_solution(&s);
     print_solution(&s);


     Solution best = hill_climbing(1000);
     printf("=== HILL CLIMBING ===\n");
     print_solution(&best); */
    /*
            Solution s0;
        random_solution(&s0);
        printf("Solucao inicial:\n");
        print_solution(&s0);

        Solution best = hill_climbing_from(s0, 1000);
        printf("\n=== HILL CLIMBING ===\n");
        printf("Solucao final:\n");
        print_solution(&best); */

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <ficheiro_tourism>\n", argv[0]);
        return 1;
    }

    if (!init_data(argv[1]))
    {
        return 1;
    }

    init_rand();

    printf("Problema carregado: C = %d, m = %d\n", prob.C, prob.m);
    

    // Parâmetros a testar
    double alphas[] = {0.90, 0.95, 0.99};
    int num_alphas = 3;
    int num_runs = 10; // O enunciado pede pelo menos 10 repetições

    printf("=== INICIO DO ESTUDO EXPERIMENTAL ===\n");

    // 1. Ciclo para variar o parâmetro (Alpha)
    for (int a = 0; a < num_alphas; a++)
    {
        double current_alpha = alphas[a];
        double tmax = 100.0;
        double tmin = 0.1;

        double soma_fitness = 0.0;
        Solution best_of_all;
        best_of_all.fitness = -1.0; // Inicializar com valor baixo

        printf("\nTestando Alpha: %.2f | Tmax: %.1f | Tmin: %.1f\n", current_alpha, tmax, tmin);

        // 2. Ciclo das Repetições (Runs)
        for (int run = 0; run < num_runs; run++)
        {

            // Chama a tua função (nota: adiciona os argumentos que faltam se necessário)
            Solution s = simulated_annealing(tmax, tmin, current_alpha, 1);

            soma_fitness += s.fitness;

            // Guardar a melhor de todas as runs
            if (s.fitness > best_of_all.fitness)
            {
                copy_solution(&best_of_all, &s);
            }

            // Opcional: ver progresso (comentado para não poluir o terminal)
            // printf(".");
        }

        // Apresentar resultados para este Alpha
        double media = soma_fitness / num_runs;
        printf("\nRESULTADOS (media de %d runs):\n", num_runs);
        printf("Media Fitness: %.2f\n", media);
        printf("Melhor Fitness Encontrada: %.2f\n", best_of_all.fitness);
        printf("--------------------------------------------------\n");
    }

    return 0;
}