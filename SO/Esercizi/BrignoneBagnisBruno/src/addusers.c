#include "common.h"

/** Questo file contiene un piccolo programma di utilità per aggiungere utenti alla simulazione in esecuzione.
 * Il programma accetta come argomenti il percorso della FIFO del manager e il numero di utenti da aggiungere,
 * e invia un comando testuale al manager per richiedere la creazione dei nuovi utenti.
 */
int main(int argc, char **argv) {
    int fd;
    char line[64];
    if (argc != 3) {
        exit_with_message("uso: addusers <percorso_fifo> <numero_utenti>");
    }
    fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        exit_with_errno("open fifo");
    }
    snprintf(line, sizeof(line), "ADD_USERS %d\n", atoi(argv[2]));
    if (write(fd, line, strlen(line)) < 0) {
        exit_with_errno("write fifo");
    }
    close(fd);
    return 0;
}
