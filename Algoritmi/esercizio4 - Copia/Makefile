CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude -Isrc
LDFLAGS =
.RECIPEPREFIX = >

SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin

LIB_SRCS = $(SRC_DIR)/hash_table.c $(SRC_DIR)/graph.c $(SRC_DIR)/bfs.c
MAIN_SRC = $(SRC_DIR)/main_ex4.c
TEST_SRC = $(TEST_DIR)/test_ex4.c

MAIN_BIN = $(BIN_DIR)/main_ex4
TEST_BIN = $(BIN_DIR)/test_ex4

all: $(MAIN_BIN) $(TEST_BIN)

$(BIN_DIR):
>mkdir -p $(BIN_DIR)

$(MAIN_BIN): $(LIB_SRCS) $(MAIN_SRC) | $(BIN_DIR)
>$(CC) $(CFLAGS) $(LIB_SRCS) $(MAIN_SRC) -o $@ $(LDFLAGS)

$(TEST_BIN): $(LIB_SRCS) $(TEST_SRC) | $(BIN_DIR)
>$(CC) $(CFLAGS) $(LIB_SRCS) $(TEST_SRC) -o $@ $(LDFLAGS)

test: $(TEST_BIN)
>./$(TEST_BIN)

clean:
>rm -f $(MAIN_BIN) $(TEST_BIN)

.PHONY: all test clean
