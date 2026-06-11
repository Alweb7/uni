
#ifndef PQ_H
#define PQ_H

/**
 * @brief Coda con priorita' generica implementata tramite heap ternario.
 */
typedef struct PriorityQueue PriorityQueue;

/**
 * @brief Crea una nuova coda con priorita'.
 *
 * @param cmp Funzione di confronto fra due elementi.
 * @param hash Funzione hash usata per tracciare la posizione degli elementi.
 * @return Puntatore alla coda creata, oppure `NULL` in caso di errore.
 */
PriorityQueue* priority_queue_create(
    int (*cmp)(const void*, const void*),
    unsigned long (*hash)(const void*)
);

/**
 * @brief Inserisce un elemento nella coda.
 *
 * @param pq Coda su cui operare.
 * @param item Elemento da inserire.
 * @return `0` se l'inserimento ha successo, `-1` in caso di errore.
 */
int priority_queue_push(PriorityQueue*, void*);

/**
 * @brief Verifica se un elemento e' presente nella coda.
 *
 * @param pq Coda su cui operare.
 * @param item Elemento da cercare.
 * @return `1` se presente, `0` se assente, `-1` se gli argomenti non sono validi.
 */
int priority_queue_contains(const PriorityQueue*, const void*);

/**
 * @brief Restituisce l'elemento con priorita' massima senza rimuoverlo.
 *
 * @param pq Coda su cui operare.
 * @return Puntatore all'elemento in testa oppure `NULL` se la coda e' vuota o invalida.
 */
void* priority_queue_top(const PriorityQueue*);

/**
 * @brief Rimuove l'elemento con priorita' massima.
 *
 * @param pq Coda su cui operare.
 */
void priority_queue_pop(PriorityQueue*);

/**
 * @brief Rimuove un elemento arbitrario dalla coda.
 *
 * @param pq Coda su cui operare.
 * @param item Elemento da rimuovere.
 * @return `0` se la rimozione ha successo, `-1` in caso di errore o elemento assente.
 */
int priority_queue_remove(PriorityQueue*, const void*);

/**
 * @brief Restituisce il numero di elementi presenti nella coda.
 *
 * @param pq Coda da interrogare.
 * @return Numero di elementi, oppure `-1` se la coda non e' valida.
 */
int priority_queue_size(const PriorityQueue*);

/**
 * @brief Libera tutta la memoria associata alla coda.
 *
 * @param pq Coda da distruggere.
 */
void priority_queue_free(PriorityQueue*);
#endif
