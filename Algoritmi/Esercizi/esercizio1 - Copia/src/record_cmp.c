#include "record_cmp.h"
#include <string.h>

int cmp_record_id(const void *a, const void *b) {
    const Record *ra = a, *rb = b;
    if (ra->id < rb->id) return -1;
    if (ra->id > rb->id) return 1;
    return 0;
}

int cmp_record_field1(const void *a, const void *b) {
    const Record *ra = a, *rb = b;
    if (ra->field1 < rb->field1) return -1;
    if (ra->field1 > rb->field1) return 1;
    return 0;
}

int cmp_record_field2(const void *a, const void *b) {
    const Record *ra = a, *rb = b;
    if (ra->field2 < rb->field2) return -1;
    if (ra->field2 > rb->field2) return 1;
    return 0;
}

int cmp_record_field3(const void *a, const void *b) {
    const Record *ra = a, *rb = b;
    return strncmp(ra->field3, rb->field3, FIELD3_LEN);
}
