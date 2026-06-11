#ifndef RECORD_H
#define RECORD_H
#include <stdint.h>
#include <stdio.h>
#define FIELD3_LEN 16

/**
 * @brief Struttura che rappresenta un record di dati.
 *
 * Ogni record contiene:
 * - un identificatore univoco (`id`)
 * - un valore reale (`field1`)
 * - un valore intero (`field2`)
 * - una stringa di lunghezza massima FIELD3_MAX_LEN
 *
 * La struttura Ã¨ pensata per essere letta e scritta
 * direttamente su file binari.
 */
typedef struct {
  uint64_t id;
  float field1;
  int64_t field2;
  char field3[FIELD3_LEN];
} Record;

/**
 * @brief Legge un record da un file binario.
 *
 * La funzione legge dal file un singolo record e lo memorizza
 * nella struttura puntata da `record`.
 *
 * @param file Puntatore al file da cui leggere.
 * @param record Puntatore alla struttura Record da riempire.
 * @return 1 se la lettura ha successo, 0 in caso di fine file o errore.
 */
int read_record(FILE *file, Record *record);
/**
 * @brief Scrive un record su un file binario.
 *
 * La funzione scrive nel file il record puntato da `record`.
 *
 * @param file Puntatore al file su cui scrivere.
 * @param record Puntatore alla struttura Record da scrivere.
 * @return 1 se la scrittura ha successo, 0 in caso di errore.
 */
int write_record(FILE *file, const Record *record);
/**
 * @brief Carica tutti i record presenti in un file.
 *
 * Legge sequenzialmente tutti i record dal file e alloca dinamicamente
 * un array di Record contenente i dati letti.
 *
 * @param file Puntatore al file da cui leggere.
 * @param records Puntatore a un array di Record allocato dinamicamente.
 * @param count Numero di record letti.
 * @return 1 se il caricamento ha successo, 0 in caso di errore.
 */
int load_records_from_file(FILE *file, Record **records, size_t *count);
/**
 * @brief Salva un array di record su un file.
 *
 * Scrive sequenzialmente tutti i record dell'array nel file.
 *
 * @param file Puntatore al file su cui scrivere.
 * @param records Puntatore all'array di Record da scrivere.
 * @param count Numero di record da scrivere.
 * @return 1 se il salvataggio ha successo, 0 in caso di errore.
 */
int save_records_to_file(FILE *file, const Record *records, size_t count);
/**
 * @brief Stampa un record su standard output.
 *
 * La funzione stampa i campi del record in un formato leggibile.
 *
 * @param record Puntatore al record da stampare.
 */
void print_record(const Record *record);

#endif // RECORD_H