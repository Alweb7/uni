# 📚 SPIEGAZIONE PARTE 2: CONFIG.C, IPC.C, SIMULATION.C

---

# SRC/CONFIG.C

## FUNZIONE: trim (RIGHE 3-16)

```c
static void trim(char *s) {
    size_t len;
    char *start = s;
    
    // Salta spazi iniziali
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }
    
    // Sposta contenuto utile all'inizio
    if (start != s) {
        memmove(s, start, strlen(start) + 1);  // strlen(start)+1 include \0
    }
    
    // Rimuovi spazi finali
    len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}
```

**Spiegazione:**
Helper per parsing configurazione. Rimuove spazi da inizio e fine stringa.

## FUNZIONE: parse_int_or_die (RIGHE 18-33)

```c
static int parse_int_or_die(const char *key, const char *value) {
    char *end = NULL;
    long parsed;
    
    if (value[0] == '\0') {
        exit_with_message("config: missing value for %s", key);
    }
    
    errno = 0;
    parsed = strtol(value, &end, 10);  // Converti stringa a long
    
    // Controlla errori: overflow, formato non valido, non numerico
    if (errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX || 
        end == value || *end != '\0') {
        exit_with_message("config: invalid integer for %s: %s", key, value);
    }
    
    return (int) parsed;
}
```

## FUNZIONE: copy_string_field (RIGHE 35-43)

```c
static void copy_string_field(char *dst, size_t dst_size, const char *field_name, const char *value) {
    if (value[0] == '\0') {
        exit_with_message("config: missing value for %s", field_name);
    }
    
    if (strlen(value) >= dst_size) {
        exit_with_message("config: value too long for %s", field_name);
    }
    
    snprintf(dst, dst_size, "%s", value);  // Copia sicura
}
```

## FUNZIONE: config_defaults (RIGHE 45-91)

```c
void config_defaults(config_t *cfg) {
    memset(cfg, 0, sizeof(*cfg));  // Azzera tutto
    
    // Impostazioni default ragionevoli
    cfg->sim_duration_days = 3;
    cfg->nanoseconds_per_simulated_minute = 50000000;   // 50 ms per minuto simulato
    cfg->day_duration_minutes = 120;
    cfg->num_workers = 6;
    cfg->num_users = 18;
    // ...e così via...
}
```

**Spiegazione:**
Inizializza configurazione con valori sensati. Se file non specifica un parametro, usa questo default.

## FUNZIONE: set_int_field (RIGHE 93-152)

```c
static bool set_int_field(config_t *cfg, const char *key, const char *value) {
#define SET_INT(name) \
    if (strcmp(key, #name) == 0) { \
        cfg->name = parse_int_or_die(#name, value); \
        return true; \
    }
    SET_INT(sim_duration_days)
    SET_INT(nanoseconds_per_simulated_minute)
    // ...
#undef SET_INT
    return false;
}
```

**Spiegazione:**
Macro che mappachiave testuale (es: "sim_duration_days") al campo struct e lo popola.
- Usa `#name` per stringify nome campo
- Elimina copy-paste di tanti if/strcmp

## FUNZIONE: config_load (RIGHE 154-198)

```c
void config_load(const char *path, config_t *cfg) {
    FILE *fp;
    char line[256];
    int line_no = 0;
    
    config_defaults(cfg);  // Parte dai default
    
    fp = fopen(path, "r");
    if (fp == NULL) {
        exit_with_errno("fopen config");
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *eq;
        bool known = false;
        
        line_no++;
        trim(line);
        
        // Salta righe vuote e commenti
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        
        // Cerca separatore '='
        eq = strchr(line, '=');
        if (eq == NULL) {
            exit_with_message("config: invalid line %d, expected key=value", line_no);
        }
        
        *eq = '\0';        // Termina chiave
        trim(line);        // Trim chiave
        trim(eq + 1);      // Trim valore
        
        // Tenta parsare come campo intero
        known = set_int_field(cfg, line, eq + 1);
        
        // Poi prova campi stringa speciali
        if (strcmp(line, "menu_path") == 0) {
            copy_string_field(cfg->menu_path, sizeof(cfg->menu_path), "menu_path", eq + 1);
            known = true;
        } else if (strcmp(line, "fifo_path") == 0) {
            copy_string_field(cfg->fifo_path, sizeof(cfg->fifo_path), "fifo_path", eq + 1);
            known = true;
        } else if (strcmp(line, "csv_path") == 0) {
            copy_string_field(cfg->csv_path, sizeof(cfg->csv_path), "csv_path", eq + 1);
            known = true;
        }
        
        if (!known) {
            exit_with_message("config: unknown key at line %d: %s", line_no, line);
        }
    }
    
    fclose(fp);
}
```

**Spiegazione:**
Parser configurazione line-by-line:
1. Leggi riga
2. Skip commenti e vuote
3. Dividi su '='
4. Trim whitespace
5. Parsa chiave e assegna valore

## FUNZIONE: parse_menu_line (RIGHE 200-245)

```c
static void parse_menu_line(menu_data_t *menu, char *line) {
    char *colon = strchr(line, ':');
    char *token;
    int *count = NULL;
    dish_t *target = NULL;
    
    if (colon == NULL) {
        return;  // Riga formato errato, skip
    }
    
    *colon = '\0';
    trim(line);      // Nome sezione (PRIMI, SECONDI, COFFEE)
    trim(colon + 1); // Lista piatti separati da virgola
    
    // Seleziona array target basato su sezione
    if (strcmp(line, "PRIMI") == 0) {
        count = &menu->primi_count;
        target = menu->primi;
    } else if (strcmp(line, "SECONDI") == 0) {
        count = &menu->secondi_count;
        target = menu->secondi;
    } else if (strcmp(line, "COFFEE") == 0) {
        count = &menu->coffee_count;
        target = menu->coffee;
    } else {
        return;  // Sezione sconosciuta
    }
    
    // Tokenizza piatti separati da virgola
    token = strtok(colon + 1, ",");
    while (token != NULL && *count < MAX_DISHES_PER_TYPE) {
        trim(token);
        snprintf(target[*count].name, sizeof(target[*count].name), "%s", token);
        (*count)++;
        token = strtok(NULL, ",");
    }
}
```

**Spiegazione:**
Parser menu.txt formato:
```
PRIMI: Pasta al Forno, Risotto Milanese
SECONDI: Polpettone, Pesce al Forno
COFFEE: Espresso, Cappuccino, Decaffeinato, Ginseng
```

## FUNZIONE: menu_load (RIGHE 247-262)

```c
void menu_load(const char *path, menu_data_t *menu) {
    FILE *fp;
    char line[256];
    
    memset(menu, 0, sizeof(*menu));
    
    fp = fopen(path, "r");
    if (fp == NULL) {
        exit_with_errno("fopen menu");
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        trim(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        parse_menu_line(menu, line);  // Delega parsing a funzione helper
    }
    
    fclose(fp);
}
```

## FUNZIONE: config_validate (RIGHE 264-350)

```c
void config_validate(const config_t *cfg, const menu_data_t *menu) {
    // Controlli che impediscono simulazione impossibile
    
    if (cfg->sim_duration_days <= 0) {
        exit_with_message("sim_duration_days must be > 0");
    }
    
    if (cfg->num_workers < STATION_COUNT) {
        exit_with_message("num_workers must be >= %d (una per stazione)", STATION_COUNT);
    }
    
    if ((long long) cfg->num_users + cfg->max_dynamic_users > MAX_USERS) {
        exit_with_message("num_users + max_dynamic_users must be <= %d", MAX_USERS);
    }
    
    // ...altri controlli...
}
```

---

---

# SRC/IPC.C

## FUNZIONE: ipc_create_and_map (RIGHE 5-18)

```c
int ipc_create_and_map(size_t size, void **addr) {
    // Crea nuovo segmento memoria condivisa
    int shm_id = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0600);
    // IPC_PRIVATE: ID univoco privato
    // IPC_CREAT: crea se non esiste
    // IPC_EXCL: fallisce se esiste (vogliamo creazione)
    // 0600: permessi rw- owner
    
    if (shm_id < 0) {
        exit_with_errno("shmget create");
    }
    
    // Attacca segmento a spazio indirizzo corrente
    *addr = shmat(shm_id, NULL, 0);
    // NULL = kernel sceglie indirizzo
    // 0 = read+write
    
    if (*addr == (void *) -1) {
        exit_with_errno("shmat create");
    }
    
    return shm_id;  // ID per accesso futuro
}
```

**Spiegazione:**
Manager chiama questa una volta. Crea la struttura gigante `shared_state_t` e la attacca.

## FUNZIONE: ipc_open_and_map (RIGHE 20-35)

```c
int ipc_open_and_map(const char *token, size_t size, void **addr) {
    long shm_id;
    char *end = NULL;
    
    // Token è string ID semaforo passato come argv (es: "12345")
    errno = 0;
    shm_id = strtol(token, &end, 10);  // Converti string a numero
    
    if (errno != 0 || token[0] == '\0' || end == token || *end != '\0' || 
        shm_id < 0 || shm_id > INT_MAX) {
        exit_with_message("invalid shared memory token: %s", token);
    }
    
    // Attacca segmento esistente (NO IPC_CREAT!)
    *addr = shmat((int) shm_id, NULL, 0);
    
    if (*addr == (void *) -1) {
        exit_with_errno("shmat open");
    }
    
    return (int) shm_id;
}
```

**Spiegazione:**
Worker e User chiamano questo. Ricevono ID via argv, lo usano per connettersi alla stessa memoria.

## FUNZIONE: ipc_unmap_and_close (RIGHE 37-42)

```c
void ipc_unmap_and_close(int shm_id, void *addr, size_t size) {
    (void) shm_id;      // Non usato, ma prendiamo per API consistency
    (void) size;
    
    if (addr != NULL && addr != (void *) -1) {
        shmdt(addr);    // Detach segmento da spazio processi
    }
}
```

## FUNZIONE: ipc_destroy (RIGHE 44-46)

```c
void ipc_destroy(int shm_id) {
    shmctl(shm_id, IPC_RMID, NULL);  // Rimuovi da sistema operativo
}
```

**Spiegazione:**
Manager chiama al termine. Rimuove la memoria condivisa dal kernel.

## FUNZIONE: init_sem_or_die (RIGHE 48-50)

```c
static void init_sem_or_die(ipc_sem_t *sem, unsigned value) {
    *sem = semaphore_create(value);
}
```

## FUNZIONE: initialize_shared_state (RIGHE 52-103)

```c
void initialize_shared_state(shared_state_t *shared, const config_t *cfg, const menu_data_t *menu) {
    int i;
    
    memset(shared, 0, sizeof(*shared));
    
    shared->cfg = *cfg;    // Copia configurazione
    shared->menu = *menu;  // Copia menu
    
    // SINCRONIZZATORI PRINCIPALI
    init_sem_or_die(&shared->init_sem, 0);       // Barriera boot
    init_sem_or_die(&shared->day_start_sem, 0);  // Barriera giornata
    init_sem_or_die(&shared->stats_mutex, 1);    // Mutex stats (inizialmente libero)
    init_sem_or_die(&shared->ticket_reader_sem, (unsigned) cfg->ticket_reader_capacity);
    init_sem_or_die(&shared->table_sem, (unsigned) cfg->num_table_seats);
    
    // INVENTARIO
    init_sem_or_die(&shared->inventory.mutex, 1);
    
    // STAZIONI (4)
    for (i = 0; i < STATION_COUNT; ++i) {
        init_sem_or_die(&shared->stations[i].mutex, 1);      // Mutex coda
        init_sem_or_die(&shared->stations[i].items, 0);      // Contatore utenti in coda
        init_sem_or_die(&shared->stations[i].slots, MAX_QUEUE);  // Slot liberi
    }
    
    // POSTAZIONI per ogni stazione
    init_sem_or_die(&shared->stations[STATION_PRIMI].seat_sem, (unsigned) cfg->num_worker_seats_primi);
    init_sem_or_die(&shared->stations[STATION_SECONDI].seat_sem, (unsigned) cfg->num_worker_seats_secondi);
    init_sem_or_die(&shared->stations[STATION_COFFEE].seat_sem, (unsigned) cfg->num_worker_seats_coffee);
    init_sem_or_die(&shared->stations[STATION_CASSA].seat_sem, (unsigned) cfg->num_worker_seats_cassa);
    
    // GRUPPI
    for (i = 0; i < MAX_GROUPS; ++i) {
        init_sem_or_die(&shared->groups[i].mutex, 1);         // Protegge contatori
        init_sem_or_die(&shared->groups[i].barrier_sem, 0);   // Barriera sincronizzazione
    }
    
    // UTENTI
    for (i = 0; i < MAX_USERS; ++i) {
        shared->users[i].id = i;
        init_sem_or_die(&shared->users[i].served_sem, 0);    // Privato per ogni utente
    }
}
```

**Spiegazione:**
Crea TUTTI i semafori della simulazione una sola volta nel manager.

## FUNZIONE: shared_destroy (RIGHE 105-139)

```c
void shared_destroy(shared_state_t *shared) {
    int i;
    
    // Distruggi tutti i semafori creati in initialize_shared_state
    semaphore_destroy(shared->init_sem);
    semaphore_destroy(shared->day_start_sem);
    semaphore_destroy(shared->stats_mutex);
    // ...
    
    for (i = 0; i < STATION_COUNT; ++i) {
        semaphore_destroy(shared->stations[i].mutex);
        semaphore_destroy(shared->stations[i].items);
        semaphore_destroy(shared->stations[i].slots);
        semaphore_destroy(shared->stations[i].seat_sem);
    }
    
    // ...pulire tutti...
}
```

---

---

# SRC/SIMULATION.C

## FUNZIONE: accumulate_statistics (RIGHE 3-18)

```c
static void accumulate_statistics(stats_snapshot_t *dst, const stats_snapshot_t *src) {
    int i;
    
    dst->served_users += src->served_users;        // Accumula utenti serviti
    dst->unserved_users += src->unserved_users;
    dst->pauses_total += src->pauses_total;
    dst->active_workers_total += src->active_workers_total;
    dst->revenue_cents += src->revenue_cents;
    
    // Accumula per ogni stazione
    for (i = 0; i < STATION_COUNT; ++i) {
        dst->dishes_served[i] += src->dishes_served[i];
        dst->leftovers[i] += src->leftovers[i];
        dst->wait_count[i] += src->wait_count[i];
        dst->wait_ns[i] += src->wait_ns[i];
    }
}
```

**Spiegazione:**
Aggiunge statistiche giornaliere al totale cumulativo. Usato da `close_day()`.

## FUNZIONE: enqueue_user (RIGHE 87-108)

```c
bool enqueue_user(station_queue_t *queue, int user_id) {
    bool ok = false;
    
    // Producer-consumer bounded buffer pattern
    // Producer (user) attende slot libero
    semaphore_wait(&queue->slots);          // Slot -=1, blocca se 0
    
    semaphore_wait(&queue->mutex);          // Entra sezione critica
    
    if (queue->count < MAX_QUEUE) {
        queue->queue[queue->tail] = user_id;
        queue->tail = (queue->tail + 1) % MAX_QUEUE;
        queue->count++;
        ok = true;
    }
    
    semaphore_post(&queue->mutex);          // Esce sezione critica
    
    if (ok) {
        semaphore_post(&queue->items);      // Items += 1 (un nuovo item per consumer)
    } else {
        semaphore_post(&queue->slots);      // Slot ripristinato se errore
    }
    
    return ok;
}
```

**Spiegazione:**
Inserisce utente in coda con primitivi sincronizzazione:
- `slots`: conta spazi liberi (producer attende >0)
- `items`: conta utenti pronti (consumer attende >0)
- `mutex`: protegge head/tail/count

## FUNZIONE: dequeue_user_timed (RIGHE 110-128)

```c
bool dequeue_user_timed(station_queue_t *queue, int *user_id, long long timeout_ns) {
    // Consumer (worker) attende con timeout
    if (!semaphore_wait_timeout(&queue->items, timeout_ns)) {
        return false;  // Timeout scaduto, nessun utente
    }
    
    semaphore_wait(&queue->mutex);          // Sezione critica
    
    *user_id = queue->queue[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE;
    queue->count--;
    
    semaphore_post(&queue->mutex);          // Fine sezione critica
    
    semaphore_post(&queue->slots);          // Slot += 1 (uno spazio libero in più)
    
    return true;
}
```

**Spiegazione:**
Estrae utente da coda con timeout. Se timeout, ritorna false (worker può controllare day_open).

## FUNZIONE: record_wait (RIGHE 130-137)

```c
void record_wait(shared_state_t *shared, station_type_t station, long long wait_ns) {
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.wait_ns[station] += wait_ns;
    shared->day_stats.wait_count[station] += 1;
    semaphore_post(&shared->stats_mutex);
}
```

**Spiegazione:**
Accumula tempo attesa. Da usare: `wait_ns = get_time_ns() - user->request_enqueued_ns`
Nel worker il valore viene calcolato subito dopo il dequeue, prima di simulare il servizio: così la metrica rappresenta il tempo in coda, non `coda + servizio`.

## FUNZIONE: record_dish_served (RIGHE 139-144)

```c
void record_dish_served(shared_state_t *shared, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.dishes_served[station] += 1;
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: record_unserved_user (RIGHE 146-153)

```c
void record_unserved_user(shared_state_t *shared, user_state_t *user) {
    semaphore_wait(&shared->stats_mutex);
    
    // Evita raddoppio: se già conteggiato, non riconta
    if (!user->daily_outcome_recorded) {
        shared->day_stats.unserved_users += 1;
        user->daily_outcome_recorded = 1;
    }
    
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: record_served_user (RIGHE 155-162)

```c
void record_served_user(shared_state_t *shared, user_state_t *user) {
    semaphore_wait(&shared->stats_mutex);
    
    if (!user->daily_outcome_recorded) {
        shared->day_stats.served_users += 1;
        user->daily_outcome_recorded = 1;
    }
    
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: assign_workers (RIGHE 180-224)

```c
void assign_workers(shared_state_t *shared) {
    int i = 0;
    
    // Pesa le stazioni in base ai tempi medi di servizio
    int weights[STATION_COUNT] = {
        shared->cfg.avg_service_time_primi_seconds,      // Primi più lenti → più worker
        shared->cfg.avg_service_time_secondi_seconds,
        shared->cfg.avg_service_time_coffee_seconds,
        shared->cfg.avg_service_time_cassa_seconds
    };
    
    int workers_per_station[STATION_COUNT] = {1, 1, 1, 1};  // Almeno 1 per stazione
    int remaining = shared->cfg.num_workers - STATION_COUNT; // Worker in più da assegnare
    
    // Distribuzione greedy basata su pesi
    while (remaining > 0) {
        int max_idx = 0;
        
        // Trova stazione con weight massimo
        for (int s = 1; s < STATION_COUNT; ++s) {
            if (weights[s] > weights[max_idx]) {
                max_idx = s;
            }
        }
        
        // Assegna worker a stazione più pesante
        workers_per_station[max_idx] += 1;
        weights[max_idx] = weights[max_idx] / 2 + 1;  // Ridotta weight (già assegnato)
        remaining--;
    }
    
    // Popola array assegnazioni
    for (int station = 0; station < STATION_COUNT; ++station) {
        shared->stations[station].assigned_workers = workers_per_station[station];
        
        for (int j = 0; j < workers_per_station[station]; ++j) {
            shared->worker_assignments[i++] = station;
        }
    }
}
```

**Spiegazione:**
Distribuisce operatori alle stazioni:
1. Garantisce almeno 1 per stazione
2. Assegna rimanenti a stazioni più lente (maggior tempo medio servizio)

**Esempio:**
- avg_service_time_primi_seconds = 6, avg_service_time_secondi_seconds = 8, avg_service_time_coffee_seconds = 3, avg_service_time_cassa_seconds = 4
- 6 worker totali: 1+1+1+1 = 4, rimangono 2
- Weight max = 8 (secondi) → assegna 1 → weights = [6, 4+1, 3, 4]
- Weight max = 6 (primi) → assegna 1 → weights = [3+1, 5, 3, 4]
- Result: secondi 2, primi 2, caffè 1, cassa 1

## FUNZIONE: setup_day (RIGHE 226-267)

```c
void setup_day(shared_state_t *shared) {
    // Reset metriche giornaliere
    memset(&shared->day_stats, 0, sizeof(shared->day_stats));
    
    shared->current_day += 1;  // Incrementa giorno
    shared->day_open = 1;      // Giornata inizia
    shared->table_occupied = 0;
    
    assign_workers(shared);    // Ridistribuisci worker
    
    // Rifornimento iniziale magazzino
    semaphore_wait(&shared->inventory.mutex);
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        shared->inventory.primi[i] = shared->cfg.avg_refill_primi;
    }
    for (int i = 0; i < shared->menu.secondi_count; ++i) {
        shared->inventory.secondi[i] = shared->cfg.avg_refill_secondi;
    }
    semaphore_post(&shared->inventory.mutex);
    
    // Reset code stazioni
    for (int i = 0; i < STATION_COUNT; ++i) {
        semaphore_wait(&shared->stations[i].mutex);
        shared->stations[i].head = 0;
        shared->stations[i].tail = 0;
        shared->stations[i].count = 0;
        shared->stations[i].active_workers = 0;
        semaphore_set_value(shared->stations[i].items, 0);
        semaphore_set_value(shared->stations[i].slots, MAX_QUEUE);
        semaphore_post(&shared->stations[i].mutex);
    }
    
    // Reset sincronizzazione gruppi
    for (int i = 0; i < MAX_GROUPS; ++i) {
        semaphore_wait(&shared->groups[i].mutex);
        shared->groups[i].planned_today = 0;
        shared->groups[i].ready_today = 0;
        semaphore_set_value(shared->groups[i].barrier_sem, 0);
        semaphore_post(&shared->groups[i].mutex);
    }
    
    // RILASCIA TUTTI I PROCESSI per inizio giornata
    for (int i = 0; i < shared->cfg.num_workers + shared->living_users; ++i) {
        semaphore_post(&shared->day_start_sem);  // Sbloccaognuno dal giorno precedente
    }
}
```

Il reset delle code non aggiorna solo gli indici (`head`, `tail`, `count`): reimposta anche i semafori contatore `items` e `slots`. In questo modo, se a fine giornata restano richieste non servite, il giorno dopo la coda riparte davvero vuota e i worker non leggono elementi fantasma.

## FUNZIONE: close_day (RIGHE 269-311)

```c
void close_day(shared_state_t *shared) {
    shared->day_open = 0;  // Chiudi giornata
    
    // Conteggia utenti rimasti in sospeso come non serviti
    semaphore_wait(&shared->stats_mutex);
    for (int i = 0; i < shared->total_users_spawned; ++i) {
        if (shared->users[i].alive &&
            shared->users[i].attending_today &&
            !shared->users[i].daily_outcome_recorded) {
            shared->day_stats.unserved_users += 1;
            shared->users[i].daily_outcome_recorded = 1;
        }
    }
    semaphore_post(&shared->stats_mutex);
    
    // Calcola avanzi magazzino
    semaphore_wait(&shared->inventory.mutex);
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        shared->day_stats.leftovers[STATION_PRIMI] += shared->inventory.primi[i];
    }
    for (int i = 0; i < shared->menu.secondi_count; ++i) {
        shared->day_stats.leftovers[STATION_SECONDI] += shared->inventory.secondi[i];
    }
    shared->day_stats.leftovers[STATION_COFFEE] = 0;  // Illimitato
    semaphore_post(&shared->inventory.mutex);
    
    // Accumula statistiche giornaliere al totale
    semaphore_wait(&shared->stats_mutex);
    accumulate_statistics(&shared->total_stats, &shared->day_stats);
    memset(shared->worker_pauses_day, 0, sizeof(shared->worker_pauses_day));
    semaphore_post(&shared->stats_mutex);
}
```

## FUNZIONE: refill_inventory_if_needed (RIGHE 313-345)

```c
void refill_inventory_if_needed(shared_state_t *shared, long long elapsed_ns, long long *next_refill_ns) {
    if (elapsed_ns < *next_refill_ns) {
        return;  // Non ancora tempo
    }
    
    // Aggiungi porzioni
    semaphore_wait(&shared->inventory.mutex);
    
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        int target = shared->inventory.primi[i] + shared->cfg.avg_refill_primi;
        if (target > shared->cfg.max_porzioni_primi) {
            target = shared->cfg.max_porzioni_primi;  // Non superare max
        }
        shared->inventory.primi[i] = target;
    }
    
    for (int i = 0; i < shared->menu.secondi_count; ++i) {
        int target = shared->inventory.secondi[i] + shared->cfg.avg_refill_secondi;
        if (target > shared->cfg.max_porzioni_secondi) {
            target = shared->cfg.max_porzioni_secondi;
        }
        shared->inventory.secondi[i] = target;
    }
    
    semaphore_post(&shared->inventory.mutex);
    
    // Pianifica prossimo refill con variazione casuale ±20%
    *next_refill_ns += simulate_seconds_to_nanoseconds(
        &shared->cfg,
        add_random_variation(shared->cfg.avg_refill_time * 60, 20)
    );
}
```

## FUNZIONE: avg_wait_ms (RIGHE 347-354)

```c
static double avg_wait_ms(long long total_ns, long count) {
    if (count == 0) {
        return 0.0;
    }
    
    // Converte nanosecondi totali in millisecondi medio
    return (double) total_ns / 1000000.0 / (double) count;
}
```

[CONTINUA CON MANAGER.C E ALTRI...]
