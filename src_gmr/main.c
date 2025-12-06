#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

int ler_inteiro(const char *mensagem)
{
    char buffer[100];
    int valor;
    while (1)
    {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (sscanf(buffer, "%d", &valor) == 1)
            {
                return valor;
            }
        }
        printf(" >> Entrada invalida! Tente novamente.\n");
    }
}

int main(int argc, char *argv[])
{
    char nome_fich[100];

    // Carregamento do ficheiro
    if (argc == 2)
        sprintf(nome_fich, "%s", argv[1]);
    else
    {
        printf("Ficheiro: ");
        if (fgets(nome_fich, sizeof(nome_fich), stdin) != NULL)
        {
            nome_fich[strcspn(nome_fich, "\n")] = 0;
        }
        else
        {
            fprintf(stderr, "Erro de leitura do nome do ficheiro.\n");
            return 1;
        }
    }

    if (!init_data(nome_fich))
    {
        printf("Erro leitura.\n");
        return 0;
    }

    init_rand();

    int opcao;
    int runs = 10; // Repetições para o relatório

    // Parâmetros Gerais (defaults)
    int pop_size = 50;      // População
    int gen = 1000;         // Gerações
    int hc_iter = 1000;     // Iterações Hill Climbing
    double sa_tmax = 100.0; // Temp inicial SA

    do
    {
        printf("\n=== MENU AVANCADO (Ficheiro: %s) ===\n", nome_fich);
        printf("1. Trepa-Colinas (Hill Climbing)\n");
        printf("2. Recristalizacao (Simulated Annealing)\n");
        printf("3. Evolutivo (Algoritmo Genetico)\n");
        printf("4. Hibrido 1 (GA + SA)\n");
        printf("5. Hibrido 2 (GA + HC)\n");
        printf("0. Sair\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 0)
            break;

        // --- SUB-MENU DE ESCOLHAS ---
        int viz = 1;   // 1=Swap, 2=Swap2
        int cross = 1; // 1=Unif, 2=OnePoint, 3=TwoPoint
        int sel = 1;   // 1=Torneio, 2=Roleta
        int voltar = 0; // Flag para voltar ao menu principal

        // 1. Pergunta da Vizinhança (Para menus 1, 2, 4, 5)
        if (opcao == 1 || opcao == 2 || opcao == 4 || opcao == 5) {
            do {
                viz = ler_inteiro("   > Vizinhanca? (1-Swap Simples, 2-Swap Duplo, 0-Voltar): ");
            } while (viz < 0 || viz > 2); // Aceita 0, 1, 2
            
            if (viz == 0) voltar = 1;
        }

        // 2. Perguntas do Evolutivo (Para menus 3, 4, 5) - Só entra se não tiver voltado antes
        if (!voltar && (opcao == 3 || opcao == 4 || opcao == 5)) {
            
            // Crossover
            do {
                cross = ler_inteiro("   > Crossover? (1-Uniforme, 2-Um Ponto, 3-Dois Pontos, 0-Voltar): ");
            } while (cross < 0 || cross > 3);
            
            if (cross == 0) voltar = 1;
            
            // Seleção (só pergunta se ainda não quis voltar)
            if (!voltar) {
                do {
                    sel = ler_inteiro("   > Selecao? (1-Torneio, 2-Roleta, 0-Voltar): ");
                } while (sel < 0 || sel > 2);
                
                if (sel == 0) voltar = 1;
            }
        }

        // --- VERIFICAÇÃO FINAL ---
        // Se o utilizador escolheu 0 em qualquer submenu, reinicia o loop principal
        if (voltar) {
            printf(" >> Operacao cancelada. A voltar ao menu...\n");
            continue; 
        }

        // --- EXECUÇÃO ---
        Solution sol, global_best;
        global_best.fitness = -1.0;
        double soma_fit = 0.0;

        printf("\n--- A executar %d runs ---\n", runs);
        printf("Run\tFitness\n"); // Cabeçalho para Excel

        for (int r = 0; r < runs; r++)
        {
            switch (opcao)
            {
            case 1: // Hill Climbing
                sol = hill_climbing(hc_iter, viz);
                break;
            case 2: // Simulated Annealing
                sol = simulated_annealing(sa_tmax, 0.001, 0.99, viz);
                break;
            case 3: // Evolutivo
                sol = evolutionary_algorithm(pop_size, gen, 0.7, 0.1, sel, cross);
                break;
            case 4: // Hibrido 1 (GA + SA)
                // Agora passamos as escolhas (sel, cross, viz) para o híbrido
                sol = hybrid_algorithm_1(pop_size, gen, sa_tmax, 0.001, sel, cross, viz);
                break;
            case 5: // Hibrido 2 (GA + HC)
                sol = hybrid_algorithm_2(pop_size, gen, hc_iter, sel, cross, viz);
                break;
            }
            // Guardar estatísticas
            soma_fit += sol.fitness;
            if (sol.fitness > global_best.fitness)
                copy_solution(&global_best, &sol);

            // Imprimir linha para Excel (Run e Fitness)
            // \t cria uma tabulação que o Excel separa automaticamente em colunas
            printf("%d\t%.2f\n", r + 1, sol.fitness);
        }

        printf("\n--- ESTATISTICA FINAL ---\n");
        printf("Media: %.2f\n", soma_fit / runs);
        printf("Melhor: %.2f\n", global_best.fitness);
        printf("Solucao: ");
        for (int i = 0; i < prob.C; i++)
            if (global_best.selected[i])
                printf("%d ", i);
        printf("\n-------------------------\n");

    } while (opcao != 0);

    return 0;
}