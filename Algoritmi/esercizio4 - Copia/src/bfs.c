#include "bfs.h"

#include "hash_table.h"

#include <stddef.h>
#include <stdlib.h>

/** @brief Struttura per rappresentare una coda circolare di puntatori.
 * @param data Array dinamico di elementi.
 * @param head Indice del primo elemento valido.
 * @param size Numero di elementi correnti nella coda.
 * @param capacity Capacità dell'array allocato.
 */
typedef struct {
    void** data;
    size_t head;
    size_t size;
    size_t capacity;
} Queue;

/** @brief Inizializza la coda.
 * @param q Puntatore alla coda.
 * @return 1 se l'allocazione ha successo, 0 altrimenti.
 */
static int queue_init(Queue* q) {
    q->capacity = 16;
    q->data = malloc(sizeof(void*) * q->capacity);
    if (q->data == NULL) {
        return 0;
    }
    q->head = 0;
    q->size = 0;
    return 1;
}

/** @brief Libera la memoria usata dalla coda.
 * @param q Puntatore alla coda.
 */
static void queue_free(Queue* q) {
    free(q->data);
}

/** @brief Inserisce un elemento in coda. Raddoppia la capacità se necessario.
 * @param q Puntatore alla coda.
 * @param value Puntatore all'elemento da inserire.
 * @return 1 in caso di successo, 0 in caso di errore di allocazione.
 */
static int queue_push(Queue* q, void* value) {
    size_t idx;

    if (q->size == q->capacity) {
        size_t i;
        size_t new_capacity = q->capacity * 2;
        void** new_data = malloc(sizeof(void*) * new_capacity);
        if (new_data == NULL) {
            return 0;
        }

        for (i = 0; i < q->size; ++i) {
            new_data[i] = q->data[(q->head + i) % q->capacity];
        }

        free(q->data);
        q->data = new_data;
        q->capacity = new_capacity;
        q->head = 0;
    }

    idx = (q->head + q->size) % q->capacity;
    q->data[idx] = value;
    q->size += 1;
    return 1;
}

/** @brief Estrae un elemento dalla coda e lo restituisce. Restituisce NULL se la coda è vuota.
 * @param q Puntatore alla coda.
 * @return Puntatore all'elemento estratto, o NULL se la coda è vuota.
 */
static void* queue_pop(Queue* q) {
    void* value;

    if (q->size == 0) {
        return NULL;
    }

    value = q->data[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size -= 1;
    return value;
}

/** @brief Esegue una visita in ampiezza (BFS) su un grafo a partire da un nodo di inizio.
 * @param gr Puntatore al grafo.
 * @param start Puntatore al nodo di partenza.
 * @param compare Funzione per confrontare i nodi.
 * @param hash Funzione per calcolare l'hash dei nodi.
 * @return Un array terminato da NULL con i nodi visitati nell'ordine di esplorazione, o NULL in caso di errore.
 */
void** breadth_first_visit(Graph gr,void* start,int (*compare)(const void*, const void*),unsigned long (*hash)(const void*)) {
    HashTable* visited;
    Queue queue;
    void** order;
    int max_nodes;
    int visited_count;
    if (gr == NULL || start == NULL || compare == NULL || hash == NULL) {
        return NULL;
    }
    if (!graph_contains_node(gr, start)) {
        return NULL;
    }
    max_nodes = graph_num_nodes(gr);
    order = malloc(sizeof(void*) * (size_t)(max_nodes + 1));
    if (order == NULL) {
        return NULL;
    }
    // Tabella hash per tracciare i nodi già visitati.
    visited = ht_create(compare, hash);
    if (visited == NULL) {
        free(order);
        return NULL;
    }
    if (!queue_init(&queue)) {
        ht_free(visited, NULL);
        free(order);
        return NULL;
    }
    if (ht_put(visited, start, (void*)1) < 0 || !queue_push(&queue, start)) {
        queue_free(&queue);
        ht_free(visited, NULL);
        free(order);
        return NULL;
    }
    visited_count = 0;
    while (queue.size > 0) {
        int i;
        int n_neigh;
        void* current = queue_pop(&queue);
        void** neigh = graph_get_neighbours(gr, current);
        order[visited_count++] = current;
        n_neigh = graph_num_neighbours(gr, current);
        for (i = 0; i < n_neigh; ++i) {
            if (!ht_contains(visited, neigh[i])) {
                if (ht_put(visited, neigh[i], (void*)1) >= 0) {
                    queue_push(&queue, neigh[i]);
                }
            }
        }
        free(neigh);
    }
    order[visited_count] = NULL;
    queue_free(&queue);
    ht_free(visited, NULL);
    return order;
}
