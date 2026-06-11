# --- CONFIGURAZIONE ---
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -I$(INCLUDE_DIR)
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

# Nomi degli eseguibili richiesti dalla consegna
MAIN_TARGET = $(BIN_DIR)/main_ex3
TEST_TARGET = $(BIN_DIR)/test_ex3

# File sorgente condivisi tra programma principale e test
COMMON_SRCS = $(SRC_DIR)/pq.c $(SRC_DIR)/hash.c

# --- TARGET PRINCIPALI ---
all: $(MAIN_TARGET) $(TEST_TARGET)

# Compilazione del programma principale
$(MAIN_TARGET): $(SRC_DIR)/main.c $(COMMON_SRCS) $(INCLUDE_DIR)/pq.h $(INCLUDE_DIR)/hash.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilazione dell'eseguibile dei test
$(TEST_TARGET): $(SRC_DIR)/test.c $(COMMON_SRCS) $(INCLUDE_DIR)/pq.h $(INCLUDE_DIR)/hash.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Pulizia dei file compilati
clean:
	rm -f $(MAIN_TARGET) $(TEST_TARGET)
