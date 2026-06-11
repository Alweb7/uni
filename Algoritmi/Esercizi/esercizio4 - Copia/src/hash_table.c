#include "hash_table.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

/** @brief Nodo di una lista collegata usata per risolvere collisioni. 
 * @param key Chiave memorizzata nel nodo.
 * @param value Valore associato alla chiave.
 * @param next Puntatore al nodo successivo nella lista.
*/
typedef struct _Node {
    void* key;
    void* value;
    struct _Node* next;
} Node;

struct _HashTable {
    Node** buckets;
    int size;
    unsigned long capacity;
    int (*compare)(const void*, const void*);
    unsigned long (*hash)(const void*);
};

HashTable* ht_create(int (*compare)(const void*, const void*), unsigned long (*hash)(const void*)) {
    HashTable* table;

    if (compare == NULL || hash == NULL) {
        return NULL;
    }

    table = malloc(sizeof(*table));
    if (table == NULL) {
        return NULL;
    }

    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->compare = compare;
    table->hash = hash;
    table->buckets = calloc(table->capacity, sizeof(*table->buckets));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

static int hash_table_resize(HashTable* table) {
    unsigned long old_capacity = table->capacity;
    Node** old_buckets = table->buckets;
    unsigned long new_capacity = table->capacity * 2;
    Node** new_buckets = calloc(new_capacity, sizeof(*new_buckets));

    if (new_buckets == NULL) {
        return 0;
    }

    table->capacity = new_capacity;
    table->buckets = new_buckets;

    for (unsigned long i = 0; i < old_capacity; i++) {
        Node* curr = old_buckets[i];
        while (curr) {
            Node* next = curr->next;
            unsigned long new_idx = table->hash(curr->key) % table->capacity;
            curr->next = table->buckets[new_idx];
            table->buckets[new_idx] = curr;
            curr = next;
        }
    }
    free(old_buckets);
    return 1;
}

int ht_put(HashTable* table, const void* key, const void* value) {
    Node* new_node;
    unsigned long idx;

    if (table == NULL || key == NULL) {
        return -1;
    }

    idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    while (curr) {
        if (table->compare(curr->key, key) == 0) {
            curr->value = (void*)value;
            return 0;
        }
        curr = curr->next;
    }

    if ((double)table->size / table->capacity >= LOAD_FACTOR_THRESHOLD) {
        if (!hash_table_resize(table)) {
            return -1;
        }
        idx = table->hash(key) % table->capacity;
    }

    new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return -1;
    }

    new_node->key = (void*)key;
    new_node->value = (void*)value;
    new_node->next = table->buckets[idx];
    table->buckets[idx] = new_node;
    table->size++;
    return 0;
}

void* ht_get(const HashTable* table, const void* key) {
    if (!table || !key) return NULL;
    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    while (curr) {
        if (table->compare(curr->key, key) == 0) return curr->value;
        curr = curr->next;
    }
    return NULL;
}

int ht_contains(const HashTable* table, const void* key) {
    if (table == NULL || key == NULL) {
        return 0;
    }

    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    while (curr) {
        if (table->compare(curr->key, key) == 0) {
            return 1;
        }
        curr = curr->next;
    }

    return 0;
}

int ht_remove(HashTable* table, const void* key, void** old_value) {
    Node* curr;
    Node* prev;
    unsigned long idx;

    if (table == NULL || key == NULL) {
        return 0;
    }

    idx = table->hash(key) % table->capacity;
    curr = table->buckets[idx];
    prev = NULL;

    while (curr) {
        if (table->compare(curr->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[idx] = curr->next;
            } else {
                prev->next = curr->next;
            }

            if (old_value != NULL) {
                *old_value = curr->value;
            }

            free(curr);
            table->size--;
            return 1;
        }

        prev = curr;
        curr = curr->next;
    }

    return 0;
}

size_t ht_size(const HashTable* table) {
    return table ? (size_t)table->size : 0;
}

void** ht_keys(const HashTable* table) {
    if (!table || table->size == 0) return NULL;
    void** keys = calloc((size_t)table->size, sizeof(*keys));
    if (keys == NULL) {
        return NULL;
    }

    int k = 0;
    for (unsigned long i = 0; i < table->capacity; i++) {
        Node* curr = table->buckets[i];
        while (curr && k < table->size) {
            keys[k++] = curr->key;
            curr = curr->next;
        }
    }
    return keys;
}

void ht_free(HashTable* table, void (*free_value)(void*)) {
    if (!table) return;
    for (unsigned long i = 0; i < table->capacity; i++) {
        Node* curr = table->buckets[i];
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            if (free_value != NULL) {
                free_value(temp->value);
            }
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}
