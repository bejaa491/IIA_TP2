CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99
LDFLAGS = -lm

TARGET = tp2
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

test_csv: $(TARGET)
	./run_tests.sh


.PHONY: all clean test_csv