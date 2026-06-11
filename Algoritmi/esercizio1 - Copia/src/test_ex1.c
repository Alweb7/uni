#include "lib_sort.h"
#include "record.h"
#include "record_cmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_selectionsort_int() {
    int arr[5] = {5, 3, 4, 1, 2};
    selectionsort(arr, 5, sizeof(int), (int (*)(const void *, const void *))strcmp);
    for (int i = 1; i < 5; ++i) assert(arr[i-1] <= arr[i]);
}

void test_selectionsort_record() {
    Record arr[3] = {
        {2, 1.0f, 3, "b"},
        {1, 2.0f, 2, "a"},
        {3, 0.5f, 1, "c"}
    };
    selectionsort(arr, 3, sizeof(Record), cmp_record_id);
    assert(arr[0].id == 1 && arr[1].id == 2 && arr[2].id == 3);
}

void test_quicksort_record() {
    Record arr[3] = {
        {2, 1.0f, 3, "b"},
        {1, 2.0f, 2, "a"},
        {3, 0.5f, 1, "c"}
    };
    quicksort(arr, 3, sizeof(Record), cmp_record_id);
    assert(arr[0].id == 1 && arr[1].id == 2 && arr[2].id == 3);
}

void test_hybridsort_threshold() {
    Record arr[4] = {
        {4, 0, 0, ""}, {3, 0, 0, ""}, {2, 0, 0, ""}, {1, 0, 0, ""}
    };
    hybridsort(arr, 4, sizeof(Record), 5, cmp_record_id); // selectionsort only
    for (int i = 1; i < 4; ++i) assert(arr[i-1].id < arr[i].id);
    Record arr2[4] = {
        {4, 0, 0, ""}, {3, 0, 0, ""}, {2, 0, 0, ""}, {1, 0, 0, ""}
    };
    hybridsort(arr2, 4, sizeof(Record), 0, cmp_record_id); // quicksort only
    for (int i = 1; i < 4; ++i) assert(arr2[i-1].id < arr2[i].id);
}

void test_empty_array() {
    Record arr[1];
    hybridsort(arr, 0, sizeof(Record), 2, cmp_record_id);
}

void test_duplicates() {
    Record arr[4] = {
        {2, 0, 0, ""}, {1, 0, 0, ""}, {2, 0, 0, ""}, {1, 0, 0, ""}
    };
    hybridsort(arr, 4, sizeof(Record), 2, cmp_record_id);
    assert(arr[0].id <= arr[1].id && arr[1].id <= arr[2].id && arr[2].id <= arr[3].id);
}

int main() {
    test_selectionsort_int();
    test_selectionsort_record();
    test_quicksort_record();
    test_hybridsort_threshold();
    test_empty_array();
    test_duplicates();
    printf("Tutti i test superati!\n");
    return 0;
}
