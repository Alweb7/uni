#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

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

/**
 * @brief Ridimensiona la tavola raddoppiando la capacita'.
 *
 * Esegue il rehashing di tutti gli elementi con la nuova capacita'.
 * Questo metodo e' chiamato automaticamente quando il fattore di carico
 * supera LOAD_FACTOR_THRESHOLD.
 *
 * @param table Puntatore alla tavola hash.
 */
static void hash_table_resize(HashTable* table) {
    unsigned long old_capacity = table->capacity;
    Node** old_buckets = table->buckets;
    table->capacity *= 2;
    table->buckets = (Node**)calloc(table->capacity, sizeof(Node*));

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
}

HashTable* hash_table_create(int (*f1)(const void*, const void*), unsigned long (*f2)(const void*)) {
    if (!f1 || !f2) return NULL;
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->compare = f1;
    table->hash = f2;
    table->buckets = (Node**)calloc(table->capacity, sizeof(Node*));
    return table;
}

void hash_table_put(HashTable* table, const void* key, const void* value) {
    if (!table) return;
    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];

    while (curr) {
        if (table->compare(curr->key, key) == 0) {
            curr->value = (void*)value;
            return;
        }
        curr = curr->next;
    }

    if ((double)table->size / table->capacity >= LOAD_FACTOR_THRESHOLD) {
        hash_table_resize(table);
        idx = table->hash(key) % table->capacity;
    }

    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->key = (void*)key;
    new_node->value = (void*)value;
    new_node->next = table->buckets[idx];
    table->buckets[idx] = new_node;
    table->size++;
}

void* hash_table_get(const HashTable* table, const void* key) {
    if (!table || !key) return NULL;
    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    while (curr) {
        if (table->compare(curr->key, key) == 0) return curr->value;
        curr = curr->next;
    }
    return NULL;
}

int hash_table_contains_key(const HashTable* table, const void* key) {
    if (!table || !key) return 0;
    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    while (curr) {
        if (table->compare(curr->key, key) == 0) return 1;
        curr = curr->next;
    }
    return 0;
}

void hash_table_remove(HashTable* table, const void* key) {
    if (!table || !key) return;
    unsigned long idx = table->hash(key) % table->capacity;
    Node* curr = table->buckets[idx];
    Node* prev = NULL;

    while (curr) {
        if (table->compare(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else table->buckets[idx] = curr->next;
            free(curr);
            table->size--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int hash_table_size(const HashTable* table) {
    return table ? table->size : 0;
}

void** hash_table_keyset(const HashTable* table) {
    if (!table || table->size == 0) return NULL;
    void** keys = (void**)calloc(table->size, sizeof(void*));
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

void hash_table_free(HashTable* table) {
    if (!table) return;
    for (unsigned long i = 0; i < table->capacity; i++) {
        Node* curr = table->buckets[i];
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}
