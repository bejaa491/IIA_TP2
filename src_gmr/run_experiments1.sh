#!/usr/bin/env bash
set -e

EXEC=./projeto          # nome do executável (do teu Makefile)
RESULTS_DIR=results
OUTPUT_CSV="$RESULTS_DIR/resultados.csv"

mkdir -p "$RESULTS_DIR"

########################################
# 1) LISTA DAS INSTÂNCIAS / GRAFOS
########################################
# <-- ADAPTA AQUI -->
GRAPHS=(
  "tourism_5.txt"
  "tourism_20.txt"
  "tourism_50.txt"
  "tourism_100.txt"
  "tourism_250.txt"
  "tourism_500.txt"
)

# Nome da instância (para a coluna Instancia): aqui uso o nome sem extensão
get_instance_name() {
  local f="$1"
  f=$(basename "$f")
  echo "${f%.*}"
}

########################################
# 2) FUNÇÃO QUE EXECUTA UMA CONFIG
########################################
# Argumentos:
# 1: ficheiro instância
# 2: nomeInstancia (I1, tourism_20, etc.)
# 3: nomeConfig (HC-1, SA-1, GA-1, ...)
# 4: nomeAlgoritmo (Trepa-Colinas, SA, GA, H1, H2)
# 5: opcao (1..5 do menu)
# 6: viz (1 ou 2, ou vazio se não usar)
# 7: hc_iter (ou vazio)
# 8: tmax (ou vazio)
# 9: tmin (ou vazio)
# 10: alpha (ou vazio)
# 11: sel (1=Torneio, 2=Roleta, ou vazio)
# 12: cross (1,2,3 ou vazio)
run_config() {
  local file="$1"
  local inst="$2"
  local config="$3"
  local alg_nome="$4"
  local opcao="$5"
  local viz="$6"
  local hc_iter="$7"
  local tmax="$8"
  local tmin="$9"
  local alpha="${10}"
  local sel="${11}"
  local cross="${12}"

  # Defaults do teu código (fixos)
  local pop_size=50
  local generations=1000
  local prob_cross=0.7
  local prob_mut=0.1

  # Construir a sequência de inputs para o menu
  local input=""
  input+="${opcao}\n"

  # SA (opcoes 2 e 4)
  if [ "$opcao" -eq 2 ] || [ "$opcao" -eq 4 ]; then
    input+="${tmax}\n"
    input+="${tmin}\n"
    input+="${alpha}\n"
  fi

  # HC (opcoes 1 e 5)
  if [ "$opcao" -eq 1 ] || [ "$opcao" -eq 5 ]; then
    input+="${hc_iter}\n"
  fi

  # Vizinhanca (1,2,4,5)
  if [ "$opcao" -eq 1 ] || [ "$opcao" -eq 2 ] || [ "$opcao" -eq 4 ] || [ "$opcao" -eq 5 ]; then
    input+="${viz}\n"
  fi

  # GA / Hibridos (3,4,5)
  if [ "$opcao" -eq 3 ] || [ "$opcao" -eq 4 ] || [ "$opcao" -eq 5 ]; then
    input+="${cross}\n"
    input+="${sel}\n"
  fi

  # ENTER para sair da função esperar_enter()
  input+="\n"
  # 0 para sair do menu (Opcao: 0)
  input+="0\n"

  echo ">> [$(basename "$file")] Config=$config Alg=$alg_nome"

  # Executar programa e extrair só as linhas "Run<TAB>Fitness"
  printf "$input" | "$EXEC" "$file" | \
    awk -v inst="$inst" \
        -v ficheiro="$file" \
        -v config="$config" \
        -v alg="$alg_nome" \
        -v viz="$viz" \
        -v hc_iter="$hc_iter" \
        -v tmax="$tmax" \
        -v tmin="$tmin" \
        -v alpha="$alpha" \
        -v pop="$pop_size" \
        -v gen="$generations" \
        -v pc="$prob_cross" \
        -v pm="$prob_mut" \
        -v sel="$sel" \
        -v cross="$cross" \
        'BEGIN { FS="\t" }
         /^[0-9]+\t/ {
           run=$1; fit=$2;
           gsub(",", ".", fit);  # por segurança
           print inst "," ficheiro "," config "," alg "," viz "," hc_iter "," \
                 tmax "," tmin "," alpha "," pop "," gen "," pc "," pm "," \
                 sel "," cross "," run "," fit;
         }' >> "$OUTPUT_CSV"
}

########################################
# 3) CABEÇALHO DO CSV
########################################
echo "Instancia,Ficheiro,Config,Algoritmo,Viz,Iter_HC,Tmax,Tmin,Alpha,Pop,Geracoes,Prob_Cross,Prob_Mut,Selecao,Crossover,Run,Fitness" > "$OUTPUT_CSV"

########################################
# 4) LISTA DE CONFIGURAÇÕES
########################################
# EXEMPLOS com valores que combinam bem com o teu código:
# - HC-1: Trepa-Colinas, viz=1, 1000 iter
# - HC-2: Trepa-Colinas, viz=2, 1000 iter
# - SA-1: SA, viz=1, Tmax=100, Tmin=0.001, alpha=0.99
# - SA-2: SA, viz=2, Tmax=100, Tmin=0.001, alpha=0.95
# - GA-1: GA puro, cross=1, sel=1
# - GA-2: GA puro, cross=3, sel=2
# - H1-1: Híbrido 1 (GA+SA), viz=1, SA(100,0.001,0.99), GA(1,1)
# - H1-2: Híbrido 1 (GA+SA), viz=2, SA(100,0.001,0.95), GA(3,2)
# - H2-1: Híbrido 2 (GA+HC), viz=1, HC 1000, GA(1,1)
# - H2-2: Híbrido 2 (GA+HC), viz=2, HC 1000, GA(3,2)

for file in "${GRAPHS[@]}"; do
  inst="$(get_instance_name "$file")"

  # HC-1 e HC-2
  run_config "$file" "$inst" "HC-1" "Trepa-Colinas" 1 1 1000 "" "" "" "" ""
  run_config "$file" "$inst" "HC-2" "Trepa-Colinas" 1 2 1000 "" "" "" "" ""

  # SA-1 e SA-2
  run_config "$file" "$inst" "SA-1" "Recristalizacao" 2 1 "" 100.0 0.001 0.99 "" ""
  run_config "$file" "$inst" "SA-2" "Recristalizacao" 2 2 "" 100.0 0.001 0.95 "" ""

  # GA-1 e GA-2
  run_config "$file" "$inst" "GA-1" "Evolutivo" 3 "" "" "" "" "" 1 1
  run_config "$file" "$inst" "GA-2" "Evolutivo" 3 "" "" "" "" "" 2 3

  # H1-1 e H1-2 (GA + SA)
  run_config "$file" "$inst" "H1-1" "Hibrido1_GA_SA" 4 1 "" 100.0 0.001 0.99 1 1
  run_config "$file" "$inst" "H1-2" "Hibrido1_GA_SA" 4 2 "" 100.0 0.001 0.95 2 3

  # H2-1 e H2-2 (GA + HC)
  run_config "$file" "$inst" "H2-1" "Hibrido2_GA_HC" 5 1 1000 "" "" "" 1 1
  run_config "$file" "$inst" "H2-2" "Hibrido2_GA_HC" 5 2 1000 "" "" "" 2 3
done

echo "Ficheiro CSV gerado em: $OUTPUT_CSV"
