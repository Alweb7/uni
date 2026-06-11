#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#ifndef COMMON_H
#define COMMON_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Limiti massimi statici usati per dimensionare gli array condivisi. */
#define MAX_USERS 256
#define MAX_WORKERS 64
#define MAX_GROUPS MAX_USERS
#define MAX_QUEUE 512
#define MAX_DISHES_PER_TYPE 8
#define MAX_NAME_LEN 64
#define MAX_FIFO_PATH 128
#define MAX_LOG_PATH 128
#define MAX_MENU_PATH 128

typedef int ipc_sem_t;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/* Tipologie di stazione gestite dalla mensa simulata. */
typedef enum station_type {
    STATION_PRIMI = 0,
    STATION_SECONDI = 1,
    STATION_COFFEE = 2,
    STATION_CASSA = 3,
    STATION_COUNT = 4
} station_type_t;

/* Cause possibili di arresto della simulazione globale. */
typedef enum termination_reason {
    TERM_NONE = 0,
    TERM_TIMEOUT = 1,
    TERM_OVERLOAD = 2,
    TERM_SIGNAL = 3
} termination_reason_t;

/* Rappresenta una singola voce di menu con nome e prezzo associato. */
typedef struct dish {
    char name[MAX_NAME_LEN];
    int price_cents;
} dish_t;

/* Colleziona l'intero menu disponibile per una giornata di simulazione. */
typedef struct menu_data {
    int primi_count;
    int secondi_count;
    int coffee_count;
    dish_t primi[MAX_DISHES_PER_TYPE];
    dish_t secondi[MAX_DISHES_PER_TYPE];
    dish_t coffee[MAX_DISHES_PER_TYPE];
} menu_data_t;

/* Parametri runtime letti dal file di configurazione. */
typedef struct config {
    /* Durata complessiva della simulazione espressa in giorni simulati. */
    int sim_duration_days;
    /* Numero di nanosecondi reali che simulano un minuto della mensa. */
    int nanoseconds_per_simulated_minute;
    /* Durata di una giornata simulata in minuti. */
    int day_duration_minutes;
    /* Numero di processi worker totali creati dal manager. */
    int num_workers;
    /* Numero iniziale di processi utente avviati all'inizio. */
    int num_users;
    /* Numero massimo di utenti aggiungibili dinamicamente tramite FIFO. */
    int max_dynamic_users;
    /* Cardinalità massima di un gruppo di utenti che mangiano insieme. */
    int max_users_per_group;
    /* Soglia oltre la quale una giornata è considerata in overload. */
    int overload_threshold;
    /* Numero massimo di pause per ciascun worker in una giornata. */
    int max_pauses_per_day;
    /* Durata media di una pausa in secondi simulati. */
    int avg_pause_seconds;
    /* Durata standard di uno stop cassa attivato da comando esterno. */
    int stop_duration_seconds;
    /* Sconto percentuale applicato agli utenti con ticket. */
    int ticket_discount_percent;
    /* Numero di lettori ticket usabili contemporaneamente. */
    int ticket_reader_capacity;
    /* Tempi medi di servizio per le stazioni. */
    int avg_service_time_primi_seconds;
    int avg_service_time_secondi_seconds;
    int avg_service_time_coffee_seconds;
    int avg_service_time_cassa_seconds;
    /* Quantità media reintegrata a ogni refill per primi e secondi. */
    int avg_refill_primi;
    int avg_refill_secondi;
    /* Intervallo medio, in minuti simulati, tra due refill successivi. */
    int avg_refill_time;
    /* Capienza massima del magazzino per ciascun tipo di piatto finito. */
    int max_porzioni_primi;
    int max_porzioni_secondi;
    /* Numero di postazioni fisiche disponibili per worker su ogni stazione. */
    int num_worker_seats_primi;
    int num_worker_seats_secondi;
    int num_worker_seats_coffee;
    int num_worker_seats_cassa;
    /* Numero totale di posti a sedere per consumare il pasto. */
    int num_table_seats;
    /* Prezzi base delle portate espressi in centesimi. */
    int price_primi;
    int price_secondi;
    int price_coffee;
    /* Percorsi delle risorse IPC e degli output statistici. */
    char menu_path[MAX_MENU_PATH];
    char fifo_path[MAX_FIFO_PATH];
    char csv_path[MAX_LOG_PATH];
} config_t;

/* Snapshot aggregato delle metriche giornaliere o complessive. */
typedef struct stats_snapshot {
    long served_users;
    long unserved_users;
    long dishes_served[STATION_COUNT];
    long leftovers[STATION_COUNT];
    long wait_count[STATION_COUNT];
    long pauses_total;
    long active_workers_total;
    long revenue_cents;
    long long wait_ns[STATION_COUNT];
} stats_snapshot_t;

/* Restituisce il timestamp corrente del clock monotono in nanosecondi. */
long long get_time_ns(void);
/* Sospende il processo per il numero di nanosecondi richiesto. */
void sleep_ns(long long ns);
/* Crea un semaforo System V inizializzato al valore richiesto. */
ipc_sem_t semaphore_create(unsigned value);
/* Esegue una P sul semaforo indicato. */
int semaphore_wait(ipc_sem_t *sem);
/* Esegue una V sul semaforo indicato. */
int semaphore_post(ipc_sem_t *sem);
/* Attende un semaforo fino al timeout indicato e restituisce true se l'attesa riesce. */
bool semaphore_wait_timeout(ipc_sem_t *sem, long long timeout_ns);
/* Reimposta il valore corrente di un semaforo System V. */
void semaphore_set_value(ipc_sem_t sem, unsigned value);
/* Rimuove il semaforo dal sistema. */
void semaphore_destroy(ipc_sem_t sem);
/* Termina il processo stampando il messaggio associato a errno. */
void exit_with_errno(const char *msg);
/* Termina il processo stampando un messaggio formattato su stderr. */
void exit_with_message(const char *fmt, ...);
/* Estrae un intero casuale nell'intervallo chiuso [min_value, max_value]. */
int random_in_range(int min_value, int max_value);
/* Converte secondi simulati in nanosecondi reali in base alla configurazione attiva. */
long long simulate_seconds_to_nanoseconds(const config_t *cfg, int sim_seconds);
/* Applica una variazione casuale percentuale attorno a un valore base. */
int add_random_variation(int base, int percent);
/* Restituisce il nome testuale della stazione richiesta. */
const char *get_station_name(station_type_t station);

#endif
