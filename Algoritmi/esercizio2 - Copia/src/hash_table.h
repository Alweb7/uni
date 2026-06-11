#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>

/**
 * @brief Struttura opaca che rappresenta una tavola hash generica con concatenamento.
 */
typedef struct _HashTable HashTable;

/**
 * @brief Crea una nuova tavola hash vuota.
 *
 * @param f1 Funzione di confronto personalizzata per le chiavi. Ritorna 0 se chiavi uguali, != 0 altrimenti.
 * @param f2 Funzione di hash personalizzata. Ritorna il valore hash di una chiave.
 *
 * @return Puntatore a nuova HashTable, o NULL se errore.
 */
HashTable* hash_table_create(int (*f1)(const void*, const void*), unsigned long (*f2)(const void*));

/**
 * @brief Inserisce o aggiorna una coppia chiave-valore nella tavola.
 *
 * Se la chiave esiste, aggiorna il valore; altrimenti inserisce un nuovo elemento.
 * Se il fattore di carico supera la soglia, la tavola viene ridimensionata automaticamente.
 *
 * @param table Puntatore alla tavola hash.
 * @param key Chiave da inserire (non deve essere NULL).
 * @param value Valore associato alla chiave.
 */
void hash_table_put(HashTable* table, const void* key, const void* value);

/**
 * @brief Recupera il valore associato a una chiave.
 *
 * @param table Puntatore alla tavola hash.
 * @param key Chiave da cercare.
 *
 * @return Puntatore al valore se trovato, NULL altrimenti.
 */
void* hash_table_get(const HashTable* table, const void* key);

/**
 * @brief Verifica se una chiave esiste nella tavola.
 *
 * @param table Puntatore alla tavola hash.
 * @param key Chiave da cercare.
 *
 * @return 1 se la chiave e' presente, 0 altrimenti.
 */
int hash_table_contains_key(const HashTable* table, const void* key);

/**
 * @brief Rimuove una coppia chiave-valore dalla tavola.
 *
 * Se la chiave non esiste, non fa nulla.
 *
 * @param table Puntatore alla tavola hash.
 * @param key Chiave da rimuovere.
 */
void hash_table_remove(HashTable* table, const void* key);

/**
 * @brief Ritorna il numero di elementi nella tavola.
 *
 * @param table Puntatore alla tavola hash.
 *
 * @return Numero di elementi, o 0 se table e' NULL.
 */
int hash_table_size(const HashTable* table);

/**
 * @brief Ritorna un array contente tutte le chiavi nella tavola.
 *
 * L'array deve essere liberato dal chiamante. Ogni elemento dell'array
 * e' un puntatore a una chiave memorizzata nella tavola.
 *
 * @param table Puntatore alla tavola hash.
 *
 * @return Array di puntatori alle chiavi, o NULL se tavola vuota.
 */
void** hash_table_keyset(const HashTable* table);

/**
 * @brief Libera tutta la memoria della tavola hash.
 *
 * NOTA: non libera i dati puntati dalle chiavi e valori. E' responsabilita'
 * del chiamante liberare la memoria di key e value se sono stati allocati dinamicamente.
 *
 * @param table Puntatore alla tavola hash.
 */
void hash_table_free(HashTable* table);

#endif
