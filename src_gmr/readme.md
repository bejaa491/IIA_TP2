# 🌍 Problema de Otimização Turística - Algoritmos Heurísticos

Este projeto implementa e compara diversas abordagens heurísticas e meta-heurísticas para resolver um problema de **Dispersão Máxima (Maximum Dispersion Problem)**.

O objetivo é selecionar um subconjunto de **m** pontos turísticos a partir de um conjunto de **C** candidatos, de forma a **maximizar a soma das distâncias** entre todos os pontos selecionados.

---

## 🚀 Funcionalidades e Algoritmos

O projeto inclui um sistema de menu interativo robusto que permite configurar e executar os seguintes algoritmos:

### 1. Pesquisa Local

- **🏔️ Trepa-Colinas (Hill Climbing)**
  - Estratégia: *Best Improvement* (com aceitação de custo igual para navegar em planaltos).
  - Vizinhanças:
    - *Swap* (Troca 1-1)
    - *Swap2* (Troca 2-2)

- **🔥 Recristalização Simulada (Simulated Annealing)**
  - Parâmetros configuráveis:
    - Temperatura Inicial (`Tmax`)
    - Temperatura Final (`Tmin`)
    - Fator de Arrefecimento (`α`)
  - Aceita soluções piores com base na distribuição de Boltzmann para escapar a ótimos locais.

### 2. Algoritmos Evolutivos (Algoritmo Genético) 🧬

- **Representação:** Binária (array de 0s e 1s).
- **Seleção:**
  - Torneio (*Tournament*)
  - Roleta (*Roulette Wheel*)
- **Crossover (Recombinação):**
  - Uniforme (*Uniform*)
  - Um Ponto de Corte (*One-Point*)
  - Dois Pontos de Corte (*Two-Point*)
  - *Nota:* Inclui mecanismo de reparação automática para garantir exatamente `m` pontos selecionados.
- **Mutação:**
  - Baseada em troca (*Swap* e *Swap2*) para manter a viabilidade.

### 3. Algoritmos Híbridos 🤝

Abordagem estilo “relay” (passagem de testemunho):

- **Híbrido 1:** Algoritmo Evolutivo (Exploração) → Recristalização Simulada (Refinação).
- **Híbrido 2:** Algoritmo Evolutivo (Exploração) → Trepa-Colinas (Refinação).

---

## 📂 Estrutura do Projeto

- **`main.c`**  
  Ponto de entrada. Contém o menu interativo, gestão de parâmetros dinâmicos e ciclo de repetições para recolha estatística.

- **`algoritmo.c` / `algoritmo.h`**  
  Implementação do núcleo dos algoritmos (HC, SA, EA, Híbridos) e operadores genéticos.

- **`funcao.c` / `funcao.h`**  
  Definição da estrutura da solução, função objetivo (fitness) e gerador de soluções aleatórias.

- **`utils.c` / `utils.h`**  
  Funções auxiliares (leitura de ficheiros, geração de números aleatórios, validação de input).

- **`tourism_*.txt`**  
  Instâncias de dados para teste (grafos de distâncias).

---

## 🛠️ Como Compilar

Certifica-te de que tens o compilador `gcc` instalado.  
No terminal, na pasta do projeto, executa:

```bash
gcc main.c algoritmo.c funcao.c utils.c -o projeto -lm
```

> 💡 **Nota:** A flag `-lm` é necessária em sistemas Linux/macOS para ligar a biblioteca matemática (`math.h`).

---

## ▶️ Como Executar

Podes executar o programa passando o ficheiro de dados como argumento.

### Linux / macOS

```bash
./projeto tourism_5.txt
```

### Windows

```bat
projeto.exe tourism_5.txt
```

---

## 📊 Utilização do Menu

Ao iniciar, o programa apresenta um menu onde podes:

- **Escolher o Algoritmo**
  - Trepa-Colinas (HC)
  - Recristalização Simulada (SA)
  - Algoritmo Evolutivo (EA)
  - Algoritmos Híbridos

- **Configurar Parâmetros Dinâmicos**
  - Tamanho da população, número de gerações.
  - Probabilidades de Crossover e Mutação.
  - Temperaturas (`Tmax`, `Tmin`) e fator de arrefecimento (`α`) para SA.
  - Tipo de vizinhança (Swap / Swap2).
  - Tipo de seleção (Torneio / Roleta).
  - Tipo de crossover (Uniforme / One-Point / Two-Point).

- **Execução**
  - O programa executa tipicamente **10 repetições (runs)** independentes por configuração.

- **Resultados**
  - Apresenta o **fitness de cada run** em formato amigável para exportação para Excel.
  - Calcula e mostra **média**, **melhor solução** e outros indicadores de qualidade.

---

## 📝 Formato dos Ficheiros de Dados

Os ficheiros de entrada (`tourism_*.txt`) devem seguir o seguinte formato:

```text
C m          <-- C: nº total de candidatos, m: nº de pontos a selecionar
e1 e2 dist   <-- Distância entre o ponto 1 e o ponto 2
e1 e3 dist
...
```

- `C`: número total de pontos/candidatos.
- `m`: número de pontos a selecionar no subconjunto.
- Cada linha seguinte representa uma aresta do grafo de distâncias:
  - `e1`, `e2`: identificadores dos pontos.
  - `dist`: distância (custo) entre esses dois pontos.

---

## 👨‍💻 Autores

Desenvolvido no âmbito da disciplina de **Inteligência Artificial / Otimização**.  
Projeto académico de experimentação com **algoritmos heurísticos e meta-heurísticos** aplicados a um problema realista de **planeamento turístico**.

---