#include "simulation.h"
#include "config.h"

static volatile sig_atomic_t g_stop = 0;

/** @brief Gestisce i segnali inviati al manager.
 * @param sig Il segnale ricevuto.
 */
static void on_signal(int sig) {
    (void) sig;
    g_stop = 1;
}

/** @brief Crea un figlio ed esegue uno degli eseguibili del simulatore passandogli shm e identificativo.
 * @param path Il percorso dell'eseguibile da eseguire.
 * @param shm_id L'ID della memoria condivisa.
 * @param id L'identificativo del processo figlio.
 */
static void spawn_child(const char *path, int shm_id, int id) {
    char shm_buf[16];
    char id_buf[16];
    pid_t pid = fork();
    if (pid < 0) {
        exit_with_errno("fork");
    }
    if (pid == 0) {
        snprintf(shm_buf, sizeof(shm_buf), "%d", shm_id);
        snprintf(id_buf, sizeof(id_buf), "%d", id);
        execl(path, path, shm_buf, id_buf, (char *) NULL);
        exit_with_errno("execl");
    }
}

/** @brief Ricrea la FIFO di controllo usata dagli eseguibili esterni della versione completa.
 * @param path Il percorso della FIFO da ricreare.
 */
static void ensure_fifo(const char *path) {
    unlink(path);
    if (mkfifo(path, 0666) != 0) {
        exit_with_errno("mkfifo");
    }
}

/** @brief Assegna gli utenti iniziali a gruppi casuali e determina il possesso del ticket.
 * @param shared Puntatore allo stato condiviso.
 */
static void assign_groups(shared_state_t *shared) {
    int user = 0;
    int group = 0;
    while (user < shared->cfg.num_users) {
        int size = random_in_range(1, shared->cfg.max_users_per_group);
        for (int i = 0; i < size && user < shared->cfg.num_users; ++i) {
            shared->users[user].group_id = group;
            shared->users[user].has_ticket = (random_in_range(0, 99) < 80);
            shared->users[user].alive = 1;
            user++;
        }
        group++;
    }
    shared->living_users = shared->cfg.num_users;
    shared->total_users_spawned = shared->cfg.num_users;
}

/** @brief Attende che tutti i processi figli completino la propria inizializzazione.
 * @param shared Puntatore allo stato condiviso.
 */
static void wait_for_init(shared_state_t *shared) {
    int expected = shared->cfg.num_workers + shared->cfg.num_users;
    for (int i = 0; i < expected; ++i) {
        semaphore_wait(&shared->init_sem);
    }
}

/** @brief Avvia tutti i processi worker e user previsti dalla configurazione iniziale.
 * @param shared Puntatore allo stato condiviso.
 * @param shm_id L'ID della memoria condivisa.
 */
static void spawn_initial_processes(shared_state_t *shared, int shm_id) {
    for (int i = 0; i < shared->cfg.num_workers; ++i) {
        spawn_child("./worker", shm_id, i);
    }
    for (int i = 0; i < shared->cfg.num_users; ++i) {
        spawn_child("./user", shm_id, i);
    }
}

/** @brief Aggiunge nuovi utenti alla simulazione durante l'esecuzione rispettando il limite massimo configurato.
 * @param shared Puntatore allo stato condiviso.
 * @param shm_id L'ID della memoria condivisa.
 * @param count Il numero di nuovi utenti da creare.
 */
static void spawn_additional_users(shared_state_t *shared, int shm_id, int count) {
    int start = shared->total_users_spawned;
    int end = start + count;
    if (end > shared->cfg.num_users + shared->cfg.max_dynamic_users) {
        end = shared->cfg.num_users + shared->cfg.max_dynamic_users;
    }
    for (int i = start; i < end; ++i) {
        shared->users[i].group_id = i;
        shared->users[i].has_ticket = (random_in_range(0, 99) < 80);
        shared->users[i].alive = 1;
        shared->total_users_spawned += 1;
        shared->living_users += 1;
        spawn_child("./user", shm_id, i);
        semaphore_wait(&shared->init_sem);
    }
}

/** @brief Interpreta una riga letta dalla FIFO di controllo ed esegue il comando richiesto.
 * @param shared Puntatore allo stato condiviso.
 * @param shm_id L'ID della memoria condivisa.
 * @param line La riga letta dalla FIFO.
 */
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

/** @brief Consuma eventuali comandi pendenti sulla FIFO di controllo della simulazione.
 * @param shared Puntatore allo stato condiviso.
 * @param shm_id L'ID della memoria condivisa.
 * @param fifo_fd Il descrittore della FIFO di controllo.
 */
static void handle_control_commands(shared_state_t *shared, int shm_id, int fifo_fd) {
    char buffer[256];
    ssize_t n = read(fifo_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        return;
    }
    buffer[n] = '\0';
    char *line = strtok(buffer, "\n");
    while (line != NULL) {
        process_control_line(shared, shm_id, line);
        line = strtok(NULL, "\n");
    }
}

int main(int argc, char **argv) {
    /* cfg e menu contengono i dati di configurazione caricati dai file. */
    config_t cfg;
    menu_data_t menu;
    /* shared è la vista in memoria della struttura globale condivisa. */
    shared_state_t *shared;
    /* shm_fd e fifo_fd sono i descrittori delle due IPC principali del manager. */
    int shm_fd;
    int fifo_fd;
    /* reason memorizza il motivo finale di terminazione per report e CSV. */
    termination_reason_t reason = TERM_TIMEOUT;
    if (argc != 2) {
        exit_with_message("uso: manager <percorso_config>");
    }
    srand((unsigned) (time(NULL) ^ getpid()));
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    config_load(argv[1], &cfg);
    menu_load(cfg.menu_path, &menu);
    config_validate(&cfg, &menu);
    /* Dopo la validazione il manager prepara tutte le risorse IPC condivise. */
    ensure_fifo(cfg.fifo_path);
    fifo_fd = open(cfg.fifo_path, O_RDWR | O_NONBLOCK);
    if (fifo_fd < 0) {
        exit_with_errno("open fifo");
    }
    shm_fd = ipc_create_and_map(sizeof(shared_state_t), (void **) &shared);
    initialize_shared_state(shared, &cfg, &menu);
    assign_groups(shared);
    spawn_initial_processes(shared, shm_fd);
    wait_for_init(shared);
    shared->started = 1;
    for (int day = 1; day <= cfg.sim_duration_days && !g_stop; ++day) {
        /* day_ns è la durata reale dell'intera giornata simulata. */
        long long day_ns = (long long) cfg.day_duration_minutes * cfg.nanoseconds_per_simulated_minute;
        /* start_ns è il timestamp di partenza della giornata corrente. */
        long long start_ns = get_time_ns();
        /* next_refill_ns è il prossimo istante relativo in cui fare refill del magazzino. */
        long long next_refill_ns = simulate_seconds_to_nanoseconds(&cfg, add_random_variation(cfg.avg_refill_time * 60, 20));
        /* Il ciclo interno rappresenta lo scorrere del tempo reale della giornata simulata. */
        setup_day(shared);
        while (!g_stop && get_time_ns() - start_ns < day_ns) {
            handle_control_commands(shared, shm_fd, fifo_fd);
            refill_inventory_if_needed(shared, get_time_ns() - start_ns, &next_refill_ns);
            if (shared->disorder_active && get_time_ns() >= shared->disorder_until_ns) {
                shared->disorder_active = 0;
            }
            sleep_ns(cfg.nanoseconds_per_simulated_minute);
        }
        close_day(shared);
        /* L'overload viene controllato osservando le code residue alla fine della giornata. */
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
            break;
        }
    }
    if (g_stop && !shared->overload_triggered) {
        reason = TERM_SIGNAL;
    } else if (shared->overload_triggered) {
        reason = TERM_OVERLOAD;
    }
    shared->shutdown = 1;
    /* Un ultimo rilascio sul semaforo giornaliero fa uscire i processi ancora addormentati. */
    for (int i = 0; i < shared->cfg.num_workers + shared->living_users; ++i) {
        semaphore_post(&shared->day_start_sem);
    }
    while (wait(NULL) > 0) {
    }
    print_day_report(stdout, shared, shared->current_day, true, reason);
    append_csv_row(shared, shared->current_day, true, reason);
    shared_destroy(shared);
    close(fifo_fd);
    unlink(cfg.fifo_path);
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    ipc_destroy(shm_fd);
    return 0;
}
