#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include "funcao.h"
#include "algoritmo.h"

// Função auxiliar: Lê um inteiro de forma segura (evita loops infinitos)
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
                return valor;
        }
        printf(" >> Entrada invalida! Tente novamente.\n");
    }
}
// Função auxiliar: Lê um double de forma segura (evita loops infinitos)
double ler_double(const char *mensagem)
{
    char buffer[100];
    double valor;
    while (1)
    {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            // Troca virgula por ponto para compatibilidade
            for (int i = 0; buffer[i]; i++)
                if (buffer[i] == ',')
                    buffer[i] = '.';
            if (sscanf(buffer, "%lf", &valor) == 1)
                return valor;
        }
        printf(" >> Entrada invalida! Tente novamente.\n");
    }
}
// Função auxiliar: Pausa o programa até ENTER ser pressionado
void esperar_enter()
{
    printf("\nPressione ENTER para continuar...");
    char buffer[1024];
    fgets(buffer, sizeof(buffer), stdin);
}
// Função auxiliar: Limpa o ecrã
void limpar_ecra()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main(int argc, char *argv[])
{
    char nome_fich[100];

    // Carregamento de dados seguro
    if (argc == 2)
    {
        sprintf(nome_fich, "%s", argv[1]);
    }
    else
    {
        printf("Ficheiro: ");
        if (fgets(nome_fich, sizeof(nome_fich), stdin) != NULL)
        {
            nome_fich[strcspn(nome_fich, "\n")] = 0;
        }
        else
            return 1;
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
    double sa_tmax = 100.0; // Temp Max
    double sa_tmin = 0.001; // Temp Min
    double sa_alpha = 0.99; // Arrefecimento

    do
    {
        limpar_ecra();

        printf("\n=== MENU AVANCADO (Ficheiro: %s) ===\n", nome_fich);
        printf("1. Trepa-Colinas (Hill Climbing)\n");
        printf("2. Recristalizacao (Simulated Annealing)\n");
        printf("3. Evolutivo (Algoritmo Genetico)\n");
        printf("4. Hibrido 1 (GA + SA)\n");
        printf("5. Hibrido 2 (GA + HC)\n");
        printf("6. Hibrido 3 (Memetico)\n");
        printf("0. Sair\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 0)
            break;

        // --- SUB-MENU DE ESCOLHAS ---
        int viz = 1;           // 1=Swap, 2=Swap2
        int cross = 1;         // 1=Unif, 2=OnePoint, 3=TwoPoint
        int sel = 1;           // 1=Torneio, 2=Roleta
        int voltar = 0;        // Flag para voltar ao menu principal        
        double prob_ls = 0.05; // Probabilidade de refinamento (Memetico)

        // --- CONFIGURAÇÃO ESPECÍFICA ---

        // 1. Parametros SA (Temperaturas) - Apenas para Opção 2 e 4
        if (opcao == 2 || opcao == 4)
        {
            printf("\n--- Configuracao SA ---\n");
            sa_tmax = ler_double("   > Temperatura Maxima (Tmax)? ");
            // Proteção simples para não voltar se o utilizador meter 0 ou negativo sem querer
            if (sa_tmax <= 0)
            {
                printf("Erro: Tmax deve ser > 0\n");
                voltar = 1;
            }

            if (!voltar)
                sa_tmin = ler_double("   > Temperatura Minima (Tmin)? ");
            if (!voltar)
                sa_alpha = ler_double("   > Alpha (Ex: 0.99, 0.95)? ");
        }
        // 2. Parametros HC (Iterações) - Apenas para Opção 1 e 5
        if (!voltar && (opcao == 1 || opcao == 5))
        {
            printf("\n--- Configuracao HC ---\n");
            hc_iter = ler_inteiro("   > Numero de Iteracoes? ");
            if (hc_iter <= 0)
                voltar = 1;
        }

        // 3. Pergunta da Vizinhança (Para menus 1, 2, 4, 5, 6)
        if (!voltar && (opcao == 1 || opcao == 2 || opcao == 4 || opcao == 5 || opcao == 6))
        {
            do
            {
                viz = ler_inteiro("   > Vizinhanca? (1-Swap, 2-Swap2, 0-Voltar): ");
            } while (viz < 0 || viz > 2);
            if (viz == 0)
                voltar = 1;
        }

        // 4. Perguntas do Evolutivo (Para menus 3, 4, 5, 6)

        if (!voltar && (opcao >= 3 && opcao <= 6))
        {
            printf("\n--- Configuracao Evolutivo ---\n");
            
            do
            {
                cross = ler_inteiro("   > Crossover? (1-Uniforme, 2-Um Ponto, 3-Dois Pontos, 0-Voltar): ");
            } while (cross < 0 || cross > 3);
            if (cross == 0)
                voltar = 1;

            if (!voltar)
            {
                do
                {
                    sel = ler_inteiro("   > Selecao? (1-Torneio, 2-Roleta, 0-Voltar): ");
                } while (sel < 0 || sel > 2);
                if (sel == 0)
                    voltar = 1;
            }
            if (!voltar && opcao == 6)
            {
                prob_ls = ler_double("   > Probabilidade de Refinamento (Ex: 0.05)? ");
            }
        }

        // --- VERIFICAÇÃO FINAL ---
        // Se o utilizador escolheu 0 em qualquer submenu, reinicia o loop principal
        if (voltar)
            continue;

        // --- EXECUÇÃO ---
        Solution sol, global_best;
        global_best.fitness = -1.0;
        double soma_fit = 0.0;
        double valores[runs];

        printf("\n--- A executar %d runs ---\n", runs);

        printf("Run\tFitness\tDiversidade\n");
        for (int r = 0; r < runs; r++)
        {
            int diversidade_final = -1;
            switch (opcao)
            {
            case 1: // Hill Climbing
                sol = hill_climbing(hc_iter, viz);
                break;
            case 2: // Simulated Annealing
                sol = simulated_annealing(sa_tmax, sa_tmin, sa_alpha, viz);
                break;
            case 3: // Evolutivo
                sol = evolutionary_algorithm(pop_size, gen, 0.7, 0.1, sel, cross, viz, prob_ls, &diversidade_final);
                break;
            case 4: // Hibrido 1 (GA + SA)
                sol = hybrid_algorithm_1(pop_size, gen, sa_tmax, sa_tmin, sel, cross, viz);
                break;
            case 5: // Hibrido 2 (GA + HC)
                sol = hybrid_algorithm_2(pop_size, gen, hc_iter, sel, cross, viz);
                break;
            case 6: // Hibrido 3 (Memetico) 
                sol = evolutionary_algorithm(pop_size, gen, 0.7, 0.1, sel, cross, viz, prob_ls, &diversidade_final);
                break;
            }
            // Guardar estatísticas
            soma_fit += sol.fitness;
            if (sol.fitness > global_best.fitness)
                copy_solution(&global_best, &sol);

            // Imprimir resultados
            if (diversidade_final != -1)
            {
                // Se for Evolutivo, mostra a diversidade
                printf("%d\t%.2f\t%d\n", r + 1, sol.fitness, diversidade_final);
            }
            else
            {
                // Se for HC ou SA, mostra traço "-"
                printf("%d\t%.2f\t-\n", r + 1, sol.fitness);
            }
            valores[r] = sol.fitness;
        }
        double media = soma_fit / runs;
        double soma2 = 0.0;
        for (int r = 0; r < runs; r++)
        {
            double d = valores[r] - media;
            soma2 += d * d;
        }
        double desvio = sqrt(soma2 / runs); // populacional; para amostral / (runs-1)

        printf("\n--- ESTATISTICA FINAL ---\n");
        printf("Media: %.2f\n", media);
        printf("Melhor: %.2f\n", global_best.fitness);
        printf("Desvio Padrao: %.2f\n", desvio);
        printf("Solucao: ");
        for (int i = 0; i < prob.C; i++)
            if (global_best.selected[i])
                printf("%d ", i);
        printf("\n-------------------------\n");

        if (opcao != 0)
            esperar_enter();

    } while (opcao != 0);

    return 0;
}