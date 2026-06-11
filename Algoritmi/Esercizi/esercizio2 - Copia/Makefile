CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -Isrc -mconsole
BIN_DIR = bin
SRC_DIR = src

all: $(BIN_DIR)/main_ex2.exe $(BIN_DIR)/test_ex2.exe

$(BIN_DIR)/main_ex2.exe: $(SRC_DIR)/main_ex2.c $(SRC_DIR)/hash_table.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_ex2.exe: $(SRC_DIR)/test_ex2.c $(SRC_DIR)/hash_table.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BIN_DIR)/*.exe