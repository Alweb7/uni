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

/** @brief Limiti massimi statici usati per dimensionare gli array condivisi. */
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

/** @brief Unione per la gestione dei parametri della semafori.
 * @param val Usato per operazioni di semctl con SETVAL, rappresenta il valore da assegnare al semaforo.
 * @param buf Usato per operazioni di semctl con IPC_STAT o IPC_SET, rappresenta un puntatore a una struttura semid_ds che contiene informazioni sul semaforo.
 * @param array Usato per operazioni di semctl con GETALL o SETALL, rappresenta un puntatore a un array di unsigned short che contiene i valori dei semafori in un set.*/
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/** @brief Tipologie di stazione gestite dalla mensa simulata. */
typedef enum station_type {
    STATION_PRIMI = 0,
    STATION_SECONDI = 1,
    STATION_COFFEE = 2,
    STATION_CASSA = 3,
    STATION_COUNT = 4
} station_type_t;

/** @brief Cause possibili di arresto della simulazione globale. */
typedef enum termination_reason {
    TERM_NONE = 0,
    TERM_TIMEOUT = 1,
    TERM_OVERLOAD = 2,
    TERM_SIGNAL = 3
} termination_reason_t;

/** @brief Rappresenta una singola voce di menu con nome e prezzo associato. 
 * @param name Il nome del piatto, usato anche per identificare univocamente il piatto stesso.
 * @param price_cents Il prezzo del piatto espresso in centesimi, usato per calcolare il ricavo totale e medio durante la simulazione.
*/
typedef struct dish {
    char name[MAX_NAME_LEN];
    int price_cents;
} dish_t;

/** @brief Struttura che raccoglie i dati del menu disponibile per una giornata di simulazione.
 * @param primi_count Numero di piatti disponibili per la categoria "primi".
 * @param secondi_count Numero di piatti disponibili per la categoria "secondi".
 * @param coffee_count Numero di piatti disponibili per la categoria "coffee".
 * @param primi Array di piatti disponibili per la categoria "primi".
 * @param secondi Array di piatti disponibili per la categoria "secondi".
 * @param coffee Array di piatti disponibili per la categoria "coffee".
 */
typedef struct menu_data {
    int primi_count;
    int secondi_count;
    int coffee_count;
    dish_t primi[MAX_DISHES_PER_TYPE];
    dish_t secondi[MAX_DISHES_PER_TYPE];
    dish_t coffee[MAX_DISHES_PER_TYPE];
} menu_data_t;

/** @brief Struttura che raccoglie tutti i parametri di configurazione della simulazione, letti da file di configurazione o da riga di comando.
 * @param sim_duration_days Durata complessiva della simulazione espressa in giorni simulati.
 * @param nanoseconds_per_simulated_minute Numero di nanosecondi reali che simulano un minuto della mensa.
 * @param day_duration_minutes Durata di una giornata simulata in minuti.
 * @param num_workers Numero di processi worker totali creati dal manager.
 * @param num_users Numero iniziale di processi utente avviati all'inizio.
 * @param max_dynamic_users Numero massimo di utenti aggiungibili dinamicamente tramite FIFO.
 * @param max_users_per_group Cardinalità massima di un gruppo di utenti che mangiano insieme.
 * @param overload_threshold Soglia oltre la quale una giornata è considerata in overload.
 * @param max_pauses_per_day Numero massimo di pause per ciascun worker in una giornata.
 * @param avg_pause_seconds Durata media di una pausa in secondi simulati.
 * @param stop_duration_seconds Durata standard di uno stop cassa attivato da comando esterno.
 * @param ticket_discount_percent Sconto percentuale applicato agli utenti con ticket.
 * @param ticket_reader_capacity Numero di lettori ticket usabili contemporaneamente.
 * @param avg_service_time_primi_seconds Tempi medi di servizio per le stazioni.
 * @param avg_service_time_secondi_seconds Tempi medi di servizio per le stazioni.
 * @param avg_service_time_coffee_seconds Tempi medi di servizio per le stazioni.
 * @param avg_service_time_cassa_seconds Tempi medi di servizio per le stazioni.
 * @param avg_refill_primi Quantità media reintegrata a ogni refill per primi e secondi.
 * @param avg_refill_secondi Quantità media reintegrata a ogni refill per primi e secondi.
 * @param avg_refill_time Intervallo medio, in minuti simulati, tra due refill successivi.
 * @param max_porzioni_primi Capienza massima del magazzino per ciascun tipo di piatto finito.
 * @param max_porzioni_secondi Capienza massima del magazzino per ciascun tipo di piatto finito.
 * @param num_worker_seats_primi Numero di postazioni fisiche disponibili per worker su ogni stazione.
 * @param num_worker_seats_secondi Numero di postazioni fisiche disponibili per worker su ogni stazione.
 * @param num_worker_seats_coffee Numero di postazioni fisiche disponibili per worker su ogni stazione.
 * @param num_worker_seats_cassa Numero di postazioni fisiche disponibili per worker su ogni stazione.
 * @param num_table_seats Numero totale di posti a sedere per consumare il pasto.
 * @param price_primi Prezzi base delle portate espressi in centesimi.
 * @param price_secondi Prezzi base delle portate espressi in centesimi.
 * @param price_coffee Prezzi base delle portate espressi in centesimi.
 * @param menu_path Percorsi delle risorse IPC e degli output statistici.
 * @param fifo_path Percorsi delle risorse IPC e degli output statistici.
 * @param csv_path Percorsi delle risorse IPC e degli output statistici.
*/
typedef struct config {
    int sim_duration_days;
    int nanoseconds_per_simulated_minute;
    int day_duration_minutes;
    int num_workers;
    int num_users;
    int max_dynamic_users;
    int max_users_per_group;
    int overload_threshold;
    int max_pauses_per_day;
    int avg_pause_seconds;
    int stop_duration_seconds;
    int ticket_discount_percent;
    int ticket_reader_capacity;
    int avg_service_time_primi_seconds;
    int avg_service_time_secondi_seconds;
    int avg_service_time_coffee_seconds;
    int avg_service_time_cassa_seconds;
    int avg_refill_primi;
    int avg_refill_secondi;
    int avg_refill_time;
    int max_porzioni_primi;
    int max_porzioni_secondi;
    int num_worker_seats_primi;
    int num_worker_seats_secondi;
    int num_worker_seats_coffee;
    int num_worker_seats_cassa;
    int num_table_seats;
    int price_primi;
    int price_secondi;
    int price_coffee;
    char menu_path[MAX_MENU_PATH];
    char fifo_path[MAX_FIFO_PATH];
    char csv_path[MAX_LOG_PATH];
} config_t;

/** @brief Snapshot dei dati statistici raccolti durante una giornata di simulazione.
 * @param served_users Numero totale di utenti che hanno completato il pasto con successo.
 * @param unserved_users Numero totale di utenti che non hanno completato il pasto.
 * @param dishes_served Array che conta il numero di piatti serviti per ciascuna stazione.
 * @param leftovers Array che conta il numero di porzioni avanzate per ciascuna stazione alla fine della giornata.
 * @param wait_count Array che conta il numero di attese per ciascuna stazione, usato per calcolare i tempi medi di attesa. 
 * @param pauses_total Numero totale di pause effettuate da tutti i worker in una giornata, usato per calcolare la media di pause per worker. 
 * @param active_workers_total Numero totale di worker attivi durante la giornata, usato per calcolare la media di worker attivi. 
 * @param revenue_cents Ricavo totale in centesimi accumulato durante la giornata, usato per calcolare il ricavo medio per utente. 
 * @param wait_ns Array che accumula il tempo totale di attesa in nanosecondi per ciascuna stazione, usato per calcolare i tempi medi di attesa.
*/
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


/** @brief Restituisce il tempo corrente in nanosecondi a partire da un'epoca fissa.
 * @return Il tempo corrente in nanosecondi, usato principalmente per misurare i tempi di attesa e i timeout nella simulazione.
 */
long long get_time_ns(void);


/** @brief Sospende il processo per il numero di nanosecondi richiesto. 
 * @param ns Il numero di nanosecondi per cui sospendere il processo. 
*/
void sleep_ns(long long ns);


/** @brief Crea un semaforo System V inizializzato al valore richiesto. 
 * @param value Il valore iniziale da assegnare al semaforo creato.
 * @return Il semaforo creato, rappresentato come un intero non negativo.
*/
ipc_sem_t semaphore_create(unsigned value);


/** @brief Esegue una P sul semaforo indicato. 
 * @param sem Il semaforo su cui eseguire l'operazione di wait. 
 * @return Restituisce 0 se l'operazione di wait è stata completata con successo. 
*/
int semaphore_wait(ipc_sem_t *sem);


/** @brief Esegue una V sul semaforo indicato. 
 * @param sem Il semaforo su cui eseguire l'operazione di post. 
 * @return Restituisce 0 se l'operazione di post è stata completata con successo. 
*/
int semaphore_post(ipc_sem_t *sem);


/** @brief Attende un semaforo fino al timeout indicato e restituisce true se l'attesa riesce. 
 * @param sem Il semaforo su cui eseguire l'operazione di wait con timeout. 
 * @param timeout_ns Il numero di nanosecondi da attendere prima di considerare l'operazione fallita. 
 * @return Restituisce true se l'operazione di wait è stata completata con successo entro il timeout specificato, false se il timeout è scaduto senza che il semaforo diventasse disponibile. 
*/
bool semaphore_wait_timeout(ipc_sem_t *sem, long long timeout_ns);


/** @brief Reimposta il valore corrente di un semaforo System V. 
 * @param sem Il semaforo di cui modificare il valore. 
 * @param value Il nuovo valore da assegnare al semaforo specificato. 
*/
void semaphore_set_value(ipc_sem_t sem, unsigned value);


/** @brief Rimuove il semaforo dal sistema. 
 * @param sem Il semaforo da rimuovere. 
*/
void semaphore_destroy(ipc_sem_t sem);


/** @brief   Termina il processo stampando il messaggio associato a errno. 
 * @param msg Il messaggio da stampare prima della descrizione dell'errore associato a errno. 
*/
void exit_with_errno(const char *msg);


/** @brief Termina il processo stampando un messaggio formattato su stderr. 
 * @param fmt La stringa di formato, simile a quella usata da printf, che specifica come formattare il messaggio di errore. 
 * @param ... Gli argomenti variabili che corrispondono ai specificatori di formato presenti nella stringa fmt. 
*/
void exit_with_message(const char *fmt, ...);


/** @brief Estrae un intero casuale nell'intervallo chiuso [min_value, max_value]. 
 * @param min_value Il limite inferiore dell'intervallo da cui estrarre il numero casuale. 
 * @return Un intero casuale compreso tra min_value e max_value, inclusi entrambi. 
*/
int random_in_range(int min_value, int max_value);


/** @brief Converte secondi simulati in nanosecondi reali in base alla configurazione attiva. 
 * @param cfg Un puntatore alla struttura di configurazione che contiene il parametro nanoseconds_per_simulated_minute, necessario per effettuare la conversione. 
 * @param sim_seconds Il numero di secondi simulati da convertire in nanosecondi reali. 
 * @return Il numero di nanosecondi reali corrispondente ai secondi simulati specificati.
*/
long long simulate_seconds_to_nanoseconds(const config_t *cfg, int sim_seconds);


/** @brief Applica una variazione casuale percentuale attorno a un valore base. 
 * @param base Il valore di riferimento attorno al quale applicare la variazione. 
 * @param percent La percentuale massima di variazione da applicare al valore base. 
*/
int add_random_variation(int base, int percent);


/** @brief Restituisce il nome testuale della stazione richiesta. 
 * @param station Il tipo di stazione di cui ottenere il nome. 
 * @return Una stringa costante che rappresenta il nome della stazione specificata.
*/
const char *get_station_name(station_type_t station);

#endif
