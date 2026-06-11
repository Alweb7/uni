
#include <stdlib.h>
#include "pq.h"
#include "hash.h"

/**
 * @brief Rappresentazione concreta della priority queue.
 *
 * `data` contiene lo heap ternario, mentre `map` associa ogni elemento
 * alla sua posizione corrente nello heap.
 */
struct PriorityQueue {
    void** data;
    int size, cap;
    int (*cmp)(const void*, const void*);
    Hash* map;
};

/**
 * @brief Restituisce l'indice del padre di un nodo nello heap ternario.
 *
 * @param index Indice del nodo figlio.
 * @return Indice del padre.
 */
static int parent_index(int index) {
    return (index - 1) / 3;
}

/**
 * @brief Trova il figlio con priorita' massima tra i tre figli di un nodo.
 *
 * @param pq Heap su cui operare.
 * @param index Indice del nodo padre.
 * @return Indice del nodo migliore tra padre e figli.
 */
static int highest_priority_child(const PriorityQueue* pq, int index) {
    int best = index;
    int child;

    for (child = 1; child <= 3; child++) {
        int child_index = 3 * index + child;

        if (child_index < pq->size &&
            pq->cmp(pq->data[child_index], pq->data[best]) > 0) {
            best = child_index;
        }
    }

    return best;
}

/**
 * @brief Scambia due elementi dello heap aggiornando anche la tabella hash.
 *
 * @param pq Heap su cui operare.
 * @param left Primo indice.
 * @param right Secondo indice.
 * @return `0` se lo scambio ha successo, `-1` in caso di errore.
 */
static int swap_entries(PriorityQueue* pq, int left, int right) {
    void* tmp = pq->data[left];

    pq->data[left] = pq->data[right];
    pq->data[right] = tmp;

    if (hash_put(pq->map, pq->data[left], left) != 0) {
        return -1;
    }

    if (hash_put(pq->map, pq->data[right], right) != 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief Ripristina la proprieta' di heap risalendo verso la radice.
 *
 * @param pq Heap su cui operare.
 * @param index Posizione iniziale dell'elemento.
 * @return Nuova posizione dell'elemento, oppure `-1` in caso di errore.
 */
static int sift_up(PriorityQueue* pq, int index) {
    while (index > 0) {
        int parent = parent_index(index);

        if (pq->cmp(pq->data[index], pq->data[parent]) <= 0) {
            break;
        }

        if (swap_entries(pq, index, parent) != 0) {
            return -1;
        }

        index = parent;
    }

    return index;
}

/**
 * @brief Ripristina la proprieta' di heap scendendo verso le foglie.
 *
 * @param pq Heap su cui operare.
 * @param index Posizione iniziale dell'elemento.
 * @return Nuova posizione dell'elemento, oppure `-1` in caso di errore.
 */
static int sift_down(PriorityQueue* pq, int index) {
    while (1) {
        int best = highest_priority_child(pq, index);

        if (best == index) {
            break;
        }

        if (swap_entries(pq, index, best) != 0) {
            return -1;
        }

        index = best;
    }

    return index;
}

PriorityQueue* priority_queue_create(
    int (*cmp)(const void*, const void*),
    unsigned long (*hash)(const void*)
) {
    PriorityQueue* pq;

    if (!cmp || !hash) {
        return NULL;
    }

    pq = malloc(sizeof(*pq));
    if (!pq) {
        return NULL;
    }

    pq->size = 0;
    pq->cap = 16;
    pq->cmp = cmp;
    pq->data = malloc(sizeof(void*) * (size_t)pq->cap);
    pq->map = hash_create(hash, pq->cap);

    if (!pq->data || !pq->map) {
        free(pq->data);
        hash_free(pq->map);
        free(pq);
        return NULL;
    }

    return pq;
}

int priority_queue_push(PriorityQueue* pq, void* item) {
    void** resized_data;

    if (!pq || !item) {
        return -1;
    }

    if (hash_contains(pq->map, item)) {
        return -1;
    }

    if (pq->size == pq->cap) {
        pq->cap *= 2;
        resized_data = realloc(pq->data, sizeof(void*) * (size_t)pq->cap);
        if (!resized_data) {
            return -1;
        }
        pq->data = resized_data;
    }

    pq->data[pq->size] = item;
    if (hash_put(pq->map, item, pq->size) != 0) {
        return -1;
    }

    if (sift_up(pq, pq->size) < 0) {
        hash_remove(pq->map, item);
        return -1;
    }

    pq->size++;
    return 0;
}

void* priority_queue_top(const PriorityQueue* pq) {
    if (!pq || pq->size == 0) {
        return NULL;
    }

    return pq->data[0];
}

void priority_queue_pop(PriorityQueue* pq) {
    void* removed_item;

    if (!pq || pq->size == 0) {
        return;
    }

    removed_item = pq->data[0];
    hash_remove(pq->map, removed_item);
    pq->size--;

    if (pq->size == 0) {
        return;
    }

    pq->data[0] = pq->data[pq->size];
    if (hash_put(pq->map, pq->data[0], 0) != 0) {
        return;
    }

    sift_down(pq, 0);
}

int priority_queue_contains(const PriorityQueue* pq, const void* item) {
    if (!pq || !item) {
        return -1;
    }

    return hash_contains(pq->map, item);
}

int priority_queue_remove(PriorityQueue* pq, const void* item) {
    int index;

    if (!pq || !item) {
        return -1;
    }

    index = hash_get(pq->map, item);
    if (index == -1) {
        return -1;
    }

    hash_remove(pq->map, item);
    pq->size--;

    if (index == pq->size) {
        return 0;
    }

    pq->data[index] = pq->data[pq->size];
    if (hash_put(pq->map, pq->data[index], index) != 0) {
        return -1;
    }

    index = sift_up(pq, index);
    if (index < 0) {
        return -1;
    }

    if (sift_down(pq, index) < 0) {
        return -1;
    }

    return 0;
}

int priority_queue_size(const PriorityQueue* pq) {
    return pq ? pq->size : -1;
}

void priority_queue_free(PriorityQueue* pq) {
    if (!pq) {
        return;
    }

    hash_free(pq->map);
    free(pq->data);
    free(pq);
}
