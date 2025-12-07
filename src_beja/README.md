# IIA_TP2
# Problema de Diversidade Máxima

Implementação de algoritmos de otimização para o problema de seleção de pontos com máxima diversidade, desenvolvido em C.

## 📋 Descrição do Problema

Dado um conjunto de C candidaturas e uma matriz de distâncias entre elas, o objetivo é selecionar m pontos que maximizem a distância média entre os pontos selecionados.

**Função Objetivo:** Maximizar a distância média entre os m pontos selecionados.

## 🚀 Algoritmos Implementados

### 1. **Hill Climbing (Trepa-Colinas)**
- **Vizinhança 1:** Troca um ponto selecionado por um não selecionado
- **Vizinhança 2:** Troca dois pontos selecionados por dois não selecionados
- Estratégia: Aceita soluções iguais ou melhores

### 2. **Algoritmo Evolutivo**
- **Seleção:** Torneio (tamanho 3) ou Roleta
- **Recombinação:** Uniforme ou Um ponto
- **Mutação:** Troca (swap) de pontos
- **Elitismo:** Preserva a melhor solução
- Parâmetros configuráveis: tamanho da população, gerações, probabilidades

### 3. **Algoritmos Híbridos**
- **Híbrido 1:** Evolutivo + Hill Climbing (refinamento local nas melhores soluções)
- **Híbrido 2:** Hill Climbing + Evolutivo (múltiplos restarts com diversificação)

## 📁 Estrutura do Projeto

```
projeto/
├── diversity.c          # Código fonte principal
├── Makefile            # Script de compilação e testes
├── README.md           # Este ficheiro
├── tests/              # Ficheiros de entrada (gerados automaticamente)
│   └── test_small.txt
└── results/            # Resultados dos testes (gerados automaticamente)
    └── *.txt
```

## 🔧 Compilação

### Requisitos
- GCC (GNU Compiler Collection)
- Make
- Sistema Linux/Unix ou WSL (Windows)

### Compilar o programa
```bash
make
```

Isto gera o executável `diversity`.

## 📊 Formato do Ficheiro de Entrada

```
C m
d11 d12 d13 ... d1C
d21 d22 d23 ... d2C
...
dC1 dC2 dC3 ... dCC
```

**Onde:**
- `C` = número total de candidaturas
- `m` = número de pontos a selecionar
- `dij` = distância entre o ponto i e o ponto j

**Exemplo (10 pontos, selecionar 5):**
```
10 5
0.0 2.5 3.1 1.8 4.2 ...
2.5 0.0 1.9 3.4 2.1 ...
...
```

## 🧪 Como Usar

### 1. Setup Inicial
```bash
# Cria diretórios e gera ficheiro de teste
make setup
make generate_test
```

### 2. Executar um Teste
```bash
# Teste específico
make test FILE=tests/test_small.txt

# Ou executar diretamente
./diversity tests/test_small.txt
```

### 3. Executar Todos os Testes
```bash
make test_all
```

### 4. Benchmark (Múltiplas Execuções)
```bash
# Executa 10 vezes e guarda resultados
make benchmark FILE=tests/test_small.txt RUNS=10
```

### 5. Verificar Memory Leaks
```bash
make valgrind FILE=tests/test_small.txt
```

## 📈 Exemplo de Saída

```
Problema carregado: C=10, m=5

=== HILL CLIMBING ===
Pontos selecionados: 1 3 5 7 9 
Fitness: 8.45

=== EVOLUTIVO ===
Pontos selecionados: 0 2 4 6 8 
Fitness: 9.12

=== HÍBRIDO 1 ===
Pontos selecionados: 1 2 5 7 9 
Fitness: 9.34

=== HÍBRIDO 2 ===
Pontos selecionados: 0 3 5 6 9 
Fitness: 9.28
```

## ⚙️ Parâmetros Configuráveis

Podes modificar os parâmetros diretamente no código (função `main`):

### Hill Climbing
```c
hill_climbing(1000, 1);  // (iterações, tipo_vizinhança)
```

### Algoritmo Evolutivo
```c
evolutionary_algorithm(
    50,    // tamanho da população
    100,   // número de gerações
    0.8,   // probabilidade de crossover
    0.1,   // probabilidade de mutação
    1,     // tipo de seleção (1=torneio, 2=roleta)
    1      // tipo de crossover (1=uniforme, 2=um ponto)
);
```

### Híbridos
```c
hybrid1(50, 100);    // (pop_size, gerações)
hybrid2(1000);       // (iterações)
```

## 🛠️ Comandos do Makefile

| Comando | Descrição |
|---------|-----------|
| `make` | Compila o programa |
| `make setup` | Cria diretórios necessários |
| `make generate_test` | Gera ficheiro de teste exemplo |
| `make test FILE=<ficheiro>` | Executa teste específico |
| `make test_all` | Executa todos os testes |
| `make valgrind FILE=<ficheiro>` | Verifica memory leaks |
| `make benchmark FILE=<ficheiro> RUNS=N` | Executa benchmark |
| `make clean` | Remove ficheiros objeto |
| `make distclean` | Remove executável e resultados |
| `make mrproper` | Remove tudo |
| `make help` | Mostra ajuda |

## 📝 Criar os Teus Próprios Testes

Cria um ficheiro no formato correto em `tests/`:

```bash
nano tests/meu_teste.txt
```

Depois executa:
```bash
make test FILE=tests/meu_teste.txt
```

## 🐛 Debugging

### Compilar com símbolos de debug
```bash
gcc -g -Wall diversity.c -o diversity -lm
gdb ./diversity
```

### Verificar memory leaks
```bash
make valgrind FILE=tests/test_small.txt
```

## 📊 Análise de Resultados

Os resultados de `make test_all` são guardados em `results/`. Podes analisar:

```bash
# Ver todos os resultados
ls results/

# Comparar fitness de diferentes algoritmos
grep "Fitness:" results/*.txt
```

## ⚡ Otimizações

O código está compilado com `-O3` para máxima performance. Para debug, usa:

```bash
gcc -g -O0 -Wall diversity.c -o diversity -lm
```

## 📚 Limitações

- Máximo de 500 candidaturas (`MAX_CANDIDATES`)
- Máximo de 10 execuções independentes (`MAX_RUNS`)

Para aumentar estes limites, modifica as constantes no código:
```c
#define MAX_CANDIDATES 1000  // Aumentar para mais candidaturas
```

## 🤝 Contribuir

Para melhorar o código:
1. Adiciona novos operadores de mutação
2. Implementa novos métodos de seleção
3. Testa com problemas maiores
4. Otimiza o cálculo de fitness

## 📄 Licença

Este projeto é para fins educacionais.

## 👤 Autor

Desenvolvido como parte de um trabalho académico sobre algoritmos de otimização.

---

**Quick Start:**
```bash
make && make generate_test && make test FILE=tests/test_small.txt
```
