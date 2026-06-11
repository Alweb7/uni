#ifndef IPC_H
#define IPC_H

#include "common.h"

/* Stato privato di un singolo utente visibile a tutti i processi. */
typedef struct user_state {
    /* Identificativo stabile dell'utente dentro gli array condivisi. */
    int id;
    /* Gruppo sociale con cui l'utente sincronizza il passaggio in cassa. */
    int group_id;
    /* Flag che indica la disponibilità del ticket mensa. */
    int has_ticket;
    /* Flag usato dal manager per sapere se il processo esiste ancora. */
    int alive;
    /* Indica se oggi l'utente ha deciso di recarsi in mensa. */
    int attending_today;
    /* Evita il doppio conteggio dell'esito giornaliero nelle statistiche. */
    int daily_outcome_recorded;
    /* Numero di piatti effettivamente ottenuti nella giornata corrente. */
    int dishes_bought;
    /* Preferenze giornaliere espresse come indice nel menu. */
    int wanted_primo;
    int wanted_secondo;
    int wants_coffee;
    /* Campi usati per passare la richiesta al worker di turno. */
    int request_station;
    int request_dish;
    /* Esito elaborato dal worker e restituito al processo utente. */
    int service_successful;
    int served_dish_index;
    /* Totale corrente da pagare in cassa per la giornata. */
    int total_price_cents;
    /* Timestamp di enqueue per misurare il tempo di attesa reale. */
    long long request_enqueued_ns;
    /* Semaforo privato con cui il worker sblocca l'utente servito. */
    ipc_sem_t served_sem;
} user_state_t;

/* Stato condiviso di un gruppo di utenti che vuole andare alla cassa insieme. */
typedef struct group_state {
    /* Protegge i contatori del gruppo. */
    ipc_sem_t mutex;
    /* Implementa una barriera semplice tra i membri del gruppo presenti. */
    ipc_sem_t barrier_sem;
    /* Numero di membri che oggi hanno deciso di partecipare. */
    int planned_today;
    /* Numero di membri che hanno finito le stazioni cibo e sono pronti. */
    int ready_today;
} group_state_t;

/* Coda circolare usata da ciascuna stazione per ricevere richieste utente. */
typedef struct station_queue {
    /* Mutex interno per accesso coerente a head, tail e count. */
    ipc_sem_t mutex;
    /* Conta gli elementi pronti al consumo da parte dei worker. */
    ipc_sem_t items;
    /* Conta gli slot liberi disponibili nella coda circolare. */
    ipc_sem_t slots;
    /* Modella le postazioni fisiche per i worker assegnati alla stazione. */
    ipc_sem_t seat_sem;
    /* Indici della coda FIFO. */
    int head;
    int tail;
    /* Numero di utenti attualmente in attesa. */
    int count;
    /* Worker realmente attivi oggi su questa stazione. */
    int active_workers;
    /* Worker assegnati dal manager all'inizio della giornata. */
    int assigned_workers;
    /* Buffer circolare contenente gli id utente. */
    int queue[MAX_QUEUE];
} station_queue_t;

/* Magazzino delle portate con disponibilità finita. */
typedef struct inventory_state {
    ipc_sem_t mutex;
    int primi[MAX_DISHES_PER_TYPE];
    int secondi[MAX_DISHES_PER_TYPE];
} inventory_state_t;

/* Stato globale della simulazione condiviso via memoria tra tutti i processi. */
typedef struct shared_state {
    /* Copia della configurazione e del menu letti dal manager all'avvio. */
    config_t cfg;
    menu_data_t menu;
    /* Sincronizzatori di bootstrap e di inizio giornata. */
    ipc_sem_t init_sem;
    ipc_sem_t day_start_sem;
    /* Protegge contatori statistici e altri piccoli campi condivisi. */
    ipc_sem_t stats_mutex;
    /* Risorse contese dagli utenti durante la giornata. */
    ipc_sem_t ticket_reader_sem;
    ipc_sem_t table_sem;
    /* Flag globali della simulazione modificati dal manager o dai segnali. */
    volatile sig_atomic_t shutdown;
    volatile sig_atomic_t day_open;
    volatile sig_atomic_t current_day;
    volatile sig_atomic_t started;
    volatile sig_atomic_t disorder_active;
    volatile sig_atomic_t overload_triggered;
    /* Momento di fine del blocco cassa generato da DISORDER. */
    long long disorder_until_ns;
    /* Numero totale di utenti creati e numero ancora vivi nella simulazione. */
    int total_users_spawned;
    int living_users;
    /* Mapping worker -> stazione e metriche cumulative sui worker. */
    int worker_assignments[MAX_WORKERS];
    int worker_ever_active[MAX_WORKERS];
    int worker_pauses_day[MAX_WORKERS];
    int worker_pauses_total[MAX_WORKERS];
    /* Occupazione tavoli della giornata corrente. */
    int table_occupied;
    /* Risorse operative della mensa. */
    station_queue_t stations[STATION_COUNT];
    inventory_state_t inventory;
    group_state_t groups[MAX_GROUPS];
    user_state_t users[MAX_USERS];
    /* Snapshot delle statistiche del giorno e dell'intera simulazione. */
    stats_snapshot_t day_stats;
    stats_snapshot_t total_stats;
} shared_state_t;

/* Crea un segmento di memoria condivisa System V e lo attacca al processo chiamante. */
int ipc_create_and_map(size_t size, void **addr);
/* Apre un segmento di memoria condivisa System V esistente e lo attacca al processo chiamante. */
int ipc_open_and_map(const char *token, size_t size, void **addr);
/* Stacca l'area condivisa dal processo corrente. */
void ipc_unmap_and_close(int shm_id, void *addr, size_t size);
/* Rimuove il segmento di memoria condivisa dal sistema. */
void ipc_destroy(int shm_id);
/* Inizializza lo stato condiviso e tutte le primitive di sincronizzazione del simulatore. */
void initialize_shared_state(shared_state_t *shared, const config_t *cfg, const menu_data_t *menu);
/* Dealloca tutte le primitive IPC interne allo stato condiviso. */
void shared_destroy(shared_state_t *shared);

#endif
