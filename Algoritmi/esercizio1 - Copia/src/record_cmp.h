#ifndef RECORD_CMP_H
#define RECORD_CMP_H

#include "record.h"

/**
 * @brief Funzioni di comparazione per i diversi campi di Record.
 *
 * Le funzioni di comparazione seguono la convenzione standard:
 * - valore negativo se il primo elemento è minore del secondo
 * - zero se i due elementi sono equivalenti
 * - valore positivo se il primo elemento è maggiore del secondo
 *
 * Sono compatibili con funzioni di ordinamento generiche
 * come qsort, quick_sort e hybrid_sort.
 */

/**
 * @brief Confronta due record in base al campo `id`.
 *
 * @param a Puntatore al primo record.
 * @param b Puntatore al secondo record.
 * @return Valore negativo, zero o positivo in base al confronto.
 */
int cmp_record_id(const void *a, const void *b);
/**
 * @brief Confronta due record in base al campo `field1`.
 *
 * @param a Puntatore al primo record.
 * @param b Puntatore al secondo record.
 * @return Valore negativo, zero o positivo in base al confronto.
 */
int cmp_record_field1(const void *a, const void *b);
/**
 * @brief Confronta due record in base al campo `field2`.
 *
 * @param a Puntatore al primo record.
 * @param b Puntatore al secondo record.
 * @return Valore negativo, zero o positivo in base al confronto.
 */
int cmp_record_field2(const void *a, const void *b);
/**
 * @brief Confronta due record in base al campo `field3`.
 *
 * @param a Puntatore al primo record.
 * @param b Puntatore al secondo record.
 * @return Valore negativo, zero o positivo in base al confronto.
 */
int cmp_record_field3(const void *a, const void *b);

#endif // RECORD_CMP_H
