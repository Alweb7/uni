# --- CONFIGURAZIONE ---
CC = gcc
# Ottimizzazione portata a -O3 per la massima velocità di esecuzione 
# Aggiunto -I$(SRC_DIR) per trovare correttamente gli header (.h)
CFLAGS = -Wall -Wextra -std=c99 -O3 -I$(SRC_DIR)
BIN_DIR = bin
SRC_DIR = src

# Estensione .exe per chiarezza su Windows
MAIN_TARGET = $(BIN_DIR)/main_ex1.exe
TEST_TARGET = $(BIN_DIR)/test_ex1.exe

# File sorgente necessari (escluso sort_records.c perché incluso in main_ex1.c)
COMMON_SRCS = $(SRC_DIR)/lib_sort.c $(SRC_DIR)/record.c $(SRC_DIR)/record_cmp.c

# --- TARGET PRINCIPALI ---
all: $(MAIN_TARGET) $(TEST_TARGET)

# Compilazione del programma principale
$(MAIN_TARGET): $(SRC_DIR)/main_ex1.c $(COMMON_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilazione dei test
$(TEST_TARGET): $(SRC_DIR)/test_ex1.c $(COMMON_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Pulizia dei file compilati
clean:
	rm -rf $(BIN_DIR)/*.exe