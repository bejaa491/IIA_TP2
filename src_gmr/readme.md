Aqui tens o ficheiro README.md completo. Podes clicar no botão de "Copy" no canto do bloco de código e colar diretamente num ficheiro novo no teu GitHub (chama-lhe README.md).

Markdown

# 🌍 Problema de Otimização Turística - Algoritmos Heurísticos

Este projeto implementa e compara diversas abordagens heurísticas e meta-heurísticas para resolver um problema de **Dispersão Máxima (Maximum Dispersion Problem)**.

O objetivo é selecionar um subconjunto de **m** pontos turísticos a partir de um conjunto de **C** candidatos, de forma a **maximizar a soma das distâncias** entre todos os pontos selecionados.

## 🚀 Funcionalidades e Algoritmos

O projeto inclui um sistema de menu interativo robusto que permite configurar e executar os seguintes algoritmos:

### 1. Pesquisa Local
* **🏔️ Trepa-Colinas (Hill Climbing):**
    * Estratégia: *Best Improvement* (com aceitação de custo igual para navegar em planaltos).
    * Vizinhanças: *Swap* (Troca 1-1) e *Swap2* (Troca 2-2).
* **🔥 Recristalização Simulada (Simulated Annealing):**
    * Parâmetros configuráveis: Temperatura Inicial ($T_{max}$), Temperatura Final ($T_{min}$) e Fator de Arrefecimento ($\alpha$).
    * Aceita soluções piores com base na distribuição de Boltzmann para escapar a ótimos locais.

### 2. Algoritmos Evolutivos (Algoritmo Genético) 🧬
* **Representação:** Binária (Array de 0s e 1s).
* **Seleção:** * Torneio (Tournament).
    * Roleta (Roulette Wheel).
* **Crossover (Recombinação):**
    * Uniforme (Uniform).
    * Um Ponto de Corte (One-Point).
    * Dois Pontos de Corte (Two-Point).
    * *Nota:* Inclui mecanismo de reparação automática para garantir exatamente $m$ pontos selecionados.
* **Mutação:** Baseada em troca (Swap e Swap2) para manter a viabilidade.

### 3. Algoritmos Híbridos 🤝
Abordagem "Relay" (Passagem de testemunho):
* **Híbrido 1:** Algoritmo Evolutivo (Exploração) + Recristalização Simulada (Refinação).
* **Híbrido 2:** Algoritmo Evolutivo (Exploração) + Trepa-Colinas (Refinação).

---

## 📂 Estrutura do Projeto

* **`main.c`**: Ponto de entrada. Contém o menu interativo seguro, gestão de parâmetros dinâmicos e ciclo de repetições para recolha estatística.
* **`algoritmo.c/h`**: Implementação do núcleo dos algoritmos (HC, SA, EA, Híbridos) e operadores genéticos.
* **`funcao.c/h`**: Definição da estrutura da solução, função objetivo (fitness) e gerador de soluções aleatórias.
* **`utils.c/h`**: Funções auxiliares (leitura de ficheiros, geração de números aleatórios, validação de input).
* **`tourism_*.txt`**: Instâncias de dados para teste (Grafos de distâncias).

---

## 🛠️ Como Compilar

Certifica-te de que tens o compilador `gcc` instalado. No terminal, executa o seguinte comando:

```bash
gcc main.c algoritmo.c funcao.c utils.c -o projeto -lm
Nota: A flag -lm é necessária em sistemas Linux/macOS para ligar a biblioteca matemática (math.h).

▶️ Como Executar
Podes executar o programa passando o ficheiro de dados como argumento:

Linux / macOS
Bash

./projeto tourism_5.txt
Windows
DOS

projeto.exe tourism_5.txt
📊 Utilização do Menu
Ao iniciar, o programa apresenta um menu onde podes escolher o algoritmo e configurar os parâmetros experimentais em tempo real sem recompilar:

Escolha do Algoritmo: Seleciona entre HC, SA, EA ou Híbridos.

Configuração Dinâmica:

População, Gerações, Probabilidades (Crossover/Mutação).

Temperaturas (Tmax, Tmin, Alpha).

Tipo de Vizinhança, Seleção e Crossover.

Execução: O programa executa 10 repetições (runs) independentes.

Resultados: Apresenta o fitness de cada run (formatado para fácil exportação para Excel) e a média/melhor solução final.

📝 Formato dos Ficheiros de Dados
Os ficheiros de entrada devem seguir o seguinte formato:

Plaintext

C m          <-- C: Nº total de candidatos, m: Nº de pontos a selecionar
e1 e2 dist   <-- Distância entre o ponto 1 e o ponto 2
e1 e3 dist
...
👨‍💻 Autores
Desenvolvido no âmbito da disciplina de Inteligência Artificial / Otimização.

Projeto Académico