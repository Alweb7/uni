#include "record.h"
#include "sort_records.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <input_file> <output_file> <field> <k>\n", argv[0]);
        return 1;
    }
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    size_t field = atoi(argv[3]);
    size_t k = atol(argv[4]);
    if (field < 1 || field > 4) {
        fprintf(stderr, "Campo di ordinamento non valido\n");
        return 1;
    }
    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        perror("Errore apertura input_file");
        return 1;
    }
    FILE *outfile = fopen(output_file, "wb");
    if (!outfile) {
        perror("Errore apertura output_file");
        fclose(infile);
        return 1;
    }
    sort_records(infile, outfile, field, k);
    fclose(infile);
    fclose(outfile);
    return 0;
}
