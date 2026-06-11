#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash_table.h"

/**
 * @brief Funzione hash alternativa per test (non DJB2).
 *
 * @param ptr Puntatore a una stringa (const char*).
 *
 * @return Valore hash della stringa.
 */
unsigned long simple_hash(const void* ptr) {
    const char* str = (const char*)ptr;
    unsigned long hash = 0;
    while (*str) hash = hash * 31 + *str++;
    return hash;
}

/**
 * @brief Funzione di confronto standard per stringhe.
 *
 * @param p1 Puntatore alla prima stringa.
 * @param p2 Puntatore alla seconda stringa.
 *
 * @return 0 se uguali, <0 o >0 altrimenti.
 */
int simple_compare(const void* p1, const void* p2) {
    return strcmp((const char*)p1, (const char*)p2);
}

/**
 * @brief Test di creazione della hash table.
 *
 * Verifica che una tavola vuota appena creata abbia dimensione 0.
 */
void test_create() {
    printf("Testing: Creazione... ");
    HashTable* ht = hash_table_create(simple_compare, simple_hash);
    assert(ht != NULL);
    assert(hash_table_size(ht) == 0);
    hash_table_free(ht);
    printf("PASS\n");
}

/**
 * @brief Test di inserimento e recupero.
 *
 * Verifica che put e get funzionino correttamente.
 */
void test_put_get() {
    printf("Testing: Inserimento e Recupero... ");
    HashTable* ht = hash_table_create(simple_compare, simple_hash);
    hash_table_put(ht, "chiave1", "valore1");
    assert(hash_table_size(ht) == 1);
    assert(strcmp((char*)hash_table_get(ht, "chiave1"), "valore1") == 0);
    hash_table_free(ht);
    printf("PASS\n");
}

/**
 * @brief Test del ridimensionamento automatico (rehashing).
 *
 * Inserisce 100 elementi per forzare il ridimensionamento della tavola.
 * Verifica che il ridimensionamento non perda dati.
 */
void test_resize() {
    printf("Testing: Ridimensionamento automatico (Rehash)... ");
    HashTable* ht = hash_table_create(simple_compare, simple_hash);
    char keys[100][10];
    for(int i = 0; i < 100; i++) {
        sprintf(keys[i], "k%d", i);
        hash_table_put(ht, keys[i], "val");
    }
    assert(hash_table_size(ht) == 100);
    assert(strcmp((char*)hash_table_get(ht, "k99"), "val") == 0);
    hash_table_free(ht);
    printf("PASS\n");
}

/**
 * @brief Test della funzione contains_key.
 *
 * Verifica che contains_key ritorni 1 per chiavi presenti e 0 per chiavi assenti.
 */
void test_contains_key() {
    printf("Testing: Contains Key... ");
    HashTable* ht = hash_table_create(simple_compare, simple_hash);
    hash_table_put(ht, "chiave1", "valore1");
    hash_table_put(ht, "chiave2", "valore2");
    assert(hash_table_contains_key(ht, "chiave1") == 1);
    assert(hash_table_contains_key(ht, "chiave2") == 1);
    assert(hash_table_contains_key(ht, "chiave3") == 0);
    hash_table_free(ht);
    printf("PASS\n");
}

/**
 * @brief Test della funzione remove.
 *
 * Verifica che remove diminuisca la size correttamente e che la chiave
 * non sia piu' presente dopo la rimozione.
 */
void test_remove() {
    printf("Testing: Remove... ");
    HashTable* ht = hash_table_create(simple_compare, simple_hash);
    hash_table_put(ht, "chiave1", "valore1");
    hash_table_put(ht, "chiave2", "valore2");
    hash_table_put(ht, "chiave3", "valore3");
    assert(hash_table_size(ht) == 3);

    hash_table_remove(ht, "chiave2");
    assert(hash_table_size(ht) == 2);
    assert(hash_table_contains_key(ht, "chiave2") == 0);
    assert(hash_table_contains_key(ht, "chiave1") == 1);
    assert(hash_table_contains_key(ht, "chiave3") == 1);

    hash_table_remove(ht, "chiave1");
    assert(hash_table_size(ht) == 1);
    assert(hash_table_get(ht, "chiave3") != NULL);

    hash_table_free(ht);
    printf("PASS\n");
}

/**
 * @brief Esegue tutti gli unit test.
 *
 * @return 0 se tutti i test passano.
 */
int main() {
    printf("=== UNIT TESTS: HASH TABLE ===\n");
    test_create();
    test_put_get();
    test_resize();
    test_contains_key();
    test_remove();
    printf("Tutti i test superati con successo!\n");
    return 0;
}
