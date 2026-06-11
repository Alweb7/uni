CC = gcc
CFLAGS = -D_GNU_SOURCE -std=c11 -Wall -Wextra -Werror -Wvla -pedantic -O2 -Iinclude
LDFLAGS = -pthread -lrt

# Oggetti condivisi tra i tre binari principali della simulazione.
COMMON_OBJS = src/common.o src/config.o src/ipc.o src/simulation.o

# Build completa di tutti gli eseguibili richiesti dal progetto.
all: manager worker user disorder addusers

manager: src/manager.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

worker: src/worker.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

user: src/user.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

disorder: src/disorder.o src/common.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

addusers: src/addusers.o src/common.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	# Rimuove eseguibili, oggetti intermedi e CSV statistico generato in esecuzione.
	rm -f manager worker user disorder addusers src/*.o stats.csv

.PHONY: all clean
