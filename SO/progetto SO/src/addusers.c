#include "common.h"

/* Invia al manager un comando per aggiungere nuovi utenti alla simulazione corrente. */
int main(int argc, char **argv) {
    /* fd è il descrittore della FIFO del manager. */
    int fd;
    /* line contiene il comando testuale da inviare al manager. */
    char line[64];
    /* Utility esterna: scrive un comando testuale sulla FIFO del manager. */
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
