
#include <stdlib.h>
#include "hash.h"

/**
 * @brief Singola entry della tabella hash.
 */
typedef struct {
    const void* key;
    int value;
    int state;
} Entry;

enum {
    ENTRY_EMPTY = 0,
    ENTRY_USED = 1,
    ENTRY_DELETED = 2
};

struct Hash {
    Entry* table;
    int size;
    int cap;
    unsigned long (*hash)(const void*);
};

/**
 * @brief Calcola una capacita' iniziale come potenza di due.
 *
 * @param requested Capacita' minima richiesta.
 * @return Prima potenza di due maggiore o uguale a `requested`.
 */
static int next_capacity(int requested) {
    int cap = 16;

    while (cap < requested) {
        cap *= 2;
    }

    return cap;
}

/**
 * @brief Cerca lo slot associato a una chiave.
 *
 * Se la chiave e' presente restituisce il suo slot; altrimenti restituisce
 * il primo slot disponibile compatibile con l'inserimento.
 *
 * @param m Tabella hash da interrogare.
 * @param key Chiave da cercare.
 * @param found Flag impostato a `1` se la chiave e' stata trovata.
 * @return Indice dello slot individuato.
 */
static int find_slot(const Hash* m, const void* key, int* found) {
    int index;
    int start;
    int first_deleted = -1;

    *found = 0;
    index = (int)(m->hash(key) % (unsigned long)m->cap);
    start = index;

    while (m->table[index].state != ENTRY_EMPTY) {
        if (m->table[index].state == ENTRY_USED && m->table[index].key == key) {
            *found = 1;
            return index;
        }

        if (first_deleted == -1 && m->table[index].state == ENTRY_DELETED) {
            first_deleted = index;
        }

        index = (index + 1) % m->cap;
        if (index == start) {
            break;
        }
    }

    if (first_deleted != -1) {
        return first_deleted;
    }

    return index;
}

/**
 * @brief Ridimensiona la tabella hash reinserendo tutte le entry valide.
 *
 * @param m Tabella da ridimensionare.
 * @param new_capacity Nuova capacita' desiderata.
 * @return `0` se il ridimensionamento ha successo, `-1` in caso di errore.
 */
static int hash_resize(Hash* m, int new_capacity) {
    Entry* old_table = m->table;
    int old_cap = m->cap;
    Entry* new_table;
    int old_size = m->size;
    int i;

    new_table = calloc((size_t)new_capacity, sizeof(Entry));
    if (!new_table) {
        return -1;
    }

    m->table = new_table;
    m->cap = new_capacity;
    m->size = 0;

    for (i = 0; i < old_cap; i++) {
        int found;
        int slot;

        if (old_table[i].state != ENTRY_USED) {
            continue;
        }

        slot = find_slot(m, old_table[i].key, &found);
        m->table[slot] = old_table[i];
        m->size++;
    }

    free(old_table);
    return m->size == old_size ? 0 : -1;
}

Hash* hash_create(unsigned long (*hash_fn)(const void*), int capacity) {
    Hash* map;

    if (!hash_fn) {
        return NULL;
    }

    map = malloc(sizeof(*map));
    if (!map) {
        return NULL;
    }

    map->cap = next_capacity(capacity > 0 ? capacity : 16);
    map->size = 0;
    map->hash = hash_fn;
    map->table = calloc((size_t)map->cap, sizeof(Entry));

    if (!map->table) {
        free(map);
        return NULL;
    }

    return map;
}

int hash_put(Hash* m, const void* key, int value) {
    int found;
    int slot;

    if (!m || !key) {
        return -1;
    }

    if ((m->size + 1) * 10 >= m->cap * 7 && hash_resize(m, m->cap * 2) != 0) {
        return -1;
    }

    slot = find_slot(m, key, &found);
    if (!found) {
        m->size++;
    }

    m->table[slot].key = key;
    m->table[slot].value = value;
    m->table[slot].state = ENTRY_USED;
    return 0;
}

int hash_get(const Hash* m, const void* key) {
    int found;
    int slot;

    if (!m || !key) {
        return -1;
    }

    slot = find_slot(m, key, &found);
    return found ? m->table[slot].value : -1;
}

int hash_contains(const Hash* m, const void* key) {
    return hash_get(m, key) != -1;
}

int hash_remove(Hash* m, const void* key) {
    int found;
    int slot;

    if (!m || !key) {
        return -1;
    }

    slot = find_slot(m, key, &found);
    if (!found) {
        return -1;
    }

    m->table[slot].state = ENTRY_DELETED;
    m->size--;
    return 0;
}

void hash_free(Hash* m) {
    if (!m) {
        return;
    }

    free(m->table);
    free(m);
}
