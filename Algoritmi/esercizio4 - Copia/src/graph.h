#ifndef GRAPH_H
#define GRAPH_H

/** @brief Grafo è un contenitore opaco che memorizza nodi e archi.
 * L'implementazione supporta grafi diretti o non diretti,
 * e opzionalmente archi etichettati.
 * Le identità dei nodi sono confrontate e hashate utilizzando callback fornite dall'utente.
 */

/** @brief Tipo di dato per rappresentare un grafo.
  */
typedef struct graph *Graph;

/** @brief Struttura che rappresenta un arco nel grafo.
 *@param source Nodo di partenza dell'arco.
 *@param dest Nodo di arrivo dell'arco.
 *@param label Etichetta associata all'arco, NULL se il grafo non è etichettato o se l'arco non ha etichetta.
 */
typedef struct edge {
    void* source;
    void* dest;
    void* label;
} Edge;

/**  @brief Crea un nuovo grafo vuoto.
 * @param labelled 1 se il grafo è etichettato, 0 altrimenti.
 * @param directed 1 se il grafo è diretto, 0 altrimenti.
 * @param compare Funzione di comparazione per le chiavi.
 * @param hash Funzione hash per le chiavi.
 * @return Puntatore al grafo creato, NULL in caso di errore.
 */
Graph graph_create(int labelled,int directed,int (*compare)(const void*, const void*),unsigned long (*hash)(const void*));

/**  @brief Restituisce 1 se il grafo è diretto, 0 altrimenti.
 * @param gr Puntatore al grafo.
 * @return 1 se il grafo è diretto, 0 altrimenti.
 */
int graph_is_directed(const Graph gr);


/**  @brief Restituisce 1 se il grafo è etichettato, 0 altrimenti.
 * @param gr Puntatore al grafo.
 * @return 1 se il grafo è etichettato, 0 altrimenti.
 */
int graph_is_labelled(const Graph gr);

/**  @brief Aggiunge un nodo al grafo.
 * @param gr Puntatore al grafo.
 * @param node Puntatore al nodo da aggiungere.
 * @return 1 se il nodo è stato aggiunto, 0 altrimenti.
 */
int graph_add_node(Graph gr, const void* node);


/**  @brief Controlla l'esistenza di un nodo nel grafo.
 * @param gr Puntatore al grafo.
 * @param node Puntatore al nodo da cercare.
 * @return 1 se il nodo è presente, 0 altrimenti.
 */
int graph_contains_node(const Graph gr, const void* node);

/**  @brief Rimuove un nodo e tutti gli archi incidenti.
 * @param gr Puntatore al grafo.
 * @param node Puntatore al nodo da rimuovere.
 * @return 1 se il nodo è stato rimosso, 0 altrimenti.
 */
int graph_remove_node(Graph gr, const void* node);

/**  @brief Restituisce il numero di nodi nel grafo.
 * @param gr Puntatore al grafo.
 * @return Numero di nodi nel grafo, 0 se il grafo è NULL.
 */
int graph_num_nodes(const Graph gr);

/**  @brief Restituisce un array dinamico di nodi nel grafo.
 * @param gr Puntatore al grafo.
 * @return Array di nodi allocato dinamicamente, NULL se il grafo è NULL.
 */
void** graph_get_nodes(const Graph gr);

/**  @brief Aggiunge un arco tra due nodi esistenti.
 * @param gr Puntatore al grafo.
 * @param node1 Puntatore al nodo di partenza.
 * @param node2 Puntatore al nodo di arrivo.
 * @param label Puntatore all'etichetta dell'arco, NULL se non etichettato.
 * @return 1 se l'arco è stato aggiunto, 0 altrimenti.
 */
int graph_add_edge(Graph gr, const void* node1, const void* node2, const void* label);

/**  @brief Controlla l'esistenza di un arco tra due nodi.
 * @param gr Puntatore al grafo.
 * @param node1 Puntatore al nodo di partenza.
 * @param node2 Puntatore al nodo di arrivo.
 * @return 1 se l'arco è presente, 0 altrimenti.
 */
int graph_contains_edge(const Graph gr, const void* node1, const void* node2);

/**  @brief Rimuove un arco tra due nodi.
 * @param gr Puntatore al grafo.
 * @param node1 Puntatore al nodo di partenza.
 * @param node2 Puntatore al nodo di arrivo.
 * @return 1 se l'arco è stato rimosso, 0 altrimenti.
 */
int graph_remove_edge(Graph gr, const void* node1, const void* node2);

/**  @brief Restituisce il numero totale di archi nel grafo.
 * @param gr Puntatore al grafo.
 * @return Numero di archi nel grafo, 0 se il grafo è NULL.
 */
int graph_num_edges(const Graph gr);

/**  @brief Restituisce un array dinamico di archi allocati singolarmente.
 * @param gr Puntatore al grafo.
 * @return Array di archi allocato dinamicamente, NULL se il grafo è NULL.
 */
Edge** graph_get_edges(const Graph gr);

/**  @brief Ritorna l'etichetta dell'arco tra due nodi, oppure NULL.
 * @param gr Puntatore al grafo.
 * @param node1 Puntatore al nodo di partenza.
 * @param node2 Puntatore al nodo di arrivo.
 * @return Puntatore all'etichetta dell'arco, NULL se l'arco non è presente o non è etichettato.
 */
void* graph_get_label(const Graph gr, const void* node1, const void* node2);

/**  @brief Restituisce la lista dei vicini diretti di un nodo.
 * @param gr Puntatore al grafo.
 * @param node Puntatore al nodo per cui ottenere i vicini.
 * @return Array di puntatori ai vicini diretti, NULL se il nodo non è presente o il grafo è NULL.
 */
void** graph_get_neighbours(const Graph gr, const void* node);

/**  @brief Restituisce il numero di vicini diretti di un nodo.
 * @param gr Puntatore al grafo.
 * @param node Puntatore al nodo per cui contare i vicini.
 * @return Numero di vicini diretti, 0 se il nodo non è presente o il grafo è NULL.
 */
int graph_num_neighbours(const Graph gr, const void* node);

/**  @brief Libera l'intero grafo e tutte le strutture di supporto.
 * @param gr Puntatore al grafo.
 */
void graph_free(Graph gr);

#endif
