#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"

/**
 * @brief Funzione hash DJB2 (Daniel J. Bernstein) per stringhe.
 *
 * Algoritmo semplice ma efficace che produce buona distribuzione.
 * Formula: hash = hash * 33 + carattere_corrente
 *
 * @param ptr Puntatore a una stringa (const char*).
 *
 * @return Valore hash della stringa.
 */
unsigned long string_hash(const void* ptr) {
    unsigned long hash = 5381;
    const char* str = (const char*)ptr;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

/**
 * @brief Funzione di confronto standard per stringhe.
 *
 * @param p1 Puntatore alla prima stringa.
 * @param p2 Puntatore alla seconda stringa.
 *
 * @return 0 se uguali, <0 o >0 altrimenti (come strcmp).
 */
int string_compare(const void* p1, const void* p2) {
    return strcmp((const char*)p1, (const char*)p2);
}

/**
 * @brief Pulisce una parola rimuovendo caratteri non alfabetici e convertendo in minuscole.
 *
 * @param word Puntatore alla parola da pulire (modificata in-place).
 */
void clean_word(char* word) {
    char *src = word, *dst = word;
    while (*src) {
        if (isalpha((unsigned char)*src)) {
            *dst = tolower((unsigned char)*src);
            dst++;
        }
        src++;
    }
    *dst = '\0';
}

/**
 * @brief Trova la parola piu' frequente in un file di testo.
 *
 * Legge parole dal file, le pulisce, e usa una hash table per contare
 * le occorrenze. Stampa la parola piu' frequente con lunghezza >= min_len.
 *
 * Uso: <programma> <file> <min_len>
 *   - file: percorso del file di testo da analizzare
 *   - min_len: lunghezza minima delle parole da considerare
 *
 * @param argc Numero di argomenti sulla linea di comando.
 * @param argv Array di argomenti sulla linea di comando.
 *
 * @return 0 se successo, 1 se errore.
 */
int main(int argc, char* argv[]) {
    if (argc < 3) return printf("Uso: %s <file> <min_len>\n", argv[0]), 1;

    FILE* file = fopen(argv[1], "r");
    if (!file) return perror("Errore file"), 1;

    int min_len = atoi(argv[2]);
    HashTable* table = hash_table_create(string_compare, string_hash);
    char buffer[1024];

    while (fscanf(file, "%1023s", buffer) == 1) {
        clean_word(buffer);
        if ((int)strlen(buffer) >= min_len) {
            int* count = (int*)hash_table_get(table, buffer);
            if (!count) {
                char* new_key = strdup(buffer);
                int* new_cnt = malloc(sizeof(int));
                *new_cnt = 1;
                hash_table_put(table, new_key, new_cnt);
            } else (*count)++;
        }
    }
    fclose(file);

    void** keys = hash_table_keyset(table);
    int size = hash_table_size(table);
    char* max_w = NULL;
    int max_f = -1;

    for (int i = 0; i < size; i++) {
        int f = *(int*)hash_table_get(table, keys[i]);
        if (f > max_f) { max_f = f; max_w = (char*)keys[i]; }
    }

    if (max_w) printf("Parola piu' frequente: '%s' (%d occorrenze)\n", max_w, max_f);

    if (keys) {
        for (int i = 0; i < size; i++) {
            int* val = (int*)hash_table_get(table, keys[i]);
            if (val) free(val);
        }
        for (int i = 0; i < size; i++) free(keys[i]);
        free(keys);
    }
    hash_table_free(table);
    return 0;
}
