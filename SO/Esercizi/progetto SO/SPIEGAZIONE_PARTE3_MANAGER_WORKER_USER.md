# 📚 SPIEGAZIONE PARTE 3: MANAGER.C, WORKER.C, USER.C

---

# SRC/MANAGER.C (Main Orchestrator)

## RIGHE 1-5: HEADER E VARIABILE GLOBALE

```c
#include "simulation.h"
#include "config.h"

static volatile sig_atomic_t g_stop = 0;  // Flag per Ctrl+C, tipo atomico per segnali
```

## FUNZIONE: on_signal (RIGHE 7-10)

```c
static void on_signal(int sig) {
    (void) sig;                  // Ignora argomento (non usato)
    g_stop = 1;                  // Setta flag per arresto elegante
}
```

**Spiegazione:**
Handler segnali. Quando manager riceve SIGINT/SIGTERM, imposta `g_stop = 1` per uscire dal loop.

## FUNZIONE: spawn_child (RIGHE 12-30)

```c
static void spawn_child(const char *path, int shm_id, int id) {
    char shm_buf[16];            // Buffer per stringare shm_id
    char id_buf[16];             // Buffer per stringare id
    
    pid_t pid = fork();          // Crea processo figlio
    
    if (pid < 0) {
        exit_with_errno("fork");       // Fork fallito
    }
    
    if (pid == 0) {              // FIGLIO
        snprintf(shm_buf, sizeof(shm_buf), "%d", shm_id);
        snprintf(id_buf, sizeof(id_buf), "%d", id);
        
        // Esegui programma con parametri
        execl(path, path, shm_buf, id_buf, (char *) NULL);
        
        exit_with_errno("execl");      // Se ritorna, execl fallì
    }
    
    // PADRE continua qui (figlio ha exec)
}
```

**Spiegazione:**
- `fork()` crea processo figlio (copia completa del padre)
- Se pid < 0: errore fork
- Se pid == 0: siamo in FIGLIO (PID univoco)
- Se pid > 0: siamo in PADRE (pid è PID del figlio)
- In FIGLIO: `execl()` rimpiazza immagine processo con nuovo programma
- Parametri passati come argv

## FUNZIONE: ensure_fifo (RIGHE 32-39)

```c
static void ensure_fifo(const char *path) {
    unlink(path);                // Rimuove FIFO vecchia se esiste
    
    if (mkfifo(path, 0666) != 0) {
        exit_with_errno("mkfifo");     // Crea FIFO nuova (named pipe)
    }
}
```

**Spiegazione:**
FIFO (First In First Out = named pipe) usata per comandi da addusers e disorder.
- `mkfifo()` crea file speciale che funziona come coda di messaggi su filesystem
- Permessi 0666 = rw per tutti

## FUNZIONE: assign_groups (RIGHE 41-57)

```c
static void assign_groups(shared_state_t *shared) {
    int user = 0;
    int group = 0;
    
    while (user < shared->cfg.num_users) {
        // Dimensione casuale gruppo: 1 a max_users_per_group
        int size = random_in_range(1, shared->cfg.max_users_per_group);
        
        for (int i = 0; i < size && user < shared->cfg.num_users; ++i) {
            shared->users[user].group_id = group;
            shared->users[user].has_ticket = (random_in_range(0, 99) < 80);  // 80% con ticket
            shared->users[user].alive = 1;
            user++;
        }
        
        group++;  // Prossimo gruppo
    }
    
    shared->living_users = shared->cfg.num_users;
    shared->total_users_spawned = shared->cfg.num_users;
}
```

**Spiegazione:**
Assegna utenti a gruppi che mangiano insieme:
- Gruppo da 1 a 3 persone (configurabile)
- 80% degli utenti hanno ticket (sconto)

## FUNZIONE: wait_for_init (RIGHE 59-65)

```c
static void wait_for_init(shared_state_t *shared) {
    int expected = shared->cfg.num_workers + shared->cfg.num_users;
    
    // Manager attende che tutti i processi segnalino "pronto"
    for (int i = 0; i < expected; ++i) {
        semaphore_wait(&shared->init_sem);  // P: aspetta segnale da un processo
    }
}
```

**Spiegazione:**
Manager aspetta barriera boot. Ogni processo (worker e user) fa `semaphore_post(&shared->init_sem)` quando pronto.

## FUNZIONE: spawn_initial_processes (RIGHE 67-72)

```c
static void spawn_initial_processes(shared_state_t *shared, int shm_id) {
    for (int i = 0; i < shared->cfg.num_workers; ++i) {
        spawn_child("./worker", shm_id, i);
    }
    
    for (int i = 0; i < shared->cfg.num_users; ++i) {
        spawn_child("./user", shm_id, i);
    }
}
```

**Spiegazione:**
Crea tutti i processi iniziali. Passagli shm_id via argv.

## FUNZIONE: spawn_additional_users (RIGHE 74-97)

```c
static void spawn_additional_users(shared_state_t *shared, int shm_id, int count) {
    int start = shared->total_users_spawned;
    int end = start + count;
    
    // Limita al massimo configurato
    if (end > shared->cfg.num_users + shared->cfg.max_dynamic_users) {
        end = shared->cfg.num_users + shared->cfg.max_dynamic_users;
    }
    
    for (int i = start; i < end; ++i) {
        shared->users[i].group_id = i;  // Gruppo mono-utente (non altera gruppi esistenti)
        shared->users[i].has_ticket = (random_in_range(0, 99) < 80);
        shared->users[i].alive = 1;
        
        shared->total_users_spawned += 1;
        shared->living_users += 1;
        
        spawn_child("./user", shm_id, i);
        semaphore_wait(&shared->init_sem);    // Attendi init del nuovo user
    }
}
```

**Spiegazione:**
Aggiunge utenti durante simulazione via comando addusers sulla FIFO.

## FUNZIONE: process_control_line (RIGHE 99-110)

```c
static void process_control_line(shared_state_t *shared, int shm_id, const char *line) {
    int value = 0;
    
    if (strcmp(line, "DISORDER") == 0) {
        shared->disorder_active = 1;
        shared->disorder_until_ns = get_time_ns() + (long long) shared->cfg.stop_duration_seconds * 1000000000LL;
    } else if (sscanf(line, "DISORDER %d", &value) == 1) {
        shared->disorder_active = 1;
        shared->disorder_until_ns = get_time_ns() + (long long) value * 1000000000LL;
    } else if (sscanf(line, "ADD_USERS %d", &value) == 1) {
        spawn_additional_users(shared, shm_id, value);
    }
}
```

**Spiegazione:**
Interpreta comandi ricevuti dalla FIFO:
- "DISORDER" o "DISORDER N": blocca cassa per N secondi
- "ADD_USERS N": aggiunge N utenti

## FUNZIONE: handle_control_commands (RIGHE 112-130)

```c
static void handle_control_commands(shared_state_t *shared, int shm_id, int fifo_fd) {
    char buffer[256];
    
    ssize_t n = read(fifo_fd, buffer, sizeof(buffer) - 1);  // Non-bloccante
    
    if (n <= 0) {
        return;                         // Niente da leggere
    }
    
    buffer[n] = '\0';
    
    char *line = strtok(buffer, "\n");  // Tokenizza righe
    while (line != NULL) {
        process_control_line(shared, shm_id, line);
        line = strtok(NULL, "\n");
    }
}
```

**Spiegazione:**
Legge da FIFO (non-bloccante con O_NONBLOCK) e processa comandi.

## FUNZIONE: main (RIGHE 132-250)

```c
int main(int argc, char **argv) {
    config_t cfg;
    menu_data_t menu;
    shared_state_t *shared;
    int shm_fd;
    int fifo_fd;
    termination_reason_t reason = TERM_TIMEOUT;
    
    if (argc != 2) {
        exit_with_message("uso: manager <percorso_config>");
    }
    
    srand((unsigned) (time(NULL) ^ getpid()));  // Inizializza random
    
    signal(SIGINT, on_signal);                  // Ctrl+C
    signal(SIGTERM, on_signal);                 // Termina gentilmente
    
    config_load(argv[1], &cfg);                 // Carica parametri
    menu_load(cfg.menu_path, &menu);            // Carica menu
    config_validate(&cfg, &menu);               // Valida
    
    ensure_fifo(cfg.fifo_path);                 // Prepara FIFO comandi
    fifo_fd = open(cfg.fifo_path, O_RDWR | O_NONBLOCK);  // Apri non-bloccante
    if (fifo_fd < 0) {
        exit_with_errno("open fifo");
    }
    
    // SETUP IPC
    shm_fd = ipc_create_and_map(sizeof(shared_state_t), (void **) &shared);
    initialize_shared_state(shared, &cfg, &menu);                    // Inizializza semafori
    assign_groups(shared);                               // Assegna utenti a gruppi
    
    // BOOTSTRAP PROCESSI
    spawn_initial_processes(shared, shm_fd);
    wait_for_init(shared);                               // Aspetta tutti pronti
    
    shared->started = 1;                                 // Via! Inizia simulazione
    
    // LOOP PRINCIPALE: per ogni giorno
    for (int day = 1; day <= cfg.sim_duration_days && !g_stop; ++day) {
        long long day_ns = (long long) cfg.day_duration_minutes * cfg.nanoseconds_per_simulated_minute;
        long long start_ns = get_time_ns();
        long long next_refill_ns = simulate_seconds_to_nanoseconds(&cfg, 
                                    add_random_variation(cfg.avg_refill_time * 60, 20));
        
        setup_day(shared);
        
        // LOOP GIORNATA: fino a fine giorno (day_ns nanosecondi reali)
        while (!g_stop && get_time_ns() - start_ns < day_ns) {
            handle_control_commands(shared, shm_fd, fifo_fd);  // Leggi FIFO
            refill_inventory_if_needed(shared, get_time_ns() - start_ns, &next_refill_ns);
            
            // Verifica fine blocco cassa
            if (shared->disorder_active && get_time_ns() >= shared->disorder_until_ns) {
                shared->disorder_active = 0;
            }
            
            sleep_ns(cfg.nanoseconds_per_simulated_minute);  // Avanza di 1 minuto simulato
        }
        
        close_day(shared);
        
        // Verifica overload
        if (shared->stations[STATION_PRIMI].count +
            shared->stations[STATION_SECONDI].count +
            shared->stations[STATION_COFFEE].count +
            shared->stations[STATION_CASSA].count > cfg.overload_threshold) {
            shared->overload_triggered = 1;
            reason = TERM_OVERLOAD;
        }
        
        print_day_report(stdout, shared, day, false, TERM_NONE);
        append_csv_row(shared, day, false, TERM_NONE);
        
        if (shared->overload_triggered) {
            break;  // Esci da loop giorni
        }
    }
    
    // CHIUSURA GRAZIOSA
    if (g_stop && !shared->overload_triggered) {
        reason = TERM_SIGNAL;
    } else if (shared->overload_triggered) {
        reason = TERM_OVERLOAD;
    }
    
    shared->shutdown = 1;  // Segnala chiusura
    
    // Sblocca processi rimasti bloccati
    for (int i = 0; i < shared->cfg.num_workers + shared->living_users; ++i) {
        semaphore_post(&shared->day_start_sem);
    }
    
    // Aspetta tutti i figli
    while (wait(NULL) > 0) {
    }
    
    print_day_report(stdout, shared, shared->current_day, true, reason);
    append_csv_row(shared, shared->current_day, true, reason);
    
    // CLEANUP IPC
    shared_destroy(shared);
    close(fifo_fd);
    unlink(cfg.fifo_path);
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    ipc_destroy(shm_fd);
    
    return 0;
}
```

---

---

# SRC/WORKER.C (Processo Operatore)

## FUNZIONE: mark_worker_active (RIGHE 3-11)

```c
static void mark_worker_active(shared_state_t *shared, int worker_id, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    
    if (!shared->worker_ever_active[worker_id]) {
        shared->worker_ever_active[worker_id] = 1;  // Prima volta
    }
    
    shared->day_stats.active_workers_total += 1;
    shared->stations[station].active_workers += 1;
    
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: mark_worker_inactive (RIGHE 13-19)

```c
static void mark_worker_inactive(shared_state_t *shared, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    
    if (shared->stations[station].active_workers > 0) {
        shared->stations[station].active_workers -= 1;
    }
    
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: take_dish_from_inventory (RIGHE 21-67)

```c
static int take_dish_from_inventory(shared_state_t *shared, station_type_t station, int preferred) {
    int found = -1;
    
    semaphore_wait(&shared->inventory.mutex);
    
    if (station == STATION_PRIMI) {
        // Prova piatto preferito
        if (preferred >= 0 && preferred < shared->menu.primi_count && 
            shared->inventory.primi[preferred] > 0) {
            shared->inventory.primi[preferred]--;
            found = preferred;
        } else {
            // Fallback: primo piatto disponibile
            for (int i = 0; i < shared->menu.primi_count; ++i) {
                if (shared->inventory.primi[i] > 0) {
                    shared->inventory.primi[i]--;
                    found = i;
                    break;
                }
            }
        }
    } else if (station == STATION_SECONDI) {
        // Stessa logica per secondi
        if (preferred >= 0 && preferred < shared->menu.secondi_count && 
            shared->inventory.secondi[preferred] > 0) {
            shared->inventory.secondi[preferred]--;
            found = preferred;
        } else {
            for (int i = 0; i < shared->menu.secondi_count; ++i) {
                if (shared->inventory.secondi[i] > 0) {
                    shared->inventory.secondi[i]--;
                    found = i;
                    break;
                }
            }
        }
    } else if (station == STATION_COFFEE) {
        // Caffè illimitato, ritorna indice richiesto
        found = preferred >= 0 ? preferred : 0;
    }
    
    semaphore_post(&shared->inventory.mutex);
    
    return found;
}
```

**Spiegazione:**
Logica adattativa: se piatto richiesto non c'è, prende alternativa stesso tipo.
- Primi/Secondi: quantità finita, tracciata
- Caffè: infinito

## FUNZIONE: maybe_pause (RIGHE 69-104)

```c
static void maybe_pause(shared_state_t *shared, int worker_id, station_type_t station, int *pauses_done) {
    // Probabilità bassa di pausa, limitata a max_pauses_per_day per giorno
    if (*pauses_done >= shared->cfg.max_pauses_per_day || random_in_range(0, 99) > 4) {
        return;  // ~96% probabilità di non fare pausa
    }
    
    semaphore_wait(&shared->stats_mutex);
    
    // Non staccare se unico operatore alla stazione
    if (shared->stations[station].active_workers <= 1) {
        semaphore_post(&shared->stats_mutex);
        return;
    }
    
    shared->stations[station].active_workers -= 1;
    semaphore_post(&shared->stats_mutex);
    
    // Libera postazione per subentrante durante pausa
    semaphore_post(&shared->stations[station].seat_sem);
    
    record_pause(shared, worker_id);
    
    // Dormi (pausa reale)
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, 
             add_random_variation(shared->cfg.avg_pause_seconds, 40)));
    
    // Riprendi postazione (con timeout per restare reattivo)
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->stations[station].seat_sem, 
            simulate_seconds_to_nanoseconds(&shared->cfg, 1))) {
            break;  // Postazione disponibile
        }
    }
    
    if (!shared->day_open || shared->shutdown) {
        return;  // Giornata finita durante pausa
    }
    
    // Ritorna attivo
    semaphore_wait(&shared->stats_mutex);
    shared->stations[station].active_workers += 1;
    semaphore_post(&shared->stats_mutex);
    
    (*pauses_done)++;
}
```

## FUNZIONE: serve_request (RIGHE 106-145)

```c
static void serve_request(shared_state_t *shared, int user_id, station_type_t station) {
    user_state_t *user = &shared->users[user_id];
    long long wait_ns = get_time_ns() - user->request_enqueued_ns;
    int service_seconds = 1;
    int served_dish_index = -1;
    
    // Tempo servizio dipende da stazione (con variazione casuale specifica)
    if (station == STATION_PRIMI) {
        service_seconds = add_random_variation(shared->cfg.avg_service_time_primi_seconds, 50);
        served_dish_index = take_dish_from_inventory(shared, station, user->request_dish);
    } else if (station == STATION_SECONDI) {
        service_seconds = add_random_variation(shared->cfg.avg_service_time_secondi_seconds, 50);
        served_dish_index = take_dish_from_inventory(shared, station, user->request_dish);
    } else if (station == STATION_COFFEE) {
        service_seconds = add_random_variation(shared->cfg.avg_service_time_coffee_seconds, 80);
        served_dish_index = take_dish_from_inventory(shared, station, user->request_dish);
    } else if (station == STATION_CASSA) {
        service_seconds = add_random_variation(shared->cfg.avg_service_time_cassa_seconds, 20);
        served_dish_index = 1;  // Sempre successo pagamento
        record_revenue(shared, user->total_price_cents);  // Incassa
    }
    
    // Simula tempo servizio
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, service_seconds));
    
    // Comunica risultato all'utente
    user->service_successful = (served_dish_index >= 0);
    user->served_dish_index = served_dish_index;
    
    if (user->service_successful && station != STATION_CASSA) {
        record_dish_served(shared, station);  // Incrementa statistiche
    }
    
    // Registra solo il tempo passato in coda, misurato prima del servizio
    record_wait(shared, station, wait_ns);
    
    // SBLOCCA utente
    semaphore_post(&user->served_sem);
}
```

## FUNZIONE: main (RIGHE 147-215)

```c
int main(int argc, char **argv) {
    int shm_fd;
    shared_state_t *shared;
    int worker_id;
    
    srand((unsigned) (time(NULL) ^ getpid()));
    
    if (argc != 3) {
        exit_with_message("uso: worker <id_shm> <id_worker>");
    }
    
    worker_id = atoi(argv[2]);
    shm_fd = ipc_open_and_map(argv[1], sizeof(shared_state_t), (void **) &shared);
    
    // Segnala al manager "sono pronto"
    semaphore_post(&shared->init_sem);
    
    // LOOP PRINCIPALE: per ogni giorno
    while (!shared->shutdown) {
        int pauses_done = 0;
        int user_id;
        station_type_t station;
        long long wait_tick_ns;
        
        // ATTESA INIZIO GIORNATA
        semaphore_wait(&shared->day_start_sem);
        
        if (shared->shutdown) {
            break;
        }
        
        // Assegnazione della stazione
        station = (station_type_t) shared->worker_assignments[worker_id];
        wait_tick_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
        
        // ATTESA POSTAZIONE LIBERA
        // Semaforo seat_sem conta postazioni disponibili
        while (shared->day_open && !shared->shutdown) {
            if (semaphore_wait_timeout(&shared->stations[station].seat_sem, wait_tick_ns)) {
                break;  // Postazione acquisita
            }
        }
        
        if (!shared->day_open || shared->shutdown) {
            continue;
        }
        
        // REGISTRA ATTIVAZIONE
        mark_worker_active(shared, worker_id, station);
        
        // LOOP SERVIZIO: servi fino fine giornata
        while (shared->day_open && !shared->shutdown) {
            // Se cassa bloccata da DISORDER, fai nulla
            if (station == STATION_CASSA && shared->disorder_active && 
                get_time_ns() < shared->disorder_until_ns) {
                sleep_ns(wait_tick_ns);
                continue;
            }
            
            // Prendi utente dalla coda con timeout
            if (dequeue_user_timed(&shared->stations[station], &user_id, wait_tick_ns)) {
                serve_request(shared, user_id, station);  // Servi
            } else {
                maybe_pause(shared, worker_id, station, &pauses_done);  // Oppure pausa
            }
        }
        
        // FINE GIORNATA
        mark_worker_inactive(shared, station);
        semaphore_post(&shared->stations[station].seat_sem);  // Libera postazione
    }
    
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    return 0;
}
```

**Ciclo giornaliero worker:**
1. Aspetta inizio giornata (barriera)
2. Aspetta postazione libera (seat_sem)
3. Marca come attivo
4. Loop: estrai utente da coda, servi (o fai pausa)
5. Fine giornata: marca inattivo, libera postazione

---

---

# SRC/USER.C (Processo Utente/Cliente)

## FUNZIONE: choose_random_menu_index (RIGHE 3-5)

```c
static int choose_random_menu_index(int count) {
    return random_in_range(0, count - 1);  // Sceglie piatto casuale
}
```

## FUNZIONE: choose_next_station (RIGHE 7-24)

```c
static station_type_t choose_next_station(shared_state_t *shared, 
                                         bool need_primo, bool need_secondo, bool need_coffee) {
    station_type_t choice = STATION_PRIMI;
    int best_count = INT_MAX;
    
    // Greedy: sceglie coda più corta tra quelle che servono ancora
    if (need_primo && shared->stations[STATION_PRIMI].count < best_count) {
        best_count = shared->stations[STATION_PRIMI].count;
        choice = STATION_PRIMI;
    }
    
    if (need_secondo && shared->stations[STATION_SECONDI].count < best_count) {
        best_count = shared->stations[STATION_SECONDI].count;
        choice = STATION_SECONDI;
    }
    
    if (need_coffee && shared->stations[STATION_COFFEE].count < best_count) {
        best_count = shared->stations[STATION_COFFEE].count;
        choice = STATION_COFFEE;
    }
    
    return choice;
}
```

**Spiegazione:**
Strategia coda più corta (shortest queue first): minimizza attese.

## FUNZIONE: reset_user_day (RIGHE 26-37)

```c
static void reset_user_day(user_state_t *user) {
    user->attending_today = 0;
    user->daily_outcome_recorded = 0;
    user->dishes_bought = 0;
    user->wanted_primo = -1;
    user->wanted_secondo = -1;
    user->wants_coffee = 0;
    user->total_price_cents = 0;
}
```

**Spiegazione:**
Azzera stato giornaliero per nuovo giorno. Mantiene id, group_id, has_ticket (persistenti).

## FUNZIONE: wait_for_service (RIGHE 39-48)

```c
static bool wait_for_service(shared_state_t *shared, user_state_t *user) {
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    
    // Attendi worker sblocchi served_sem con timeout periodico
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&user->served_sem, timeout_ns)) {
            return true;  // Worker ha servito
        }
    }
    
    return false;  // Fine giornata o shutdown
}
```

## FUNZIONE: request_station (RIGHE 50-66)

```c
static bool request_station(shared_state_t *shared, user_state_t *user, 
                           station_type_t station, int preferred_dish) {
    // Prepara richiesta
    user->request_station = station;
    user->request_dish = preferred_dish;
    user->service_successful = 0;
    user->served_dish_index = -1;
    user->request_enqueued_ns = get_time_ns();  // Timestamp per misurare attesa
    
    // Entra in coda
    if (!enqueue_user(&shared->stations[station], user->id)) {
        return false;
    }
    
    // Aspetta worker serva
    if (!wait_for_service(shared, user)) {
        return false;
    }
    
    return user->service_successful;  // Risultato della richiesta
}
```

## FUNZIONE: group_arrive (RIGHE 68-75)

```c
static void group_arrive(shared_state_t *shared, int group_id) {
    group_state_t *group = &shared->groups[group_id];
    
    semaphore_wait(&group->mutex);
    group->planned_today += 1;  // Uno più membro del gruppo oggi
    semaphore_post(&group->mutex);
}
```

## FUNZIONE: group_wait_ready (RIGHE 77-101)

```c
static bool group_wait_ready(shared_state_t *shared, int group_id) {
    int releases = 0;
    group_state_t *group = &shared->groups[group_id];
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    
    semaphore_wait(&group->mutex);
    group->ready_today += 1;  // Uno più arrivato alle stazioni
    
    if (group->ready_today == group->planned_today) {
        // ULTIMO MEMBRO: sblocca tutti
        releases = group->planned_today;
    }
    
    semaphore_post(&group->mutex);
    
    // Rilascia barrier per tutti i member
    while (releases-- > 0) {
        semaphore_post(&group->barrier_sem);
    }
    
    // Aspetta tutti gli altri (barriera), ma resta reattivo a fine giornata/shutdown
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&group->barrier_sem, timeout_ns)) {
            return true;
        }
    }
    return false;
}
```

**Spiegazione:**
Barriera di sincronizzazione:
- Ogni membro aspetta barrier_sem
- Ultimo membro arrivato rilascia N volte (per svegliare tutti)
- Tutti svegli, pronti per cassa
- L’attesa è temporizzata: se la giornata si chiude o arriva shutdown, l’utente non resta bloccato indefinitamente.

## FUNZIONE: maybe_ticket (RIGHE 103-119)

```c
static bool maybe_ticket(shared_state_t *shared, user_state_t *user) {
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    if (!user->has_ticket) {
        return true;  // Niente da fare
    }
    
    // Risorsa condivisa: lettore ticket con capacità limitata
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->ticket_reader_sem, timeout_ns)) {
            sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, 2));
            semaphore_post(&shared->ticket_reader_sem);
            return true;
        }
    }
    return false;
}
```

Anche il lettore ticket usa attesa con timeout: in caso di chiusura giornata l’utente può uscire dal percorso invece di restare appeso sul semaforo.

## FUNZIONE: consume_meal_at_table (RIGHE 109-140)

```c
static void consume_meal_at_table(shared_state_t *shared, user_state_t *user) {
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    
    // Aspetta tavolo libero
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->table_sem, timeout_ns)) {
            break;  // Tavolo acquisito
        }
    }
    
    if (!shared->day_open || shared->shutdown) {
        return;  // Giornata finita, non mangio
    }
    
    // Registra occupazione
    semaphore_wait(&shared->stats_mutex);
    shared->table_occupied += 1;
    semaphore_post(&shared->stats_mutex);
    
    // Mangia (tempo proporzionale piatti)
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, user->dishes_bought * 12));
    
    // Finisce di mangiare
    semaphore_wait(&shared->stats_mutex);
    if (shared->table_occupied > 0) {
        shared->table_occupied -= 1;
    }
    semaphore_post(&shared->stats_mutex);
    
    // Libera tavolo
    semaphore_post(&shared->table_sem);
}
```

## FUNZIONE: main (RIGHE 142-237)

```c
int main(int argc, char **argv) {
    int shm_fd;
    int user_id;
    shared_state_t *shared;
    user_state_t *user;
    
    srand((unsigned) (time(NULL) ^ getpid()));
    
    if (argc != 3) {
        exit_with_message("uso: user <id_shm> <id_utente>");
    }
    
    user_id = atoi(argv[2]);
    shm_fd = ipc_open_and_map(argv[1], sizeof(shared_state_t), (void **) &shared);
    user = &shared->users[user_id];
    
    // Segnala "pronto"
    semaphore_post(&shared->init_sem);
    
    // LOOP PRINCIPALE: per ogni giorno
    while (!shared->shutdown) {
        // ATTESA INIZIO GIORNATA
        semaphore_wait(&shared->day_start_sem);
        
        if (shared->shutdown) {
            break;
        }
        
        // Reset per nuovo giorno
        reset_user_day(user);
        
        // ~20% probabilità di non venire oggi
        if (random_in_range(0, 99) < 20) {
            continue;
        }
        
        user->attending_today = 1;
        
        // SCELTA MENU GIORNALIERO
        user->wanted_primo = choose_random_menu_index(shared->menu.primi_count);
        user->wanted_secondo = choose_random_menu_index(shared->menu.secondi_count);
        user->wants_coffee = random_in_range(0, 99) < 70;  // 70% probabilità
        
        // SEGNALA ARRIVO AL GRUPPO
        group_arrive(shared, user->group_id);
        
        // RACCOLTA PIATTI: visita stazioni in ordine prioritario
        {
            bool need_primo = true;
            bool need_secondo = true;
            bool need_coffee = user->wants_coffee;
            int wanted_coffee = need_coffee ? choose_random_menu_index(shared->menu.coffee_count) : -1;
            
            while (need_primo || need_secondo || need_coffee) {
                station_type_t station = choose_next_station(shared, need_primo, need_secondo, need_coffee);
                int preferred = -1;
                
                if (station == STATION_PRIMI) {
                    preferred = user->wanted_primo;
                    need_primo = false;
                } else if (station == STATION_SECONDI) {
                    preferred = user->wanted_secondo;
                    need_secondo = false;
                } else {
                    preferred = wanted_coffee;
                    need_coffee = false;
                }
                
                // Richiedi piatto
                if (request_station(shared, user, station, preferred)) {
                    user->dishes_bought += 1;
                    
                    // Aggiungi prezzo
                    if (station == STATION_PRIMI) {
                        user->total_price_cents += shared->cfg.price_primi;
                    } else if (station == STATION_SECONDI) {
                        user->total_price_cents += shared->cfg.price_secondi;
                    } else if (station == STATION_COFFEE) {
                        user->total_price_cents += shared->cfg.price_coffee;
                    }
                }
            }
        }
        
        // SINCRONIZZAZIONE GRUPPO: aspetta tutti i membri, con uscita su fine giornata
        if (!group_wait_ready(shared, user->group_id)) {
            record_unserved_user(shared, user);
            continue;
        }
        
        // Se non ha nulla, non va in cassa
        if (user->dishes_bought == 0) {
            record_unserved_user(shared, user);
            continue;
        }
        
        // LETTORE TICKET (se ha ticket)
        if (!maybe_ticket(shared, user)) {
            record_unserved_user(shared, user);
            continue;
        }
        
        // APPLICA SCONTO SE TICKET
        if (user->has_ticket) {
            user->total_price_cents -= (user->total_price_cents * shared->cfg.ticket_discount_percent) / 100;
        }
        
        // PAGAMENTO IN CASSA
        if (!request_station(shared, user, STATION_CASSA, 0)) {
            record_unserved_user(shared, user);
            continue;
        }
        
        // CONSUMO PASTO
        record_served_user(shared, user);
        consume_meal_at_table(shared, user);
    }
    
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    return 0;
}
```

**Ciclo giornaliero user:**
1. Aspetta inizio giornata
2. Decidi se venire (80%)
3. Sceglie menu
4. Visita stazioni in ordine coda corta
5. Sincronizza col gruppo
6. Lettore ticket (se ha ticket)
7. Paga in cassa
8. Mangia ai tavoli

---

---

# SRC/ADDUSERS.C

```c
#include "common.h"

int main(int argc, char **argv) {
    int fd;
    char line[64];
    
    if (argc != 3) {
        exit_with_message("uso: addusers <percorso_fifo> <numero_utenti>");
    }
    
    fd = open(argv[1], O_WRONLY);  // Apri FIFO per scrittura
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
```

**Spiegazione:**
Utility esterna: scrive comando sulla FIFO per aggiungere utenti.

---

---

# SRC/DISORDER.C

```c
#include "common.h"

int main(int argc, char **argv) {
    int fd;
    static const char line[] = "DISORDER\n";
    
    if (argc != 2) {
        exit_with_message("uso: disorder <percorso_fifo>");
    }
    
    fd = open(argv[1], O_WRONLY);  // Apri FIFO per scrittura
    if (fd < 0) {
        exit_with_errno("open fifo");
    }
    
    if (write(fd, line, strlen(line)) < 0) {
        exit_with_errno("write fifo");
    }
    
    close(fd);
    return 0;
}
```

**Spiegazione:**
Utility esterna: scrive comando sulla FIFO per bloccare la cassa.

---

## 🎯 RIEPILOGO FINALE

### Flusso Completo Simulazione:

1. **Manager avvia**: Carica config, crea memoria condivisa e semafori
2. **Manager spawn processi**: Crea worker e user via fork+execl
3. **Tutti i processi**: Segnalano "pronto" al manager
4. **Manager avvia simulazione**: Loop giorni
5. **setup_day()**: Reset statistiche, assegna worker, rifornisce magazzino
6. **Durante il giorno**: Worker servono, User richiedono
7. **close_day()**: Conteggia utenti rimasti, calcola avanzi
8. **Fine giornata**: Stampa report, verifica overload
9. **Fine simulazione**: Cleanup IPC, exit

### Sincronizzazione Principale:

- **init_sem**: Barriera bootstrap
- **day_start_sem**: Barriera inizio giornata
- **stats_mutex**: Protegge statistiche
- **seat_sem**: Postazioni fisiche per worker
- **served_sem**: Sblocca utente quando servito
- **barrier_sem**: Sincronizza gruppi prima cassa

### Gestione Tempo:

- **nanoseconds_per_simulated_minute**: Nanosecondi reali per minuto simulato
- **sleep_ns()**: Attesa nanosecondi
- **semaphore_wait_timeout()**: Timeout brevi per reattività

---

✅ **FINE SPIEGAZIONE COMPLETA**
