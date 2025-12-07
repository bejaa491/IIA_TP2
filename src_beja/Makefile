# Makefile para o problema de diversidade

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99
LDFLAGS = -lm

# Nome do executável
TARGET = diversity

# Ficheiros fonte
SRC = diversity.c
OBJ = $(SRC:.c=.o)

# Diretórios
TEST_DIR = tests
RESULTS_DIR = results

# Ficheiros de teste (adiciona aqui os teus ficheiros de teste)
TEST_FILES = $(wildcard $(TEST_DIR)/*.txt)

# Cores para output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
NC = \033[0m # No Color

# ========== REGRAS PRINCIPAIS ==========

all: $(TARGET)

# Compilação
$(TARGET): $(OBJ)
	@echo "$(GREEN)Linking $(TARGET)...$(NC)"
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)Build completo!$(NC)"

%.o: %.c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# ========== TESTES ==========

# Cria diretórios necessários
setup:
	@mkdir -p $(TEST_DIR)
	@mkdir -p $(RESULTS_DIR)
	@echo "$(GREEN)Diretórios criados!$(NC)"

# Gera ficheiro de teste exemplo
generate_test: setup
	@echo "10 5" > $(TEST_DIR)/test_small.txt
	@echo "Gerando matriz de distâncias 10x10..."
	@for i in $$(seq 0 9); do \
		for j in $$(seq 0 9); do \
			if [ $$i -eq $$j ]; then \
				printf "0.0 " >> $(TEST_DIR)/test_small.txt; \
			else \
				printf "%.2f " $$(echo "scale=2; $$RANDOM / 327.67" | bc) >> $(TEST_DIR)/test_small.txt; \
			fi; \
		done; \
		echo "" >> $(TEST_DIR)/test_small.txt; \
	done
	@echo "$(GREEN)Ficheiro de teste criado: $(TEST_DIR)/test_small.txt$(NC)"

# Executa um teste específico
test: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Uso: make test FILE=tests/test_small.txt$(NC)"; \
		exit 1; \
	fi
	@echo "$(YELLOW)Executando teste com $(FILE)...$(NC)"
	@./$(TARGET) $(FILE)

# Executa todos os testes
test_all: $(TARGET)
	@if [ -z "$(TEST_FILES)" ]; then \
		echo "$(RED)Nenhum ficheiro de teste encontrado em $(TEST_DIR)$(NC)"; \
		echo "$(YELLOW)Usa 'make generate_test' para criar um ficheiro exemplo$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Executando todos os testes...$(NC)"
	@for file in $(TEST_FILES); do \
		echo "\n$(YELLOW)========================================$(NC)"; \
		echo "$(YELLOW)Teste: $$file$(NC)"; \
		echo "$(YELLOW)========================================$(NC)"; \
		./$(TARGET) $$file | tee $(RESULTS_DIR)/$$(basename $$file .txt)_result.txt; \
	done
	@echo "\n$(GREEN)Todos os testes completos! Resultados em $(RESULTS_DIR)$(NC)"

# Executa com valgrind para detetar memory leaks
valgrind: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Uso: make valgrind FILE=tests/test_small.txt$(NC)"; \
		exit 1; \
	fi
	@echo "$(YELLOW)Executando com valgrind...$(NC)"
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) $(FILE)

# Benchmark - executa múltiplas vezes e calcula média
benchmark: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Uso: make benchmark FILE=tests/test_small.txt RUNS=10$(NC)"; \
		exit 1; \
	fi
	@RUNS=$${RUNS:-10}; \
	echo "$(YELLOW)Executando $$RUNS testes para benchmark...$(NC)"; \
	for i in $$(seq 1 $$RUNS); do \
		echo "Run $$i/$$RUNS..."; \
		./$(TARGET) $(FILE) >> $(RESULTS_DIR)/benchmark_tmp.txt; \
	done
	@echo "$(GREEN)Benchmark completo!$(NC)"

# ========== LIMPEZA ==========

clean:
	@echo "$(YELLOW)Limpando ficheiros objeto...$(NC)"
	rm -f $(OBJ)

distclean: clean
	@echo "$(YELLOW)Limpando executável e resultados...$(NC)"
	rm -f $(TARGET)
	rm -rf $(RESULTS_DIR)

# Remove tudo incluindo testes
mrproper: distclean
	@echo "$(RED)Removendo todos os ficheiros gerados...$(NC)"
	rm -rf $(TEST_DIR)

# ========== HELP ==========

help:
	@echo "$(GREEN)Makefile para Problema de Diversidade$(NC)"
	@echo ""
	@echo "$(YELLOW)Comandos disponíveis:$(NC)"
	@echo "  make              - Compila o programa"
	@echo "  make setup        - Cria diretórios necessários"
	@echo "  make generate_test - Gera ficheiro de teste exemplo"
	@echo "  make test FILE=<ficheiro> - Executa teste específico"
	@echo "  make test_all     - Executa todos os testes em $(TEST_DIR)"
	@echo "  make valgrind FILE=<ficheiro> - Executa com valgrind"
	@echo "  make benchmark FILE=<ficheiro> RUNS=10 - Executa benchmark"
	@echo "  make clean        - Remove ficheiros objeto"
	@echo "  make distclean    - Remove executável e resultados"
	@echo "  make mrproper     - Remove tudo"
	@echo "  make help         - Mostra esta ajuda"
	@echo ""
	@echo "$(YELLOW)Exemplos:$(NC)"
	@echo "  make && make generate_test && make test FILE=tests/test_small.txt"
	@echo "  make test_all"
	@echo "  make benchmark FILE=tests/test_small.txt RUNS=5"

.PHONY: all clean distclean mrproper test test_all setup generate_test valgrind benchmark help