#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "pq.h"

/**
 * @file test.c
 * @brief Unit test essenziali per la priority queue basata su heap ternario.
 */

static int compare_ints(const void* a, const void* b) {
    return *(const int*)a - *(const int*)b;
}

static unsigned long hash_ptr(const void* a) {
    return (unsigned long)(uintptr_t)a;
}

static void test_push_top_pop(void) {
    PriorityQueue* pq = priority_queue_create(compare_ints, hash_ptr);
    int a = 1;
    int b = 5;
    int c = 3;

    assert(pq != NULL);
    assert(priority_queue_size(pq) == 0);
    assert(priority_queue_push(pq, &a) == 0);
    assert(priority_queue_push(pq, &b) == 0);
    assert(priority_queue_push(pq, &c) == 0);
    assert(priority_queue_size(pq) == 3);
    assert(*(int*)priority_queue_top(pq) == 5);

    priority_queue_pop(pq);
    assert(priority_queue_size(pq) == 2);
    assert(*(int*)priority_queue_top(pq) == 3);

    priority_queue_pop(pq);
    assert(*(int*)priority_queue_top(pq) == 1);

    priority_queue_pop(pq);
    assert(priority_queue_top(pq) == NULL);
    priority_queue_free(pq);
}

static void test_contains_and_remove(void) {
    PriorityQueue* pq = priority_queue_create(compare_ints, hash_ptr);
    int values[] = {4, 9, 2, 7, 5, 8, 1};

    assert(pq != NULL);
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        assert(priority_queue_push(pq, &values[i]) == 0);
    }

    assert(priority_queue_contains(pq, &values[1]) == 1);
    assert(priority_queue_contains(pq, &values[3]) == 1);
    assert(priority_queue_remove(pq, &values[1]) == 0);
    assert(priority_queue_contains(pq, &values[1]) == 0);
    assert(*(int*)priority_queue_top(pq) == 8);

    assert(priority_queue_remove(pq, &values[5]) == 0);
    assert(*(int*)priority_queue_top(pq) == 7);
    assert(priority_queue_remove(pq, &values[0]) == 0);
    assert(priority_queue_size(pq) == 4);

    priority_queue_free(pq);
}

/* Verifica la gestione degli input non validi e dei duplicati di puntatore. */
static void test_invalid_arguments(void) {
    int value = 10;
    PriorityQueue* pq = priority_queue_create(compare_ints, hash_ptr);

    assert(priority_queue_create(NULL, hash_ptr) == NULL);
    assert(priority_queue_create(compare_ints, NULL) == NULL);
    assert(priority_queue_push(NULL, &value) == -1);
    assert(priority_queue_push(pq, NULL) == -1);
    assert(priority_queue_contains(NULL, &value) == -1);
    assert(priority_queue_contains(pq, NULL) == -1);
    assert(priority_queue_remove(NULL, &value) == -1);
    assert(priority_queue_remove(pq, NULL) == -1);
    assert(priority_queue_size(NULL) == -1);
    assert(priority_queue_push(pq, &value) == 0);
    assert(priority_queue_push(pq, &value) == -1);

    priority_queue_free(pq);
}

/* Esercita ridimensionamento e mantenimento dell'ordine su input grandi. */
static void test_many_elements(void) {
    enum { COUNT = 5000 };
    int values[COUNT];
    PriorityQueue* pq = priority_queue_create(compare_ints, hash_ptr);

    assert(pq != NULL);
    for (int i = 0; i < COUNT; i++) {
        values[i] = i;
        assert(priority_queue_push(pq, &values[i]) == 0);
    }

    for (int expected = COUNT - 1; expected >= 0; expected--) {
        assert(*(int*)priority_queue_top(pq) == expected);
        priority_queue_pop(pq);
    }

    assert(priority_queue_size(pq) == 0);
    priority_queue_free(pq);
}

int main(void) {
    test_push_top_pop();
    test_contains_and_remove();
    test_invalid_arguments();
    test_many_elements();
    puts("All tests passed.");
    return 0;
}
