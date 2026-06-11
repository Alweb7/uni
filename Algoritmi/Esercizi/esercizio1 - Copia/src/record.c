#include "record.h"
#include <string.h>
#include <stdlib.h>

int read_record(FILE *file, Record *record) {
    return fread(record, sizeof(Record), 1, file) == 1 ? 0 : -1;
}

int write_record(FILE *file, const Record *record) {
    return fwrite(record, sizeof(Record), 1, file) == 1 ? 0 : -1;
}

int load_records_from_file(FILE *file, Record **records, size_t *count) {
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    if (filesize < 0) return -1;
    *count = filesize / sizeof(Record);
    fseek(file, 0, SEEK_SET);
    *records = malloc(*count * sizeof(Record));
    if (!*records) return -1;
    size_t n = fread(*records, sizeof(Record), *count, file);
    if (n != *count) {
        free(*records);
        *records = NULL;
        *count = 0;
        return -1;
    }
    return 0;
}

int save_records_to_file(FILE *file, const Record *records, size_t count) {
    return fwrite(records, sizeof(Record), count, file) == count ? 0 : -1;
}

void print_record(const Record *record) {
    printf("id: %llu, field1: %f, field2: %lld, field3: '%s'\n",
        (unsigned long long)record->id, record->field1, (long long)record->field2, record->field3);
}
