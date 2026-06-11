
#ifndef HASH_H
#define HASH_H

/**
 * @brief Tabella hash con indirizzamento aperto usata come supporto alla priority queue.
 */
typedef struct Hash Hash;

/**
 * @brief Crea una nuova tabella hash.
 *
 * @param hash Funzione hash per le chiavi.
 * @param capacity Capacita' iniziale desiderata.
 * @return Puntatore alla tabella creata, oppure `NULL` in caso di errore.
 */
Hash* hash_create(unsigned long (*hash)(const void*), int capacity);

/**
 * @brief Inserisce o aggiorna una coppia chiave-valore.
 *
 * @param map Tabella su cui operare.
 * @param key Chiave da memorizzare.
 * @param value Valore associato alla chiave.
 * @return `0` se l'operazione ha successo, `-1` in caso di errore.
 */
int hash_put(Hash*, const void* key, int value);

/**
 * @brief Cerca il valore associato a una chiave.
 *
 * @param map Tabella da interrogare.
 * @param key Chiave da cercare.
 * @return Valore associato, oppure `-1` se la chiave non e' presente.
 */
int hash_get(const Hash*, const void* key);

/**
 * @brief Rimuove una chiave dalla tabella.
 *
 * @param map Tabella su cui operare.
 * @param key Chiave da rimuovere.
 * @return `0` se la rimozione ha successo, `-1` in caso di errore o chiave assente.
 */
int hash_remove(Hash*, const void* key);

/**
 * @brief Verifica se una chiave e' presente nella tabella.
 *
 * @param map Tabella da interrogare.
 * @param key Chiave da cercare.
 * @return `1` se la chiave e' presente, `0` altrimenti.
 */
int hash_contains(const Hash*, const void* key);

/**
 * @brief Libera la memoria associata alla tabella hash.
 *
 * @param map Tabella da distruggere.
 */
void hash_free(Hash*);
#endif
