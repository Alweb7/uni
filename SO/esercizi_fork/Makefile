# Makefile per esercizi fork
# Uso: make es01   oppure   make all   oppure   make clean

# 1. VARIABILI (Macro)
# CC definisce quale compilatore usare (in questo caso, gcc).
CC      = gcc

# CFLAGS definisce i "flag" (le opzioni) da passare al compilatore:
# -Wall e -Wextra: accendono tutti gli avvisi (warning) per aiutarti a trovare errori.
# -g: aggiunge le informazioni di debug (fondamentale se devi usare gdb o valgrind).
CFLAGS  = -Wall -Wextra -g

# TARGETS è una lista dei nomi degli eseguibili che vuoi creare.
# Se domani aggiungi un "es02_fork_avanzato.c", ti basta aggiungere 
# il nome qui di fianco separato da uno spazio.
TARGETS = es01_fork_base

# 2. REGOLE (Targets)
# 'all' è tradizionalmente la regola di default. 
# Se scrivi solo "make" nel terminale, lui cerca la prima regola (questa)
# e capisce che per completare "all" deve prima costruire tutto ciò che c'è in $(TARGETS).
all: $(TARGETS)

# 3. LA REGOLA MAGICA (Pattern Rule)
# Il simbolo '%' è un jolly (wildcard).
# Questa riga significa: "Per costruire UN QUALSIASI eseguibile (%), 
# ho bisogno di un file sorgente che si chiami ESATTAMENTE ALLO STESSO MODO ma con .c (%.c)"
%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	

# Qui ci sono le Variabili Automatiche (spiegate sotto).
# Se chiami "make es01_fork_base", questa riga viene tradotta dal computer in:
# gcc -Wall -Wextra -g -o es01_fork_base es01_fork_base.c



# 4. REGOLA DI PULIZIA
# Elimina gli eseguibili generati. Il flag -f (force) evita che il terminale
# ti dia errore se i file sono già stati cancellati.
clean:
	rm -f $(TARGETS)

# 5. DIRETTIVA .PHONY
# Dice a Make: "Guarda che 'all' e 'clean' non sono veri nomi di file che voglio creare, 
# sono solo nomi di comandi". Questo evita bug se per caso crei un file vuoto chiamato "clean" nella cartella.
.PHONY: all clean