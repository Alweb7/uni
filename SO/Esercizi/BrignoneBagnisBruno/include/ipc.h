#ifndef IPC_H
#define IPC_H

#include "common.h"


/** @brief Stato privato di un singolo utente visibile a tutti i processi.
 * @param id Identificativo stabile dell'utente dentro gli array condivisi. 
 * @param group_id Gruppo sociale con cui l'utente sincronizza il passaggio in cassa. 
 * @param has_ticket Flag che indica la disponibilità del ticket mensa. 
 * @param alive Flag usato dal manager per sapere se il processo esiste ancora. 
 * @param attending_today Indica se oggi l'utente ha deciso di recarsi in mensa. 
 * @param daily_outcome_recorded Evita il doppio conteggio dell'esito giornaliero nelle statistiche. 
 * @param dishes_bought Numero di piatti effettivamente ottenuti nella giornata corrente. 
 * @param wanted_primo Preferenze giornaliere espresse come indice nel menu. 
 * @param wanted_secondo Preferenze giornaliere espresse come indice nel menu. 
 * @param wants_coffee Preferenze giornaliere espresse come indice nel menu. 
 * @param request_station Campi usati per passare la richiesta al worker di turno. 
 * @param request_dish Campi usati per passare la richiesta al worker di turno. 
 * @param service_successful Esito elaborato dal worker e restituito al processo utente. 
 * @param served_dish_index Esito elaborato dal worker e restituito al processo utente. 
 * @param total_price_cents Totale corrente da pagare in cassa per la giornata. 
 * @param request_enqueued_ns Timestamp di enqueue per misurare il tempo di attesa reale. 
 * @param served_sem Semaforo privato con cui il worker sblocca l'utente servito.
 */
typedef struct user_state {
    int id;
    int group_id;
    int has_ticket;
    int alive;
    int attending_today;
    int daily_outcome_recorded;
    int dishes_bought;
    int wanted_primo;
    int wanted_secondo;
    int wants_coffee;
    int request_station;
    int request_dish;
    int service_successful;
    int served_dish_index;
    int total_price_cents;
    long long request_enqueued_ns;
    ipc_sem_t served_sem;
} user_state_t;


/** @brief Stato condiviso di un gruppo di utenti che vuole andare alla cassa insieme.
 * @param mutex Protegge i contatori del gruppo.
 * @param barrier_sem Implementa una barriera semplice tra i membri del gruppo presenti.
 * @param planned_today Numero di membri del gruppo che hanno pianificato di venire in mensa oggi.
 * @param ready_today Numero di membri del gruppo che sono arrivati alla cassa oggi e stanno aspettando i compagni.
 */
typedef struct group_state {
    ipc_sem_t mutex;
    ipc_sem_t barrier_sem;
    int planned_today;
    int ready_today;
} group_state_t;


/** @brief Coda circolare usata da ciascuna stazione per ricevere richieste utente.
 * @param mutex Mutex interno per accesso coerente a head, tail e count.
 * @param items Conta gli elementi pronti al consumo da parte dei worker.
 * @param slots Conta gli slot liberi disponibili nella coda circolare.
 * @param seat_sem Modella le postazioni fisiche per i worker assegnati alla stazione.
 * @param head Indice di testa della coda circolare.
 * @param tail Indice di coda della coda circolare.
 * @param count Numero di utenti attualmente in attesa nella coda.
 * @param active_workers Numero di worker realmente attivi oggi su questa stazione.
 * @param assigned_workers Numero di worker assegnati dal manager all'inizio della giornata.
 * @param queue Buffer circolare contenente gli id utente in attesa.
 */
typedef struct station_queue {
    ipc_sem_t mutex;
    ipc_sem_t items;
    ipc_sem_t slots;
    ipc_sem_t seat_sem;
    int head;
    int tail;
    int count;
    int active_workers;
    int assigned_workers;
    int queue[MAX_QUEUE];
} station_queue_t;


/** @brief Stato condiviso dell'inventario dei piatti disponibili, aggiornato dai worker durante la giornata.
 * @param mutex Protegge l'accesso concorrente all'inventario.
 * @param primi Array che tiene traccia delle porzioni rimanenti per ciascun primo.
 * @param secondi Array che tiene traccia delle porzioni rimanenti per ciascun secondo.
 */
typedef struct inventory_state {
    ipc_sem_t mutex;
    int primi[MAX_DISHES_PER_TYPE];
    int secondi[MAX_DISHES_PER_TYPE];
} inventory_state_t;


/** @brief Stato globale della simulazione condiviso via memoria tra tutti i processi.
 * @param cfg Copia della configurazione letta dal manager all'avvio.
 * @param menu Copia del menu letto dal manager all'avvio.
 * @param init_sem Sincronizzatore di bootstrap.
 * @param day_start_sem Sincronizzatore di inizio giornata.
 * @param stats_mutex Protegge contatori statistici e altri piccoli campi condivisi.
 * @param ticket_reader_sem Risorsa contesa dagli utenti per accedere al lettore di ticket.
 * @param table_sem Risorsa contesa dagli utenti per accedere ai tavoli.
 * @param shutdown Flag globale modificato dal manager o dai segnali per indicare la richiesta di terminare la simulazione.
 * @param day_open Flag globale modificato dal manager per indicare che la giornata è aperta e i worker possono iniziare a servire.
 * @param current_day Contatore globale del giorno corrente, usato principalmente per le statistiche.
 * @param started Flag globale modificato dal manager per indicare che la simulazione è iniziata, usato principalmente per i segnali.
 * @param disorder_active Flag globale modificato dal manager per indicare che è attivo un evento di DISORDER, usato principalmente per i segnali.
 * @param overload_triggered Flag globale modificato dal manager per indicare che è stata superata la soglia di overload, usato principalmente per i segnali.
 * @param disorder_until_ns Momento di fine del blocco cassa generato da DISORDER, usato principalmente per i segnali.
 * @param total_users_spawned Contatore globale del numero totale di utenti creati durante la simulazione, usato principalmente per le statistiche.
 * @param living_users Contatore globale del numero di utenti ancora vivi nella simulazione, usato principalmente per i segnali.
 * @param worker_assignments Array che mappa ciascun worker alla stazione a cui è assegnato oggi.
 * @param worker_ever_active Array di flag che indicano se ciascun worker è stato attivo almeno un giorno, usato principalmente per le statistiche.
 * @param worker_pauses_day Array che conta il numero di pause effettuate da ciascun worker nella giornata corrente, usato principalmente per i segnali.
 * @param worker_pauses_total Array che conta il numero totale di pause effettuate da ciascun worker durante la simulazione, usato principalmente per le statistiche.
 */
typedef struct shared_state {
    config_t cfg;
    menu_data_t menu;
    ipc_sem_t init_sem;
    ipc_sem_t day_start_sem;
    ipc_sem_t stats_mutex;
    ipc_sem_t ticket_reader_sem;
    ipc_sem_t table_sem;
    volatile sig_atomic_t shutdown;
    volatile sig_atomic_t day_open;
    volatile sig_atomic_t current_day;
    volatile sig_atomic_t started;
    volatile sig_atomic_t disorder_active;
    volatile sig_atomic_t overload_triggered;
    long long disorder_until_ns;
    int total_users_spawned;
    int living_users;
    int worker_assignments[MAX_WORKERS];
    int worker_ever_active[MAX_WORKERS];
    int worker_pauses_day[MAX_WORKERS];
    int worker_pauses_total[MAX_WORKERS];
    int table_occupied;
    station_queue_t stations[STATION_COUNT];
    inventory_state_t inventory;
    group_state_t groups[MAX_GROUPS];
    user_state_t users[MAX_USERS];
    stats_snapshot_t day_stats;
    stats_snapshot_t total_stats;
} shared_state_t;


/** @brief Crea un segmento di memoria condivisa System V e lo attacca al processo chiamante.
 * @param size La dimensione in byte del segmento di memoria condivisa da creare.
 * @param addr Puntatore a un puntatore che riceverà l'indirizzo di memoria a cui è stato attaccato il segmento condiviso. 
 * @return Il token identificativo del segmento di memoria condivisa creato, da usare per aprirlo in altri processi
 */
int ipc_create_and_map(size_t size, void **addr);


/** @brief Apre un segmento di memoria condivisa System V esistente e lo attacca al processo chiamante.
 * @param token Il token identificativo del segmento di memoria condivisa da aprire.
 * @param size La dimensione in byte del segmento di memoria condivisa.
 * @param addr Puntatore a un puntatore che riceverà l'indirizzo di memoria a cui è stato attaccato il segmento condiviso.
 * @return Il token identificativo del segmento di memoria condivisa aperto, da usare per chiuderlo in altri processi
 */
int ipc_open_and_map(const char *token, size_t size, void **addr);


/** @brief Stacca l'area condivisa dal processo corrente.
 * @param shm_id Il token identificativo del segmento di memoria condivisa.
 * @param addr L'indirizzo di memoria a cui è stato attaccato il segmento condiviso.
 * @param size La dimensione in byte del segmento di memoria condivisa.
 */
void ipc_unmap_and_close(int shm_id, void *addr, size_t size);


/** @brief Rimuove il segmento di memoria condivisa dal sistema.
 * @param shm_id Il token identificativo del segmento di memoria condivisa da rimuovere.
 */
void ipc_destroy(int shm_id);


/** @brief Inizializza lo stato condiviso e tutte le primitive di sincronizzazione del simulatore.
 * @param shared Puntatore allo stato condiviso da inizializzare.
 * @param cfg Puntatore alla configurazione della simulazione.
 * @param menu Puntatore ai dati del menu della simulazione.
 */
void initialize_shared_state(shared_state_t *shared, const config_t *cfg, const menu_data_t *menu);


/** @brief Dealloca tutte le primitive IPC interne allo stato condiviso.
 * @param shared Puntatore allo stato condiviso da deallocare.
 */
void shared_destroy(shared_state_t *shared);

#endif
