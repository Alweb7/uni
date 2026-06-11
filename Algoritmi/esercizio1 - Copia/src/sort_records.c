#include "record.h"
#include "record_cmp.h"
#include "lib_sort.h"
#include <stdio.h>
#include <stdlib.h>

void sort_records(FILE *infile, FILE *outfile, size_t field, size_t k) {
    Record *records = NULL;
    size_t n = 0;

    printf("--- Inizio Operazione ---\n");
    printf("Caricamento in corso...\n");

    if (load_records_from_file(infile, &records, &n) != 0) {
        fprintf(stderr, "Errore: il file records.bin non e' stato trovato o e' corrotto.\n");
        return;
    }

    printf("Record caricati: %u\n", (unsigned int)n);

    int (*cmp)(const void *, const void *) = NULL;
    if (field == 1) cmp = cmp_record_id;
    else if (field == 2) cmp = cmp_record_field1;
    else if (field == 3) cmp = cmp_record_field2;
    else if (field == 4) cmp = cmp_record_field3;

    if (cmp == NULL) {
        fprintf(stderr, "Errore: Campo %u non valido.\n", (unsigned int)field);
        free(records);
        return;
    }

    printf("Ordinamento iniziato (k=%u)... attendi...\n", (unsigned int)k);
    
    hybridsort(records, n, sizeof(Record), k, cmp);
    
    printf("Ordinamento terminato correttamente!\n");
    printf("Salvataggio su file...\n");

    if (save_records_to_file(outfile, records, n) != 0) {
        fprintf(stderr, "Errore nel salvataggio.\n");
    }
    
    printf("--- Operazione Completata ---\n");
    free(records);
}