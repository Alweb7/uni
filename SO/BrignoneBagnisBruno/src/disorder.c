#include "common.h"

/** Questo file contiene un piccolo programma di utilità per causare un temporaneo disordine alla cassa.
 * Il programma accetta come argomento il percorso della FIFO del manager e invia un comando testuale
 * per sospendere temporaneamente la cassa, causando l'accumulo di utenti in attesa e mettendo alla prova
 * la gestione dei gruppi e delle code da parte del manager e dei worker.
 */
int main(int argc, char **argv) {
    int fd;
    static const char line[] = "DISORDER\n";
    if (argc != 2) {
        exit_with_message("uso: disorder <percorso_fifo>");
    }
    fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        exit_with_errno("open fifo");
    }
    if (write(fd, line, strlen(line)) < 0) {
        exit_with_errno("write fifo");
    }
    close(fd);
    return 0;
}
