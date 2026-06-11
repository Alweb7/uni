#include "graph.h"

#include "hash_table.h"

#include <stddef.h>
#include <stdlib.h>

/** @brief Struttura che rappresenta un record per un nodo nel grafo.
 * @param adj Tabella hash che mappa nodi adiacenti a questo nodo (archi uscenti).
 * @param in_adj Tabella hash che mappa nodi che puntano a questo nodo (archi entranti, solo per grafi diretti).
 */
typedef struct node_record {
    HashTable* adj;
    HashTable* in_adj;
} NodeRecord;

/** @brief Struttura che rappresenta un grafo.
    *@param labelled 1 se il grafo è etichettato, 0 altrimenti.
    *@param directed 1 se il grafo è diretto, 0 altrimenti.
    *@param compare Funzione di comparazione per le chiavi.
    *@param hash Funzione hash per le chiavi.
    *@param nodes Tabella hash che mappa nodi a NodeRecord.
    *@param num_edges Numero totale di archi nel grafo.
*/
struct graph {
    int labelled;
    int directed;
    int (*compare)(const void*, const void*);
    unsigned long (*hash)(const void*);
    HashTable* nodes;
    int num_edges;
};

/** @brief Crea un nuovo record per un nodo nel grafo.
 * @param compare Funzione di comparazione per le chiavi.
 * @param hash Funzione hash per le chiavi.
 * @return Puntatore al record creato, NULL in caso di errore.
 */
static NodeRecord* node_record_create(int (*compare)(const void*, const void*),unsigned long (*hash)(const void*)) {
    NodeRecord* rec = malloc(sizeof(*rec));
    if (rec == NULL) {
        return NULL;
    }
    rec->adj = ht_create(compare, hash);
    if (rec->adj == NULL) {
        free(rec);
        return NULL;
    }
    rec->in_adj = ht_create(compare, hash);
    if (rec->in_adj == NULL) {
        ht_free(rec->adj, NULL);
        free(rec);
        return NULL;
    }
    return rec;
}

/** @brief Libera la memoria occupata da un record di nodo.
 * @param rec Puntatore al record da liberare.
 */
static void node_record_free(NodeRecord* rec) {
    if (rec == NULL) {
        return;
    }
    ht_free(rec->adj, NULL);
    ht_free(rec->in_adj, NULL);
    free(rec);
}


Graph graph_create(int labelled,int directed,int (*compare)(const void*, const void*),unsigned long (*hash)(const void*)) {
    Graph gr;
    if (compare == NULL || hash == NULL) {
        return NULL;
    }
    gr = malloc(sizeof(*gr));
    if (gr == NULL) {
        return NULL;
    }
    gr->nodes = ht_create(compare, hash);
    if (gr->nodes == NULL) {
        free(gr);
        return NULL;
    }
    gr->labelled = labelled ? 1 : 0;
    gr->directed = directed ? 1 : 0;
    gr->compare = compare;
    gr->hash = hash;
    gr->num_edges = 0;
    return gr;
}

int graph_is_directed(const Graph gr) {
    if (gr == NULL) {
        return 0;
    }
    return gr->directed;
}


int graph_is_labelled(const Graph gr) {
    if (gr == NULL) {
        return 0;
    }
    return gr->labelled;
}

int graph_add_node(Graph gr, const void* node) {
    NodeRecord* rec;

    if (gr == NULL || node == NULL) {
        return 0;
    }
    if (ht_contains(gr->nodes, node)) {
        return 0;
    }
    rec = node_record_create(gr->compare, gr->hash);
    if (rec == NULL) {
        return 0;
    }
    if (ht_put(gr->nodes, node, rec) < 0) {
        node_record_free(rec);
        return 0;
    }
    return 1;
}


int graph_contains_node(const Graph gr, const void* node) {
    if (gr == NULL || node == NULL) {
        return 0;
    }
    return ht_contains(gr->nodes, node);
}


int graph_remove_node(Graph gr, const void* node) {
    NodeRecord* rec;
    void** all_nodes;
    size_t n;
    size_t i;
    if (gr == NULL || node == NULL) {
        return 0;
    }
    rec = ht_get(gr->nodes, node);
    if (rec == NULL) {
        return 0;
    }
    if (gr->directed) {
        size_t out_count = ht_size(rec->adj);
        size_t in_count = ht_size(rec->in_adj);
        int self_loop = ht_contains(rec->adj, node);
        
        all_nodes = ht_keys(rec->adj);
        n = out_count;
        for (i = 0; i < n; ++i) {
            NodeRecord* dest = ht_get(gr->nodes, all_nodes[i]);
            if (dest != NULL) {
                ht_remove(dest->in_adj, node, NULL);
            }
        }
        free(all_nodes);
        all_nodes = ht_keys(rec->in_adj);
        n = ht_size(rec->in_adj);
        for (i = 0; i < n; ++i) {
            NodeRecord* src = ht_get(gr->nodes, all_nodes[i]);
            if (src != NULL) {
                ht_remove(src->adj, node, NULL);
            }
        }
        free(all_nodes);
        gr->num_edges -= (int)(out_count + in_count - (self_loop ? 1U : 0U));
    } else {
        void** neigh = ht_keys(rec->adj);
        size_t deg = ht_size(rec->adj);
        gr->num_edges -= (int)deg;
        for (i = 0; i < deg; ++i) {
            if (gr->compare(neigh[i], node) != 0) {
                NodeRecord* other = ht_get(gr->nodes, neigh[i]);
                if (other != NULL) {
                    ht_remove(other->adj, node, NULL);
                }
            }
        }
        free(neigh);
    }
    ht_remove(gr->nodes, node, (void**)&rec);
    node_record_free(rec);
    return 1;
}


int graph_num_nodes(const Graph gr) {
    if (gr == NULL) {
        return 0;
    }
    return (int)ht_size(gr->nodes);
}


void** graph_get_nodes(const Graph gr) {
    if (gr == NULL) {
        return NULL;
    }
    return ht_keys(gr->nodes);
}


int graph_add_edge(Graph gr, const void* node1, const void* node2, const void* label) {
    NodeRecord* rec1;
    NodeRecord* rec2;
    int already;
    void* old_label;
    if (gr == NULL || node1 == NULL || node2 == NULL) {
        return 0;
    }
    if (gr->labelled && label == NULL) {
        return 0;
    }
    if (!gr->labelled) {
        label = NULL;
    }
    rec1 = ht_get(gr->nodes, node1);
    rec2 = ht_get(gr->nodes, node2);
    if (rec1 == NULL || rec2 == NULL) {
        return 0;
    }
    already = ht_contains(rec1->adj, node2);
    old_label = ht_get(rec1->adj, node2);
    if (ht_put(rec1->adj, node2, (void*)label) < 0) {
        return 0;
    }
    if (gr->directed) {
        if (ht_put(rec2->in_adj, node1, (void*)label) < 0) {
            if (already) {
                ht_put(rec1->adj, node2, old_label);
            } else {
                ht_remove(rec1->adj, node2, NULL);
            }
            return 0;
        }
    } else if (gr->compare(node1, node2) != 0) {
        if (ht_put(rec2->adj, node1, (void*)label) < 0) {
            if (already) {
                ht_put(rec1->adj, node2, old_label);
            } else {
                ht_remove(rec1->adj, node2, NULL);
            }
            return 0;
        }
    }
    if (!already) {
        gr->num_edges += 1;
    }
    return 1;
}

int graph_contains_edge(const Graph gr, const void* node1, const void* node2) {
    NodeRecord* rec1;

    if (gr == NULL || node1 == NULL || node2 == NULL) {
        return 0;
    }
    rec1 = ht_get(gr->nodes, node1);
    if (rec1 == NULL) {
        return 0;
    }
    return ht_contains(rec1->adj, node2);
}


int graph_remove_edge(Graph gr, const void* node1, const void* node2) {
    NodeRecord* rec1;
    NodeRecord* rec2;
    if (gr == NULL || node1 == NULL || node2 == NULL) {
        return 0;
    }
    rec1 = ht_get(gr->nodes, node1);
    rec2 = ht_get(gr->nodes, node2);
    if (rec1 == NULL || rec2 == NULL) {
        return 0;
    }
    if (!ht_remove(rec1->adj, node2, NULL)) {
        return 0;
    }
    if (gr->directed) {
        ht_remove(rec2->in_adj, node1, NULL);
    } else if (gr->compare(node1, node2) != 0) {
        ht_remove(rec2->adj, node1, NULL);
    }
    gr->num_edges -= 1;
    return 1;
}


int graph_num_edges(const Graph gr) {
    if (gr == NULL) {
        return 0;
    }
    return gr->num_edges;
}


Edge** graph_get_edges(const Graph gr) {
    Edge** edges;
    size_t total;
    size_t pos;
    void** nodes;
    size_t n;
    size_t i;
    if (gr == NULL) {
        return NULL;
    }
    total = (size_t)gr->num_edges;
    edges = calloc(total, sizeof(Edge*));
    if (edges == NULL && total > 0) {
        return NULL;
    }
    nodes = ht_keys(gr->nodes);
    n = ht_size(gr->nodes);
    pos = 0;
    for (i = 0; i < n; ++i) {
        NodeRecord* rec = ht_get(gr->nodes, nodes[i]);
        void** neigh;
        size_t d;
        size_t j;
        if (rec == NULL) {
            continue;
        }
        neigh = ht_keys(rec->adj);
        d = ht_size(rec->adj);
        for (j = 0; j < d; ++j) {
            if (gr->directed || gr->compare(nodes[i], neigh[j]) <= 0) {
                Edge* e;
                if (pos >= total) {
                    continue;
                }
                e = malloc(sizeof(*e));
                if (e == NULL) {
                    continue;
                }
                e->source = nodes[i];
                e->dest = neigh[j];
                e->label = ht_get(rec->adj, neigh[j]);
                edges[pos++] = e;
            }
        }
        free(neigh);
    }
    free(nodes);
    return edges;
}


void* graph_get_label(const Graph gr, const void* node1, const void* node2) {
    NodeRecord* rec1;
    if (gr == NULL || node1 == NULL || node2 == NULL) {
        return NULL;
    }
    rec1 = ht_get(gr->nodes, node1);
    if (rec1 == NULL) {
        return NULL;
    }
    return ht_get(rec1->adj, node2);
}


void** graph_get_neighbours(const Graph gr, const void* node) {
    NodeRecord* rec;
    if (gr == NULL || node == NULL) {
        return NULL;
    }
    rec = ht_get(gr->nodes, node);
    if (rec == NULL) {
        return NULL;
    }
    return ht_keys(rec->adj);
}


int graph_num_neighbours(const Graph gr, const void* node) {
    NodeRecord* rec;
    if (gr == NULL || node == NULL) {
        return 0;
    }
    rec = ht_get(gr->nodes, node);
    if (rec == NULL) {
        return 0;
    }
    return (int)ht_size(rec->adj);
}


void graph_free(Graph gr) {
    void** nodes;
    size_t n;
    size_t i;
    if (gr == NULL) {
        return;
    }
    nodes = ht_keys(gr->nodes);
    n = ht_size(gr->nodes);
    for (i = 0; i < n; ++i) {
        NodeRecord* rec = ht_get(gr->nodes, nodes[i]);
        node_record_free(rec);
    }
    free(nodes);
    ht_free(gr->nodes, NULL);
    free(gr);
}
