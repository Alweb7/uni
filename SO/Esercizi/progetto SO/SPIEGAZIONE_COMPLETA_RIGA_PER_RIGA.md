# 📚 SPIEGAZIONE COMPLETA RIGA PER RIGA - PROGETTO MENSA

---

## 📋 INDICE NAVIGAZIONE

1. [INCLUDE/COMMON.H](#includecmmonnh)
2. [INCLUDE/CONFIG.H](#includeconfigh)
3. [INCLUDE/IPC.H](#includeipcjh)
4. [INCLUDE/SIMULATION.H](#includesimulationh)
5. [SRC/COMMON.C](#srccommonc)
6. [SRC/CONFIG.C](#srcconfigc)
7. [SRC/IPC.C](#srcipc)
8. [SRC/SIMULATION.C](#srcsimulationc)
9. [SRC/MANAGER.C](#srcmanagerc)
10. [SRC/WORKER.C](#srcworkerc)
11. [SRC/USER.C](#srcuserc)
12. [SRC/ADDUSERS.C](#srcaddusersc)
13. [SRC/DISORDER.C](#srcdisorderc)

---

# INCLUDE/COMMON.H

## RIGHE 1-8: PROTEZIONE MULTI INCLUSIONE

```c
#ifndef _GNU_SOURCE
#define _GNU_SOURCE //Necessarie per funzioni come `semtimedop()` che non sono POSIX standard
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  //POSIX.1-2008 (aka POSIX 2008)
#endif

#ifndef COMMON_H
#define COMMON_H
```

**Spiegazione:**
- **#ifndef _GNU_SOURCE ... #define _GNU_SOURCE**: Se non è già definito, attiva le estensioni GNU di C
  - Necessarie per funzioni come `semtimedop()` che non sono POSIX standard
  - Deve essere primo (#define prima di qualsiasi #include)

- **#ifndef _POSIX_C_SOURCE ... #define _POSIX_C_SOURCE 200809L**: Richiede standard POSIX 2008 completo
  - 200809L = POSIX.1-2008 (aka POSIX 2008)
  - Abilita funzioni come `clock_gettime()`, `nanosleep()`, `semtimedop()`

- **#ifndef COMMON_H ... #define COMMON_H**: Include guard standard
  - Se header già incluso, salta tutto il contenuto
  - Evita compilazione multipla dello stesso file

---

## RIGHE 10-33: INCLUDE FILES

```c
#include <errno.h>          // Numeri di errore globali (EINTR, EAGAIN, etc.)
#include <fcntl.h>          // File control: O_WRONLY, O_NONBLOCK, etc.
#include <limits.h>         // INT_MIN, INT_MAX, PATH_MAX
#include <signal.h>         // signal(), SIGINT, SIGTERM
#include <stdarg.h>         // va_list, va_start, va_end (printf variadic)
#include <stdbool.h>        // bool, true, false
#include <stddef.h>         // size_t, NULL, offsetof
#include <stdint.h>         // int64_t, uint32_t, etc.
#include <stdio.h>          // printf, fopen, fread, etc.
#include <stdlib.h>         // malloc, free, exit, rand, atoi
#include <string.h>         // strlen, strcpy, strcmp, memset, memmove
#include <sys/ipc.h>        // IPC_PRIVATE, IPC_CREAT, IPC_EXCL
#include <sys/sem.h>        // semget, semop, semctl
#include <sys/shm.h>        // shmget, shmat, shmdt, shmctl
#include <sys/stat.h>       // S_IRWXU, S_IRUSR, etc. (permission bits)
#include <sys/types.h>      // pid_t, uid_t, gid_t, key_t
#include <sys/wait.h>       // wait(), WIFEXITED, etc.
#include <time.h>           // clock_gettime, nanosleep, timespec
#include <unistd.h>         // fork, execl, close, write, sleep
```

**Spiegazione:**
Ogni header fornisce funzioni critiche:
- **IPC**: semafori, memoria condivisa, sincronizzazione
- **SYSTEM**: processi, tempo, segnali
- **STRING**: parsing configurazione, manipolazione stringhe

---

## RIGHE 35-44: LIMITI ARRAY STATICI

```c
#define MAX_USERS 256          // Max 256 utenti (iniziali + dinamici)
#define MAX_WORKERS 64         // Max 64 operatori
#define MAX_GROUPS MAX_USERS   // Max gruppi = numero utenti (worst case)
#define MAX_QUEUE 512          // Capienza max coda circolare per stazione
#define MAX_DISHES_PER_TYPE 8  // Max 8 piatti per tipo (primi, secondi, caffè)
#define MAX_NAME_LEN 64        // Max 64 caratteri per nome piatto
#define MAX_FIFO_PATH 128      // Max 128 caratteri per percorso FIFO
#define MAX_LOG_PATH 128       // Max 128 caratteri per percorso CSV
#define MAX_MENU_PATH 128      // Max 128 caratteri per percorso menu.txt
```

**Spiegazione:**
Questi sono limiti fissi usati per dimensionare array globali nella memoria condivisa:
- Non dinamici (non usiamo malloc per memoria condivisa)
- Array di dimensione fissa garantisce coherenza tra processi
- Es: `user_state_t users[MAX_USERS]` creato una sola volta al boot

---

## RIGHE 46-50: TYPEDEF SEMAFORO E UNION SEMUN

```c
typedef int ipc_sem_t;        // Alias: ID semaforo è un intero

union semun {                  // Union richiesto da semctl() per portabilità
    int val;                   // Valore intero per SETVAL
    struct semid_ds *buf;      // Struttura di stato per IPC_STAT
    unsigned short *array;     // Array di valori per versione vettore
};
```

**Spiegazione:**

- **typedef int ipc_sem_t**: Astrae l'ID semaforo (è un intero System V)
  - Rende il codice più leggibile
  - Potrebbe cambiare implementazione in futuro

- **union semun**: Richiesto da `semctl()` POSIX
  - `semctl(semid, sem_num, cmd, arg)` vuole un `union semun`
  - Non è typedef standard in POSIX, dobbiamo definirlo noi
  - Usiamo `.val` per impostare il valore iniziale del semaforo

---

## RIGHE 52-60: ENUM STAZIONI

```c
typedef enum station_type {
    STATION_PRIMI = 0,     // Stazione primi piatti
    STATION_SECONDI = 1,   // Stazione secondi
    STATION_COFFEE = 2,    // Stazione caffè/dolci
    STATION_CASSA = 3,     // Stazione cassa pagamenti
    STATION_COUNT = 4      // Costante = numero di stazioni
} station_type_t;
```

**Spiegazione:**
- Enum per stazioni della mensa (più type-safe di #define)
- Indici 0-3 usati per accesso array `stations[STATION_PRIMI]`
- **STATION_COUNT = 4**: costante pratica per loop `for (i = 0; i < STATION_COUNT; ++i)`

---

## RIGHE 62-67: ENUM MOTIVI TERMINAZIONE

```c
typedef enum termination_reason {
    TERM_NONE = 0,      // Nessuna (non terminato)
    TERM_TIMEOUT = 1,   // Terminato per raggiungimento giorni simulati
    TERM_OVERLOAD = 2,  // Terminato per sovraccarico code
    TERM_SIGNAL = 3     // Terminato per segnale (SIGINT/SIGTERM)
} termination_reason_t;
```

**Spiegazione:**
Traccia il motivo finale di terminazione della simulazione per report e CSV.

---

## RIGHE 69-73: STRUCT PIATTO (DISH)

```c
typedef struct dish {
    char name[MAX_NAME_LEN];       // Nome piatto (es: "Pasta Bolognese")
    int price_cents;               // Prezzo in centesimi (100 = 1 euro)
} dish_t;
```

**Spiegazione:**
- Rappresenta una singola voce del menu
- `price_cents`: precisione centesimi (evita floating point)
- Esempio: "Pasta" = 850 centesimi = 8.50€

---

## RIGHE 75-84: STRUCT MENU

```c
typedef struct menu_data {
    int primi_count;              // Numero di primi disponibili
    int secondi_count;            // Numero di secondi disponibili
    int coffee_count;             // Numero di caffè/dolci disponibili
    dish_t primi[MAX_DISHES_PER_TYPE];       // Array primi (max 8)
    dish_t secondi[MAX_DISHES_PER_TYPE];     // Array secondi
    dish_t coffee[MAX_DISHES_PER_TYPE];      // Array caffè/dolci
} menu_data_t;
```

**Spiegazione:**
Menu completo del giorno, condiviso tra tutti i processi via memoria condivisa.
- Caricato una sola volta al boot da `menu.txt`
- Identico per tutta la simulazione
- Ogni utente sceglie da questo menu

---

## RIGHE 86-153: STRUCT CONFIG

```c
typedef struct config {
    /* DURATA E TIMING */
    int sim_duration_days;         // Giorni simulati (es: 3)
    int nanoseconds_per_simulated_minute;      // Nanosecondi reali per minuto simulato
                                   // Es: 50000000 = 1 min sim = 50ms reale
    int day_duration_minutes;      // Minuti simulati per giorno (es: 120)
    
    /* PROCESSI E UTENTI */
    int num_workers;               // Numero operatori (es: 6)
    int num_users;                 // Utenti iniziali (es: 18)
    int max_dynamic_users;         // Max utenti aggiungibili (es: 16)
    int max_users_per_group;       // Utenti per gruppo (es: 3)
    
    /* SOGLIE DI CONTROLLO */
    int overload_threshold;        // Soglia persone in coda per overload (es: 8)
    int max_pauses_per_day;                 // Max pause/operatore/giorno (es: 2)
    int avg_pause_seconds;         // Durata media pausa in sec simulati (es: 5)
    int stop_duration_seconds;     // Durata blocco cassa DISORDER (es: 15)
    
    /* TICKET */
    int ticket_discount_percent;   // Sconto % con ticket (es: 25%)
    int ticket_reader_capacity;    // Lettori ticket paralleli (es: 1)
    
    /* TEMPI SERVIZIO (SECONDI SIMULATI) */
    int avg_service_time_primi_seconds;            // Tempo medio serv. primi (es: 6s)
    int avg_service_time_secondi_seconds;          // Tempo medio serv. secondi (es: 8s)
    int avg_service_time_coffee_seconds;           // Tempo medio serv. caffè (es: 3s)
    int avg_service_time_cassa_seconds;            // Tempo medio serv. cassa (es: 4s)
    
    /* REFILL MAGAZZINO */
    int avg_refill_primi;          // Quantità refill per primo (es: 12 porzioni)
    int avg_refill_secondi;        // Quantità refill per secondo (es: 12)
    int avg_refill_time;           // Intervallo refill in minuti (es: 10)
    int max_porzioni_primi;        // Capienza max magazzino primi (es: 20)
    int max_porzioni_secondi;      // Capienza max magazzino secondi (es: 20)
    
    /* POSTAZIONI FISICHE (POSTI IN FILA) */
    int num_worker_seats_primi;        // Postazioni stazione primi (es: 2)
    int num_worker_seats_secondi;      // Postazioni stazione secondi (es: 2)
    int num_worker_seats_coffee;       // Postazioni stazione caffè (es: 1)
    int num_worker_seats_cassa;        // Postazioni cassa (es: 1)
    
    /* TAVOLI */
    int num_table_seats;           // Posti totali per mangiare (es: 10)
    
    /* PREZZI (CENTESIMI) */
    int price_primi;               // Prezzo primo in centesimi (es: 650 = 6.50€)
    int price_secondi;             // Prezzo secondo (es: 850 = 8.50€)
    int price_coffee;              // Prezzo caffè (es: 120 = 1.20€)
    
    /* PERCORSI FILE */
    char menu_path[MAX_MENU_PATH];     // Percorso menu.txt
    char fifo_path[MAX_FIFO_PATH];     // Percorso FIFO controllo
    char csv_path[MAX_LOG_PATH];       // Percorso stats.csv
} config_t;
```

**Spiegazione:**
Tutti i parametri della simulazione, letti da file di configurazione. Nessuno hardcoded!

---

## RIGHE 155-172: STRUCT STATISTICHE

```c
typedef struct stats_snapshot {
    long served_users;             // Numero utenti serviti
    long unserved_users;           // Numero utenti non serviti (se rimasti in coda)
    long dishes_served[STATION_COUNT];     // Piatti distribuiti per stazione
    long leftovers[STATION_COUNT];         // Avanzi per stazione
    long wait_count[STATION_COUNT];        // Numero campioni tempo attesa per stazione
    long pauses_total;             // Totale pause effettuate
    long active_workers_total;     // Numero operatori attivi (per stazione/giorno)
    long revenue_cents;            // Ricavo totale in centesimi
    long long wait_ns[STATION_COUNT];      // Tempo attesa totale per stazione (nanosecondi)
} stats_snapshot_t;
```

**Spiegazione:**
Foto istantanea delle statistiche di un giorno o dell'intera simulazione.
- `wait_ns` e `wait_count` permettono calcolo media attese
- Accumulata sia giornalmente che globalmente

---

## RIGHE 174-211: DICHIARAZIONI FUNZIONI

```c
long long get_time_ns(void);                    // Timestamp monotono in ns
void sleep_ns(long long ns);               // Sleep nanosecondi
ipc_sem_t semaphore_create(unsigned value);      // Crea semaforo inizializzato
int semaphore_wait(ipc_sem_t *sem);              // P (decrementa)
int semaphore_post(ipc_sem_t *sem);              // V (incrementa)
bool semaphore_wait_timeout(...);           // P con timeout
void semaphore_destroy(ipc_sem_t sem);           // Distruggi semaforo
void exit_with_errno(const char *msg);           // Exit stampa errno
void exit_with_message(const char *fmt, ...);        // Exit stampa messaggio
int random_in_range(int min, int max);          // Random intero in [min, max]
long long simulate_seconds_to_nanoseconds(...);    // Converte secondi sim a ns reali
int add_random_variation(...);                   // Variazione casuale percentuale
const char *get_station_name(...);             // Converte STATION_xxx a stringa
```

---

---

# INCLUDE/CONFIG.H

```c
#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

void config_defaults(config_t *cfg);           // Carica default
void config_load(const char *path, config_t *cfg);    // Carica da file
void menu_load(const char *path, menu_data_t *menu);  // Carica menu
void config_validate(...);                    // Valida parametri
```

**Spiegazione:**
API pubblica del parser di configurazione. Nasconde i dettagli di parsing.

---

---

# INCLUDE/IPC.H

```c
typedef struct user_state {
    int id;                        // ID utente (0-255)
    int group_id;                  // ID gruppo cui appartiene
    int has_ticket;                // 1 se ha ticket, 0 altrimenti
    int alive;                     // 1 se processo esiste
    int attending_today;           // 1 se in mensa oggi
    int daily_outcome_recorded;    // 1 se già conteggiato (non raddoppiare)
    int dishes_bought;             // Numero piatti comprati oggi
    int wanted_primo;              // Indice primo preferito (-1 = nessuno)
    int wanted_secondo;            // Indice secondo preferito
    int wants_coffee;              // 1 se vuole caffè
    int request_station;           // Stazione richiesta (per comunicare col worker)
    int request_dish;              // Indice piatto richiesto
    int service_successful;                 // 1 se worker ha servito
    int served_dish_index;               // Indice piatto servito (-1 se fallito)
    int total_price_cents;         // Prezzo totale da pagare in centesimi
    long long request_enqueued_ns; // Timestamp enqueue (per calcolo tempo attesa)
    ipc_sem_t served_sem;          // Semaforo privato: worker sblocca quando servito
} user_state_t;
```

**Spiegazione:**
Stato di ogni utente memorizzato in array condiviso. Worker e manager leggono/scrivono questi campi.

```c
typedef struct group_state {
    ipc_sem_t mutex;               // Protegge i contatori
    ipc_sem_t barrier_sem;         // Barriera: blocca finché tutti ready
    int planned_today;             // Quanti membri partecipaNo oggi
    int ready_today;               // Quanti hanno finito stazioni e aspettano cassa
} group_state_t;
```

**Spiegazione:**
Sincronizzazione per gruppi di utenti che mangiano insieme. Aspettano tutti prima di andare in cassa.

```c
typedef struct station_queue {
    ipc_sem_t mutex;               // Protegge l'accesso alla coda
    ipc_sem_t items;               // Semaforo contatore: quanti utenti in coda
    ipc_sem_t slots;               // Semaforo contatore: slot liberi in coda
    ipc_sem_t seat_sem;            // Postazioni fisiche (posti per worker)
    int head;                      // Indice primo elemento coda
    int tail;                      // Indice per aggiungere nuovo elemento
    int count;                     // Numero elementi correnti
    int active_workers;            // Worker attivi oggi su questa stazione
    int assigned_workers;          // Worker assegnati dal manager
    int queue[MAX_QUEUE];          // Buffer circolare ID utenti
} station_queue_t;
```

**Spiegazione:**
Coda circolare bounded buffer thread-safe con 3 semafori:
- `mutex`: sincronizza accesso a head/tail/count
- `items`: conta utenti pronti (>0 per consumer wait)
- `slots`: conta spazi liberi (>0 per producer wait)
- `seat_sem`: postazioni fisiche disponibili per worker

```c
typedef struct inventory_state {
    ipc_sem_t mutex;
    int primi[MAX_DISHES_PER_TYPE];      // Quantità disponibile per tipo di primo
    int secondi[MAX_DISHES_PER_TYPE];    // Quantità disponibile per tipo di secondo
} inventory_state_t;
```

**Spiegazione:**
Magazzino con quantità finita per primi e secondi. Caffè è infinito (non tracciato).

```c
typedef struct shared_state {
    /* CONFIGURAZIONE E MENU */
    config_t cfg;
    menu_data_t menu;
    
    /* SINCRONIZZATORI PRINCIPALI */
    ipc_sem_t init_sem;            // Barriera boot: worker e user segnalano pronto
    ipc_sem_t day_start_sem;       // Barriera giornata: manager rilascia tutti i processi
    ipc_sem_t stats_mutex;         // Protegge statistiche e contatori
    ipc_sem_t ticket_reader_sem;   // Risorsa condivisa lettori ticket
    ipc_sem_t table_sem;           // Tavoli: postazioni per mangiare
    
    /* FLAG GLOBALI */
    volatile sig_atomic_t shutdown;           // 1 = ferma simulazione
    volatile sig_atomic_t day_open;           // 1 = giornata in corso
    volatile sig_atomic_t current_day;        // Giorno attuale (1-based)
    volatile sig_atomic_t started;            // 1 = simulazione iniziata
    volatile sig_atomic_t disorder_active;    // 1 = cassa bloccata
    volatile sig_atomic_t overload_triggered; // 1 = terminare per overload
    
    long long disorder_until_ns;   // Timestamp fine blocco cassa
    
    /* UTENTI */
    int total_users_spawned;       // Numero totale utenti creati (iniziali + dinamici)
    int living_users;              // Numero attuali processi utente vivi
    
    /* WORKER */
    int worker_assignments[MAX_WORKERS];     // Stazione assegnata a ogni worker
    int worker_ever_active[MAX_WORKERS];     // 1 se worker ha lavorato almeno una volta
    int worker_pauses_day[MAX_WORKERS];      // Pause effettuate oggi
    int worker_pauses_total[MAX_WORKERS];    // Pause totali
    
    /* TAVOLI */
    int table_occupied;            // Numero tavoli occupati oggi
    
    /* RISORSE DELLA MENSA */
    station_queue_t stations[STATION_COUNT];  // Code per 4 stazioni
    inventory_state_t inventory;               // Magazzino piatti
    group_state_t groups[MAX_GROUPS];          // Dati sincronizzazione gruppi
    user_state_t users[MAX_USERS];             // Stato di ogni utente
    
    /* STATISTICHE */
    stats_snapshot_t day_stats;    // Statistiche della giornata corrente
    stats_snapshot_t total_stats;  // Statistiche cumulative totali
} shared_state_t;
```

**Spiegazione:**
Struttura GIGANTE condivisa via memoria condivisa tra TUTTI i processi. È il cuore della sincronizzazione.

```c
/* FUNZIONI IPC */
int ipc_create_and_map(size_t size, void **addr);         // Crea shm nuova
int ipc_open_and_map(const char *token, size_t size, void **addr);  // Apri shm esistente
void ipc_unmap_and_close(int shm_id, void *addr, size_t size);      // Stacca shm
void ipc_destroy(int shm_id);                             // Distruggi shm dal sistema
void initialize_shared_state(shared_state_t *shared, ...);            // Inizializza tutti i semafori
void shared_destroy(shared_state_t *shared);              // Distruggi tutti semafori
```

---

---

# INCLUDE/SIMULATION.H

```c
void setup_day(shared_state_t *shared);      // Setup nuovo giorno
void close_day(shared_state_t *shared);      // Chiudi giornata
void refill_inventory_if_needed(...);        // Rifornimenti magazzino
void assign_workers(shared_state_t *shared); // Assegna worker alle stazioni
void print_day_report(...);                  // Stampa report giornaliero/finale
void append_csv_row(...);                    // Salva riga CSV
bool enqueue_user(...);                      // Metti utente in coda stazione
bool dequeue_user_timed(...);                // Estrai utente da coda (con timeout)
void record_wait(...);                       // Registra tempo attesa
void record_dish_served(...);                // Registra piatto distribuito
void record_unserved_user(...);              // Utente non servito
void record_served_user(...);                // Utente servito
void record_pause(...);                      // Registra pausa operatore
void record_revenue(...);                    // Registra ricavo pagamento
```

---

---

# SRC/COMMON.C

## FUNZIONE: semop_retry (RIGHE 3-25)

```c
static int semop_retry(ipc_sem_t semid, short sem_op, const struct timespec *timeout) {
    struct sembuf op = {
        .sem_num = 0,              // Semaforo 0 (ce n'è solo 1 per nostro setup)
        .sem_op = sem_op,          // Operazione: -1 (wait) o +1 (post)
        .sem_flg = 0               // Flag: no SEM_UNDO
    };
    
    for (;;) {                     // Loop infinito fino a successo
        int rc;
        
        if (timeout != NULL) {
            rc = semtimedop(semid, &op, 1, timeout);  // Con timeout
        } else {
            rc = semop(semid, &op, 1);                // Senza timeout
        }
        
        if (rc == 0) {
            return 0;              // Successo
        }
        
        if (errno == EINTR) {
            continue;              // Segnale interrompe, riprova
        }
        
        return -1;                 // Errore reale
    }
}
```

**Spiegazione:**
- Helper per operazioni semaforo atomiche
- Gestisce interrupt (EINTR) da segnali - riprova automaticamente
- Supporta operazioni con/senza timeout
- Usa `struct sembuf` per descrivere operazione

## FUNZIONE: get_time_ns (RIGHE 27-33)

```c
long long get_time_ns(void) {
    struct timespec ts;
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        exit_with_errno("clock_gettime");
    }
    
    // Converti secondi + nanosecondi in nanosecondi totali
    return (long long) ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
```

**Spiegazione:**
- Restituisce timestamp monotono (non retrocede mai)
- In nanosecondi per precisione
- Usato per calcolare time-out e durate simulazioni

## FUNZIONE: sleep_ns (RIGHE 35-46)

```c
void sleep_ns(long long ns) {
    struct timespec ts;
    
    if (ns <= 0) {
        return;                    // Nessuna attesa se <= 0
    }
    
    ts.tv_sec = ns / 1000000000LL;  // Secondi
    ts.tv_nsec = ns % 1000000000LL; // Nanosecondi residui
    
    // Loop finché nanosleep non completa o non arriva segnale
    while (nanosleep(&ts, &ts) != 0 && errno == EINTR) {
        // Se nanosleep ritorna -1 e errno == EINTR,
        // &ts viene riempito con tempo restante
        // Riprova automaticamente
    }
}
```

**Spiegazione:**
- Sleep precisione nanosecondi (simula tempo della mensa)
- Gestisce interrupt: se segnale arriva, riprova il tempo residuo
- Essenziale per avanzamento temporale della simulazione

## FUNZIONE: semaphore_create (RIGHE 48-56)

```c
ipc_sem_t semaphore_create(unsigned value) {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
    // IPC_PRIVATE = nuovo semaforo privato (non condiviso con altri processi)
    // 1 = crea solo 1 semaforo (non array)
    // IPC_CREAT | IPC_EXCL = crea nuovo (fallisce se esiste)
    // 0600 = permessi rw- per owner
    
    union semun arg;
    
    if (semid < 0) {
        exit_with_errno("semget");
    }
    
    arg.val = (int) value;
    
    if (semctl(semid, 0, SETVAL, arg) != 0) {
        exit_with_errno("semctl SETVAL");  // Imposta valore iniziale
    }
    
    return semid;
}
```

**Spiegazione:**
- Crea semaforo System V inizializzato
- IPC_PRIVATE = ID univoco generato dal kernel
- `semctl(..., SETVAL, ...)` imposta valore iniziale

## FUNZIONE: semaphore_wait (RIGHE 58-62)

```c
int semaphore_wait(ipc_sem_t *sem) {
    if (semop_retry(*sem, -1, NULL) != 0) {
        // -1 = decrementa semaforo (P operation)
        // NULL timeout = attesa infinita
        exit_with_errno("semop wait");
    }
    return 0;
}
```

## FUNZIONE: semaphore_post (RIGHE 64-68)

```c
int semaphore_post(ipc_sem_t *sem) {
    if (semop_retry(*sem, 1, NULL) != 0) {
        // +1 = incrementa semaforo (V operation)
        exit_with_errno("semop post");
    }
    return 0;
}
```

## FUNZIONE: semaphore_wait_timeout (RIGHE 70-82)

```c
bool semaphore_wait_timeout(ipc_sem_t *sem, long long timeout_ns) {
    struct timespec timeout;
    
    if (timeout_ns < 0) {
        timeout_ns = 0;            // Negativo = no wait (polling)
    }
    
    timeout.tv_sec = timeout_ns / 1000000000LL;
    timeout.tv_nsec = timeout_ns % 1000000000LL;
    
    if (semop_retry(*sem, -1, &timeout) == 0) {
        return true;               // Semaforo acquisito
    }
    
    if (errno == EAGAIN) {
        return false;              // Timeout scaduto (non acquisito)
    }
    
    exit_with_errno("semtimedop");
    return false;
}
```

**Spiegazione:**
- P con timeout: ritorna false se timeout scade
- Usato da worker per restare reattivo a `day_open` e `shutdown`
- Evita busy-wait, attesa bloccante ma con fallback

## FUNZIONI: exit_with_errno, exit_with_message (RIGHE 84-100)

```c
void exit_with_errno(const char *msg) {
    perror(msg);    // Stampa "msg: <strerror(errno)>"
    exit(EXIT_FAILURE);
}

void exit_with_message(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);   // Printf variadic su stderr
    fputc('\n', stderr);
    va_end(args);
    exit(EXIT_FAILURE);
}
```

## FUNZIONE: random_in_range (RIGHE 102-107)

```c
int random_in_range(int min_value, int max_value) {
    if (max_value <= min_value) {
        return min_value;
    }
    
    // Genera numero casuale in [min_value, max_value] incluso
    return min_value + (rand() % (max_value - min_value + 1));
}
```

## FUNZIONE: simulate_seconds_to_nanoseconds (RIGHE 109-111)

```c
long long simulate_seconds_to_nanoseconds(const config_t *cfg, int sim_seconds) {
    // Converte secondi simulati in nanosecondi reali
    // Formula: (nanoseconds_per_simulated_minute per minuto) * (secondi) / 60
    return ((long long) cfg->nanoseconds_per_simulated_minute * sim_seconds) / 60LL;
}
```

**Esempio:**
- Se `nanoseconds_per_simulated_minute = 50000000` (50 ms per minuto simulato)
- `simulate_seconds_to_nanoseconds(&cfg, 6)` = 50000000 * 6 / 60 = 5000000 ns = 5 ms

## FUNZIONE: add_random_variation (RIGHE 113-122)

```c
int add_random_variation(int base, int percent) {
    // Aggiunge variazione casuale ±percent% attorno a base
    // Es: base=30, percent=50 → valore tra 15 e 45
    
    int delta = (base * percent) / 100;        // 50% di base
    int min_value = base - delta;              // 30 - 15 = 15
    int max_value = base + delta;              // 30 + 15 = 45
    
    if (min_value < 1) {
        min_value = 1;                         // Minimo 1 secondo
    }
    
    return random_in_range(min_value, max_value);   // Random in [min, max]
}
```

## FUNZIONE: get_station_name (RIGHE 124-138)

```c
const char *get_station_name(station_type_t station) {
    switch (station) {
        case STATION_PRIMI:
            return "primi";
        case STATION_SECONDI:
            return "secondi";
        case STATION_COFFEE:
            return "coffee";
        case STATION_CASSA:
            return "cassa";
        default:
            return "unknown";
    }
}
```

---

---

# SRC/CONFIG.C

[CONTINUA...TROPPO LUNGO, CREO IL FILE E CONTINUO]
