#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "bfs.h"
#include "graph.h"
#include "hash_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/*
 * Programma principale per leggere il grafo delle distanze tra città italiane
 * da un file CSV, costruire il grafo e scrivere l'ordine della BFS su un file.
 */

// --- INIZIO FIX PER WINDOWS (MinGW) ---
#ifdef _WIN32
#include <errno.h>
#include <stdint.h>

// A volte MinGW non definisce ssize_t di default in C11
#ifndef ssize_t
typedef intptr_t ssize_t;
#endif

/** @brief Implementazione manuale di getline per rimpiazzare quella di Linux.
 * @param lineptr Puntatore a un puntatore alla stringa.
 * @param n Puntatore alla dimensione della stringa.
 * @param stream Puntatore al file.
 * @return Il numero di caratteri letti, o -1 in caso di errore.
 */
static ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (*lineptr == NULL) {
        *n = 128;
        if ((*lineptr = malloc(*n)) == NULL) return -1;
    }

    pos = 0;
    while ((c = fgetc(stream)) != EOF) {
        if (pos + 1 >= *n) {
            size_t new_n = *n + (*n >> 2) + 128;
            char *new_ptr = realloc(*lineptr, new_n);
            if (new_ptr == NULL) return -1;
            *n = new_n;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos++] = c;
        if (c == '\n') break;
    }

    if (c == EOF && pos == 0) return -1;

    (*lineptr)[pos] = '\0';
    return pos;
}
#endif
// --- FINE FIX ---

/** @brief Funzione di confronto per stringhe, usata come callback per il grafo e la tabella hash.
 * @param a Puntatore alla prima stringa.
 * @param b Puntatore alla seconda stringa.
 * @return 0 se le stringhe sono uguali, un valore negativo se a < b, un valore positivo se a > b.
 */
static int str_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

static unsigned long str_hash(const void* p) {
    const unsigned char* s = (const unsigned char*)p;
    unsigned long h = 5381;
    int c;

    while ((c = *s++) != 0) {
        h = ((h << 5) + h) + (unsigned long)c;
    }

    return h;
}

/** @brief Duplica una stringa usando malloc. 
 * @param s Stringa da duplicare.
 * @return Puntatore alla stringa duplicata, NULL in caso di errore di allocazione.
 */
static char* xstrdup(const char* s) {
    size_t len = strlen(s);
    char* out = malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, s, len + 1);
    return out;
}

/** @brief Rimuove i caratteri di newline finali da una stringa.
 * @param s Stringa da modificare in-place.
 */
static void chomp_newline(char* s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

/** @brief Interna il nome di una città: se esiste già lo riusa, altrimenti clona la stringa e memorizza il nodo nel grafo e nella tabella dei nomi.
 * @param names Puntatore alla tabella hash dei nomi.
 * @param gr Puntatore al grafo.
 * @param name Nome della città.
 * @return Puntatore alla stringa internata, NULL in caso di errore.
 */
static char* intern_city(HashTable* names, Graph gr, const char* name) {
    char* existing = ht_get(names, name);
    char* dup;

    if (existing != NULL) {
        return existing;
    }

    dup = xstrdup(name);
    if (dup == NULL) {
        return NULL;
    }

    if (ht_put(names, dup, dup) < 0) {
        free(dup);
        return NULL;
    }

    if (!graph_add_node(gr, dup)) {
        ht_remove(names, dup, NULL);
        free(dup);
        return NULL;
    }

    return dup;
}

/** @brief Libera tutte le etichette degli archi del grafo.
 * @param gr Puntatore al grafo.
 */
static void free_graph_labels(Graph gr) {
    Edge** edges;
    int n;
    int i;

    edges = graph_get_edges(gr);
    n = graph_num_edges(gr);

    for (i = 0; i < n; ++i) {
        if (edges[i] != NULL) {
            free(edges[i]->label);
            free(edges[i]);
        }
    }

    free(edges);
}

/** @brief Libera la memoria occupata dalla tabella dei nomi delle città.
 * @param names Puntatore alla tabella hash dei nomi.
 */
static void free_city_pool(HashTable* names) {
    void** keys;
    size_t n;
    size_t i;

    keys = ht_keys(names);
    n = ht_size(names);

    for (i = 0; i < n; ++i) {
        free(keys[i]);
    }

    free(keys);
    ht_free(names, NULL);
}

/* Funzione principale: legge il CSV, costruisce il grafo e salva l'ordine BFS. */
int main(int argc, char** argv) {
    FILE* in;
    FILE* out;
    Graph gr;
    HashTable* names;
    char* line;
    size_t line_cap;
    ssize_t line_len;
    char* start;
    void** visit;
    int i;

    if (argc != 4) {
        fprintf(stderr, "Uso: %s <italian_dist_graph.csv> <start_city> <output_file>\n", argv[0]);
        return 1;
    }

    in = fopen(argv[1], "r");
    if (in == NULL) {
        fprintf(stderr, "Errore: impossibile aprire %s\n", argv[1]);
        return 1;
    }

    out = fopen(argv[3], "w");
    if (out == NULL) {
        fprintf(stderr, "Errore: impossibile aprire %s\n", argv[3]);
        fclose(in);
        return 1;
    }

    gr = graph_create(1, 0, str_compare, str_hash);
    names = ht_create(str_compare, str_hash);
    if (gr == NULL || names == NULL) {
        fprintf(stderr, "Errore: memoria insufficiente\n");
        fclose(in);
        fclose(out);
        graph_free(gr);
        ht_free(names, NULL);
        return 1;
    }

    line = NULL;
    line_cap = 0;

    while ((line_len = getline(&line, &line_cap, in)) != -1) {
        char* c1;
        char* c2;
        char* place1;
        char* place2;
        char* dist_s;
        char* endptr;
        double* dist;
        char* n1;
        char* n2;

        (void)line_len;

        chomp_newline(line);
        if (line[0] == '\0') {
            continue;
        }

        c1 = strchr(line, ',');
        if (c1 == NULL) {
            continue;
        }
        c2 = strchr(c1 + 1, ',');
        if (c2 == NULL) {
            continue;
        }

        *c1 = '\0';
        *c2 = '\0';

        place1 = line;
        place2 = c1 + 1;
        dist_s = c2 + 1;

        dist = malloc(sizeof(*dist));
        if (dist == NULL) {
            continue;
        }

        *dist = strtod(dist_s, &endptr);
        if (endptr == dist_s) {
            free(dist);
            continue;
        }

        n1 = intern_city(names, gr, place1);
        n2 = intern_city(names, gr, place2);
        if (n1 == NULL || n2 == NULL) {
            free(dist);
            continue;
        }

        if (graph_contains_edge(gr, n1, n2)) {
            void* old_label = graph_get_label(gr, n1, n2);
            free(old_label);
        }

        if (!graph_add_edge(gr, n1, n2, dist)) {
            free(dist);
        }
    }

    free(line);
    fclose(in);

    start = ht_get(names, argv[2]);
    if (start == NULL) {
        fprintf(stderr, "Errore: nodo di partenza '%s' non presente nel grafo\n", argv[2]);
        free_graph_labels(gr);
        graph_free(gr);
        free_city_pool(names);
        fclose(out);
        return 1;
    }

    visit = breadth_first_visit(gr, start, str_compare, str_hash);
    if (visit == NULL) {
        fprintf(stderr, "Errore: BFS fallita\n");
        free_graph_labels(gr);
        graph_free(gr);
        free_city_pool(names);
        fclose(out);
        return 1;
    }

    for (i = 0; visit[i] != NULL; ++i) {
        fprintf(out, "%s\n", (char*)visit[i]);
    }

    free(visit);
    fclose(out);

    free_graph_labels(gr);
    graph_free(gr);
    free_city_pool(names);
    return 0;
}