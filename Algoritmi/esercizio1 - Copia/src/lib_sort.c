#include "lib_sort.h"
#include <string.h>
#include <stdlib.h>

static void swap_fast(void *a, void *b, size_t size) {
    if (a == b) return;
    unsigned char temp[512]; 

    if (size <= 512) {
        memcpy(temp, a, size);
        memcpy(a, b, size);
        memcpy(b, temp, size);
    } else {
    
        unsigned char *pa = a, *pb = b, t;
        for (size_t i = 0; i < size; i++) {
            t = pa[i]; pa[i] = pb[i]; pb[i] = t;
        }
    }
}


void selectionsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    unsigned char *arr = (unsigned char *)base;
    for (size_t i = 0; i < nitems - 1; ++i) {
        size_t min_idx = i;
        for (size_t j = i + 1; j < nitems; ++j) {
            if (compar(arr + j * size, arr + min_idx * size) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap_fast(arr + i * size, arr + min_idx * size, size);
        }
    }
}

static size_t median_of_three(unsigned char *arr, size_t a, size_t b, size_t c, size_t size, int (*compar)(const void *, const void *)) {
    if (compar(arr + a * size, arr + b * size) < 0) {
        if (compar(arr + b * size, arr + c * size) < 0) return b;
        if (compar(arr + a * size, arr + c * size) < 0) return c;
        return a;
    } else {
        if (compar(arr + a * size, arr + c * size) < 0) return a;
        if (compar(arr + b * size, arr + c * size) < 0) return c;
        return b;
    }
}


static void quicksort_rec(unsigned char *arr, long left, long right, size_t size, size_t k, int (*compar)(const void *, const void *)) {
    while (left < right) {
        size_t n = right - left + 1;
        if (n < k) {
            selectionsort(arr + left * size, n, size, compar);
            return;
        }

        
        size_t mid = left + (right - left) / 2;
        size_t pivot_idx = median_of_three(arr, left, mid, right, size, compar);
        
        
        unsigned char pivot_val[512];
        memcpy(pivot_val, arr + pivot_idx * size, size);

        long i = left - 1;
        long j = right + 1;

        while (1) {
            do { i++; } while (compar(arr + i * size, pivot_val) < 0);
            do { j--; } while (compar(arr + j * size, pivot_val) > 0);
            if (i >= j) break;
            swap_fast(arr + i * size, arr + j * size, size);
        }

        
        if (j - left < right - j) {
            quicksort_rec(arr, left, j, size, k, compar);
            left = j + 1;
        } else {
            quicksort_rec(arr, j + 1, right, size, k, compar);
            right = j;
        }
    }
}

void hybridsort(void *base, size_t nitems, size_t size, size_t k, int (*compar)(const void *, const void *)) {
    if (nitems < 2) return;
    quicksort_rec((unsigned char *)base, 0, (long)nitems - 1, size, k, compar);
}


void quicksort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    if (nitems < 2) return;
    
    quicksort_rec((unsigned char *)base, 0, (long)nitems - 1, size, 0, compar);
}