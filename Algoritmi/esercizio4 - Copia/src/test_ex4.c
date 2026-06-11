#include "bfs.h"
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_failed = 0;

/** @brief Macro di test semplice per contare le asserzioni.
 * @param expr Espressione booleana da testare.
 */
#define ASSERT_TRUE(expr) \
    do { \
        ++tests_run; \
        if (!(expr)) { \
            ++tests_failed; \
            fprintf(stderr, "ASSERT FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
        } \
    } while (0)

/** @brief Comparatore di stringhe usato nei test.
 * @param a Puntatore alla prima stringa.
 * @param b Puntatore alla seconda stringa.
 * @return 0 se le stringhe sono uguali, un valore negativo se a < b, un valore positivo se a > b.
 */
static int str_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

/** @brief Hash DJB2 per stringhe usato nei test.
 * @param p Puntatore alla stringa.
 * @return Il valore hash della stringa.
 */
static unsigned long str_hash(const void* p) {
    const unsigned char* s = (const unsigned char*)p;
    unsigned long h = 5381;
    int c;

    while ((c = *s++) != 0) {
        h = ((h << 5) + h) + (unsigned long)c;
    }

    return h;
}

/** @brief Controlla se un puntatore è presente in un array di puntatori.
 * @param arr Array di puntatori.
 * @param n Numero di elementi nell'array.
 * @param ptr Puntatore da cercare.
 * @return 1 se il puntatore è presente, 0 altrimenti.
 */
static int contains_ptr(void** arr, int n, const void* ptr) {
    int i;
    for (i = 0; i < n; ++i) {
        if (arr[i] == ptr) {
            return 1;
        }
    }
    return 0;
}

/** @brief Calcola la lunghezza di un array terminato da NULL.
 * @param arr Array di puntatori.
 * @return Il numero di elementi nell'array.
 */
static int nullterm_len(void** arr) {
    int len = 0;
    while (arr[len] != NULL) {
        ++len;
    }
    return len;
}

/** @brief Testa il comportamento di un grafo diretto ed etichettato.
 * @param g Puntatore al grafo da testare.
 */
static void test_graph_directed_labelled(void) {
    Graph g = graph_create(1, 1, str_compare, str_hash);
    double w1 = 3.5;
    double w2 = 7.0;

    ASSERT_TRUE(g != NULL);
    ASSERT_TRUE(graph_is_directed(g) == 1);
    ASSERT_TRUE(graph_is_labelled(g) == 1);

    ASSERT_TRUE(graph_add_node(g, "A") == 1);
    ASSERT_TRUE(graph_add_node(g, "B") == 1);
    ASSERT_TRUE(graph_add_node(g, "A") == 0);

    ASSERT_TRUE(graph_add_edge(g, "A", "B", &w1) == 1);
    ASSERT_TRUE(graph_contains_edge(g, "A", "B") == 1);
    ASSERT_TRUE(graph_contains_edge(g, "B", "A") == 0);
    ASSERT_TRUE(graph_num_edges(g) == 1);
    ASSERT_TRUE(graph_get_label(g, "A", "B") == &w1);
    ASSERT_TRUE(graph_add_edge(g, "B", "A", NULL) == 0);

    ASSERT_TRUE(graph_add_edge(g, "A", "B", &w2) == 1);
    ASSERT_TRUE(graph_num_edges(g) == 1);
    ASSERT_TRUE(graph_get_label(g, "A", "B") == &w2);

    ASSERT_TRUE(graph_remove_edge(g, "A", "B") == 1);
    ASSERT_TRUE(graph_num_edges(g) == 0);

    graph_free(g);
}

/** @brief Testa un grafo non diretto e la rimozione di un nodo.
 * @param g Puntatore al grafo da testare.
 */
static void test_graph_undirected(void) {
    Graph g = graph_create(0, 0, str_compare, str_hash);

    ASSERT_TRUE(g != NULL);
    ASSERT_TRUE(graph_add_node(g, "A") == 1);
    ASSERT_TRUE(graph_add_node(g, "B") == 1);
    ASSERT_TRUE(graph_add_node(g, "C") == 1);

    ASSERT_TRUE(graph_add_edge(g, "A", "B", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "B", "C", NULL) == 1);
    ASSERT_TRUE(graph_num_edges(g) == 2);

    ASSERT_TRUE(graph_contains_edge(g, "A", "B") == 1);
    ASSERT_TRUE(graph_contains_edge(g, "B", "A") == 1);
    ASSERT_TRUE(graph_num_neighbours(g, "B") == 2);

    ASSERT_TRUE(graph_remove_node(g, "B") == 1);
    ASSERT_TRUE(graph_contains_node(g, "B") == 0);
    ASSERT_TRUE(graph_num_edges(g) == 0);

    graph_free(g);
}

/** @brief Verifica la rimozione di un nodo in un grafo diretto con archi entranti e uscenti.
 * @param g Puntatore al grafo da testare.
 */
static void test_graph_directed_remove_node(void) {
    Graph g = graph_create(0, 1, str_compare, str_hash);

    ASSERT_TRUE(g != NULL);
    ASSERT_TRUE(graph_add_node(g, "A") == 1);
    ASSERT_TRUE(graph_add_node(g, "B") == 1);
    ASSERT_TRUE(graph_add_node(g, "C") == 1);

    ASSERT_TRUE(graph_add_edge(g, "A", "B", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "C", "A", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "A", "A", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "B", "C", NULL) == 1);
    ASSERT_TRUE(graph_num_edges(g) == 4);

    ASSERT_TRUE(graph_remove_node(g, "A") == 1);
    ASSERT_TRUE(graph_contains_node(g, "A") == 0);
    ASSERT_TRUE(graph_contains_edge(g, "A", "B") == 0);
    ASSERT_TRUE(graph_contains_edge(g, "C", "A") == 0);
    ASSERT_TRUE(graph_contains_edge(g, "A", "A") == 0);
    ASSERT_TRUE(graph_contains_edge(g, "B", "C") == 1);
    ASSERT_TRUE(graph_num_edges(g) == 1);

    graph_free(g);
}

/** @brief Controlla che la lista di nodi e di archi venga restituita correttamente.
 * @param g Puntatore al grafo da testare.
 */
static void test_get_nodes_edges(void) {
    Graph g = graph_create(1, 0, str_compare, str_hash);
    double w1 = 1.0;
    double w2 = 2.0;
    void** nodes;
    Edge** edges;

    ASSERT_TRUE(graph_add_node(g, "A") == 1);
    ASSERT_TRUE(graph_add_node(g, "B") == 1);
    ASSERT_TRUE(graph_add_node(g, "C") == 1);
    ASSERT_TRUE(graph_add_edge(g, "A", "B", &w1) == 1);
    ASSERT_TRUE(graph_add_edge(g, "A", "C", &w2) == 1);

    nodes = graph_get_nodes(g);
    ASSERT_TRUE(nodes != NULL);
    ASSERT_TRUE(graph_num_nodes(g) == 3);
    free(nodes);

    edges = graph_get_edges(g);
    ASSERT_TRUE(edges != NULL);
    ASSERT_TRUE(graph_num_edges(g) == 2);
    ASSERT_TRUE(edges[0] != NULL);
    ASSERT_TRUE(edges[1] != NULL);
    free(edges[0]);
    free(edges[1]);
    free(edges);

    graph_free(g);
}

/** @brief Verifica la visita in ampiezza su un grafo semplice.
 * @param g Puntatore al grafo da testare.
 */
static void test_bfs(void) {
    Graph g = graph_create(0, 0, str_compare, str_hash);
    void** order;
    int n;

    ASSERT_TRUE(graph_add_node(g, "A") == 1);
    ASSERT_TRUE(graph_add_node(g, "B") == 1);
    ASSERT_TRUE(graph_add_node(g, "C") == 1);
    ASSERT_TRUE(graph_add_node(g, "D") == 1);
    ASSERT_TRUE(graph_add_node(g, "X") == 1);

    ASSERT_TRUE(graph_add_edge(g, "A", "B", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "A", "C", NULL) == 1);
    ASSERT_TRUE(graph_add_edge(g, "B", "D", NULL) == 1);

    order = breadth_first_visit(g, "A", str_compare, str_hash);
    ASSERT_TRUE(order != NULL);

    n = nullterm_len(order);
    ASSERT_TRUE(n == 4);
    ASSERT_TRUE(order[0] == (void*)"A");
    ASSERT_TRUE(contains_ptr(order, n, "B") == 1);
    ASSERT_TRUE(contains_ptr(order, n, "C") == 1);
    ASSERT_TRUE(contains_ptr(order, n, "D") == 1);
    ASSERT_TRUE(contains_ptr(order, n, "X") == 0);
    free(order);

    ASSERT_TRUE(breadth_first_visit(g, "Z", str_compare, str_hash) == NULL);

    graph_free(g);
}

int main(void) {
    test_graph_directed_labelled();
    test_graph_undirected();
    test_graph_directed_remove_node();
    test_get_nodes_edges();
    test_bfs();

    if (tests_failed == 0) {
        printf("All tests passed (%d assertions).\n", tests_run);
        return 0;
    }

    printf("Tests failed: %d/%d assertions failed.\n", tests_failed, tests_run);
    return 1;
}
