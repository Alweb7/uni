#ifndef BFS_H
#define BFS_H

#include "graph.h"

/**  @brief Esegue la visita in ampiezza (BFS) su un grafo a partire da un nodo.
 * @param gr Puntatore al grafo.
 * @param start Puntatore al nodo di partenza.
 * @param compare Funzione di confronto per i nodi.
 * @param hash Funzione hash per i nodi.
 * @return Array terminato da NULL contenente i nodi visitati nell'ordine, NULL in caso di errore.
 */
void** breadth_first_visit(Graph gr,void* start,int (*compare)(const void*, const void*),unsigned long (*hash)(const void*));

#endif
