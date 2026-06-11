/*
 * es01_fork_base.c
 * Esempio base di fork(): creazione di un processo figlio
 *
 * Compilazione: gcc -o es01 es01_fork_base.c
 * Esecuzione:   ./es01
 */
// ciao
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // fork(), getpid(), getppid()
#include <sys/wait.h> // wait()

int main(void) {
    pid_t pid;

    printf("=== Processo PADRE avviato (PID: %d) ===\n", getpid());

    pid = fork(); // <-- qui nasce il processo figlio

    if (pid < 0) {
        // Errore: fork() fallita
        perror("fork fallita");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Siamo nel FIGLIO (fork() restituisce 0)
        printf("[FIGLIO]  PID=%d, PID del padre=%d\n", getpid(), getppid());
        printf("[FIGLIO]  Sto lavorando...\n");
        sleep(1);
        printf("[FIGLIO]  Ho finito, esco.\n");
        exit(EXIT_SUCCESS);

    } else {
        // Siamo nel PADRE (fork() restituisce il PID del figlio)
        printf("[PADRE]   PID=%d, ho creato il figlio con PID=%d\n", getpid(), pid);
        printf("[PADRE]   Aspetto che il figlio finisca...\n");

        int status;
        wait(&status); // attendo che il figlio termini

        if (WIFEXITED(status)) {
            printf("[PADRE]   Il figlio e' uscito con codice: %d\n", WEXITSTATUS(status));
        }
        printf("[PADRE]   Fine programma.\n");
    }

    return 0;
}
