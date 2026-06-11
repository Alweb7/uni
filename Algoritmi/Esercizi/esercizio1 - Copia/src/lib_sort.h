#ifndef LIB_SORT_H
#define LIB_SORT_H
#include <stddef.h>

/**
 * @brief Ordina un array utilizzando l'algoritmo Selection Sort.
 * @param base Puntatore al primo elemento dell'array.
 * @param num_items Numero di elementi nell'array.
 * @param size Dimensione in byte di ogni elemento.
 * @param compar Funzione di comparazione.
 */

void selectionsort(void *base, size_t num_items, size_t size, int (*compar)(const void *, const void *));

/**
 * @brief Ordina un array utilizzando l'algoritmo Quick Sort.
 * @param base Puntatore al primo elemento dell'array.
 * @param num_items Numero di elementi nell'array.
 * @param size Dimensione in byte di ogni elemento.
 * @param compar Funzione di comparazione.
 */
void quicksort(void *base, size_t num_items, size_t size, int (*compar)(const void *, const void *));

/**
 * @brief Algoritmo di ordinamento ibrido (Quick Sort + Selection Sort).
 * @param base Puntatore al primo elemento dell'array.
 * @param num_items Numero di elementi nell'array.
 * @param size Dimensione in byte di ogni elemento.
 * @param k Valore soglia sotto il quale viene usato il Selection Sort.
 * @param compar Funzione di comparazione.
 */
void hybridsort(void *base, size_t num_items, size_t size, size_t k, int (*compar)(const void *, const void *));

#endif // LIB_SORT_H
