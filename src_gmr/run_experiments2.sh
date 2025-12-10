#!/usr/bin/env bash
set -e

# ==============================
# CONFIGURACAO GERAL
# ==============================
EXECUTAVEL="./projeto"
RESULTS_DIR="results"

mkdir -p "$RESULTS_DIR"

# Lista de grafos (AJUSTA ESTES NOMES SE PRECISO)
GRAFOS=(
  "tourism_5.txt"
  "tourism_20.txt"
  "tourism_50.txt"
  "tourism_100.txt"
  "tourism_250.txt"
  "tourism_500.txt"
)

# ==============================
# FUNCAO: EXECUTAR UMA CONFIGURACAO
# ==============================
# Args:
# 1 - ficheiro grafo
# 2 - ficheiro TXT de saida deste grafo
# 3 - ficheiro CSV de saida deste grafo
# 4 - nome do Algoritmo (ex: "HillClimbing")
# 5 - descricao/config (ex: "Vizinhanca 1, Iteracoes 1000")
# 6 - sequencia de input (sem o ENTER final e sem o 0 final)
run_config() {
    local grafo="$1"
    local txt="$2"
    local csv="$3"
    local algoritmo="$4"
    local config="$5"
    local input_seq="$6"

    echo ">> $grafo | $algoritmo | $config"

    # Enviamos:
    #   - a sequencia de respostas (com \n dentro),
    #   - um ENTER extra para a funcao esperar_enter(),
    #   - e 0 para sair do menu.
    local raw
    raw=$(printf "%b\n\n0\n" "$input_seq" | "$EXECUTAVEL" "$grafo")

    # Extraimos apenas as linhas "Run <tab> Fitness"
    # Ex:  "1\t72.2799\t-"
    local runs
    runs=$(echo "$raw" | awk '/^[[:space:]]*[0-9]+\t/ {print $2}')

    if [ -z "$runs" ]; then
        echo "AVISO: nao encontrei linhas Run/Fitness para $algoritmo [$config] no grafo $grafo" >&2
        return
    fi

    # Escrever secao no TXT, estilo legível
    echo "" >> "$txt"
    echo "$config" >> "$txt"

    local i=1
    while read -r val; do
        printf "  Run %d: %s\n" "$i" "$val" >> "$txt"
        i=$((i+1))
    done <<< "$runs"

    # Calcular Melhor, Pior, Media, Desvio a partir dos runs
    local stats
    stats=$(echo "$runs" | awk '
        {
            v = $1 + 0.0;
            n++;
            sum += v;
            sum2 += v*v;
            if (n == 1 || v < min) min = v;
            if (n == 1 || v > max) max = v;
        }
        END {
            if (n > 0) {
                mean = sum / n;
                var  = sum2 / n - mean*mean;
                if (var < 0) var = 0;
                sd   = sqrt(var);
                # Melhor (max), Pior (min), Media, Desvio
                printf "%.4f %.4f %.4f %.4f", max, min, mean, sd;
            }
        }')

    local melhor pior media desvio
    read -r melhor pior media desvio <<< "$stats"

    printf "  Melhor: %.4f, Pior: %.4f, Media: %.4f, Desvio: %.4f\n" \
        "$melhor" "$pior" "$media" "$desvio" >> "$txt"

    # Linha para o CSV deste grafo:
    # separador ; e configuracao entre aspas
    # Colunas: Grafo;Algoritmo;Configuracao;Melhor;Pior;Media;Desvio
    printf "%s;%s;\"%s\";%.4f;%.4f;%.4f;%.4f\n" \
        "$(basename "$grafo")" "$algoritmo" "$config" \
        "$melhor" "$pior" "$media" "$desvio" >> "$csv"
}

# ==============================
# LOOP PRINCIPAL: UM TXT+CSV POR GRAFO
# ==============================
for grafo in "${GRAFOS[@]}"; do
    if [ ! -f "$grafo" ]; then
        echo "AVISO: grafo '$grafo' nao encontrado. A saltar..."
        continue
    fi

    base=$(basename "$grafo")
    TXT_OUT="$RESULTS_DIR/resultados_${base}_10runs.txt"
    CSV_OUT="$RESULTS_DIR/resultados_${base}_10runs.csv"

    # Ler C e m da primeira linha do ficheiro (formato: C m)
    read -r C m < "$grafo"

    # Cabecalho do TXT
    {
        echo "RESULTADOS - PROBLEMA DE DIVERSIDADE MAXIMA"
        echo "Ficheiro: $base"
        echo "C=$C, m=$m"
        echo "Execucoes: 10"
        echo "====================================="
        echo ""
    } > "$TXT_OUT"

    # Cabecalho do CSV deste grafo (usa ;)
    echo "Grafo;Algoritmo;Configuracao;Melhor;Pior;Media;Desvio" > "$CSV_OUT"

    # ==========================
    # HILL CLIMBING
    # ==========================
    echo "=== HILL CLIMBING ===" >> "$TXT_OUT"

    # Opcao 1 -> Iteracoes -> Vizinhanca
    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "HillClimbing" \
        "Vizinhanca 1, Iteracoes 500" \
        "1\n500\n1"

    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "HillClimbing" \
        "Vizinhanca 1, Iteracoes 1000" \
        "1\n1000\n1"

    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "HillClimbing" \
        "Vizinhanca 2, Iteracoes 500" \
        "1\n500\n2"

    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "HillClimbing" \
        "Vizinhanca 2, Iteracoes 1000" \
        "1\n1000\n2"

    # ==========================
    # SIMULATED ANNEALING
    # ==========================
    echo "" >> "$TXT_OUT"
    echo "=== SIMULATED ANNEALING ===" >> "$TXT_OUT"

    # Opcao 2 -> Tmax -> Tmin -> Alpha -> Vizinhanca
    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "SimAnnealing" \
        "Tmax 100, Tmin 0.001, Alpha 0.99, Viz 1" \
        "2\n100\n0.001\n0.99\n1"

    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "SimAnnealing" \
        "Tmax 100, Tmin 0.001, Alpha 0.95, Viz 1" \
        "2\n100\n0.001\n0.95\n1"

    # ==========================
    # EVOLUTIVO
    # ==========================
    echo "" >> "$TXT_OUT"
    echo "=== EVOLUTIVO (GA) ===" >> "$TXT_OUT"

    # Opcao 3 -> Crossover -> Selecao
    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "Evolutivo" \
        "Cross Uniforme (1), Selecao Torneio (1)" \
        "3\n1\n1"

    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "Evolutivo" \
        "Cross DoisPontos (3), Selecao Roleta (2)" \
        "3\n3\n2"

    # ==========================
    # HIBRIDOS
    # ==========================
    echo "" >> "$TXT_OUT"
    echo "=== HIBRIDOS ===" >> "$TXT_OUT"

    # Hibrido 1: Opcao 4 -> Tmax -> Tmin -> Alpha -> Viz -> Cross -> Sel
    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "Hibrido1_GA_SA" \
        "H1: Tmax 100, Tmin 0.001, Alpha 0.99, Viz 1, Cross 1, Sel 1" \
        "4\n100\n0.001\n0.99\n1\n1\n1"

    # Hibrido 2: Opcao 5 -> Iter -> Viz -> Cross -> Sel
    run_config "$grafo" "$TXT_OUT" "$CSV_OUT" \
        "Hibrido2_GA_HC" \
        "H2: Iter 1000, Viz 1, Cross 1, Sel 1" \
        "5\n1000\n1\n1\n1"

    echo ""
    echo "Feito grafo: $grafo"
    echo "  -> TXT: $TXT_OUT"
    echo "  -> CSV: $CSV_OUT"
    echo "----------------------------------------"
done

echo "=== TODOS OS TESTES CONCLUIDOS ==="
echo "Resultados guardados na pasta '$RESULTS_DIR'."

