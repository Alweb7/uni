#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>

/**  @brief HashTable è una tabella hash che mappa chiavi a valori.
 * La tabella utilizza funzioni di comparazione e hash fornite dall'utente.
 * Le chiavi e i valori sono puntatori generici (void*).
 */
typedef struct _HashTable HashTable;

/**  @brief Crea una tabella hash con funzioni di comparazione e hash. 
 * @param compare Funzione di comparazione per le chiavi. Deve restituire 0 se le chiavi sono uguali.
 * @param hash Funzione hash per le chiavi.
 * @return Puntatore alla tabella hash creata, NULL in caso di errore di allocazione o se i callback sono NULL.
*/
HashTable* ht_create(int (*compare)(const void*, const void*), unsigned long (*hash)(const void*));


/**  @brief Inserisce o aggiorna un valore associato a una chiave. 
 * @param table Tabella hash in cui inserire.
 * @param key Chiave da inserire o aggiornare.
 * @param value Valore da associare alla chiave.
 * @return 1 se l'inserimento o aggiornamento è avvenuto con successo, 0 in caso di errore di allocazione o se la tabella è NULL.
*/
int ht_put(HashTable* table, const void* key, const void* value);


/**  @brief Recupera il valore associato a una chiave. 
 * @param table Tabella hash da cui recuperare il valore.
 * @param key Chiave per cui recuperare il valore.
 * @return Puntatore al valore associato alla chiave, NULL se la chiave non è presente.
*/
void* ht_get(const HashTable* table, const void* key);


/**  @brief Restituisce 1 se la chiave è presente nella tabella, 0 altrimenti. 
 * @param table Tabella hash in cui cercare la chiave.
 * @param key Chiave da cercare.
 * @return 1 se la chiave è presente, 0 altrimenti.
*/
int ht_contains(const HashTable* table, const void* key);


/**  @brief Rimuove la chiave e opzionalmente restituisce il valore precedente.
 * @param table Tabella hash da cui rimuovere la chiave.
 * @param key Chiave da rimuovere.
 * @param old_value Se non NULL, viene impostato al valore associato alla chiave
 * @return 1 se la chiave è stata rimossa, 0 se la chiave non è presente o se la tabella è NULL.
*/
int ht_remove(HashTable* table, const void* key, void** old_value);


/**  @brief Restituisce il numero di elementi presenti nella tabella. 
 * @param table Tabella hash di cui contare gli elementi.
 * @return Numero di elementi presenti nella tabella, 0 se la tabella è NULL
*/
size_t ht_size(const HashTable* table);


/**  @brief Restituisce un array di chiavi allocate dinamicamente. 
 * @param table Tabella hash da cui recuperare le chiavi.
 * @return Array di chiavi allocate dinamicamente, NULL se la tabella è NULL.
 */
void** ht_keys(const HashTable* table);


/**  @brief Libera la tabella hash e i nodi; free_value viene chiamato su ogni valore. 
 * @param table Tabella hash da liberare.
 * @param free_value Funzione da chiamare su ogni valore prima di liberare la tabella. Se NULL, i valori non vengono liberati.
*/
void ht_free(HashTable* table, void (*free_value)(void*));

#endif
