# Nome do compilador
CC = gcc

# Flags de compilação (Avisos + Otimização)
CFLAGS = -Wall -Wextra -O2

# Bibliotecas (Math library)
LIBS = -lm

# Diretoria do código fonte
SRC_DIR = src

# Nome do executável final
TARGET = projeto

# Lista de todos os ficheiros .c dentro de src/
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Regra principal (default)
all:
	@echo "A compilar o projeto..."
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo "Sucesso! Executavel '$(TARGET)' criado."

# Regra para limpar o executável (se quiseres recomeçar)
clean:
	@echo "A limpar..."
	rm -f $(TARGET)

# Corre o script que gera os CSV/TXT de testes
test1: all
	./run_experiments1.sh

test2: all
	./run_experiments2.sh
.PHONY: all clean test1 test2
