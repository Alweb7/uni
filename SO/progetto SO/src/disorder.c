#include "common.h"

/* Invia al manager un comando per bloccare temporaneamente i pagamenti in cassa. */
int main(int argc, char **argv) {
    /* fd è il descrittore della FIFO del manager. */
    int fd;
    /* Utility esterna: chiede al manager di sospendere temporaneamente la cassa. */
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
