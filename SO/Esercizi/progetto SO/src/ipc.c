#include "ipc.h"

/* Inizializza un semaforo System V e termina il programma in caso di errore. */
static void init_sem_or_die(ipc_sem_t *sem, unsigned value) {
    *sem = semaphore_create(value);
}

int ipc_create_and_map(size_t size, void **addr) {
    int shm_id = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id < 0) {
        exit_with_errno("shmget create");
    }
    *addr = shmat(shm_id, NULL, 0);
    if (*addr == (void *) -1) {
        exit_with_errno("shmat create");
    }
    return shm_id;
}

int ipc_open_and_map(const char *token, size_t size, void **addr) {
    long shm_id;
    char *end = NULL;
    (void) size;
    errno = 0;
    shm_id = strtol(token, &end, 10);
    if (errno != 0 || token[0] == '\0' || end == token || *end != '\0' || shm_id < 0 || shm_id > INT_MAX) {
        exit_with_message("invalid shared memory token: %s", token);
    }
    *addr = shmat((int) shm_id, NULL, 0);
    if (*addr == (void *) -1) {
        exit_with_errno("shmat open");
    }
    return (int) shm_id;
}

void ipc_unmap_and_close(int shm_id, void *addr, size_t size) {
    (void) shm_id;
    (void) size;
    if (addr != NULL && addr != (void *) -1) {
        shmdt(addr);
    }
}

void ipc_destroy(int shm_id) {
    shmctl(shm_id, IPC_RMID, NULL);
}

void initialize_shared_state(shared_state_t *shared, const config_t *cfg, const menu_data_t *menu) {
    int i;
    memset(shared, 0, sizeof(*shared));
    shared->cfg = *cfg;
    shared->menu = *menu;
    init_sem_or_die(&shared->init_sem, 0);
    init_sem_or_die(&shared->day_start_sem, 0);
    init_sem_or_die(&shared->stats_mutex, 1);
    init_sem_or_die(&shared->ticket_reader_sem, (unsigned) cfg->ticket_reader_capacity);
    init_sem_or_die(&shared->table_sem, (unsigned) cfg->num_table_seats);
    init_sem_or_die(&shared->inventory.mutex, 1);
    for (i = 0; i < STATION_COUNT; ++i) {
        init_sem_or_die(&shared->stations[i].mutex, 1);
        init_sem_or_die(&shared->stations[i].items, 0);
        init_sem_or_die(&shared->stations[i].slots, MAX_QUEUE);
    }
    init_sem_or_die(&shared->stations[STATION_PRIMI].seat_sem, (unsigned) cfg->num_worker_seats_primi);
    init_sem_or_die(&shared->stations[STATION_SECONDI].seat_sem, (unsigned) cfg->num_worker_seats_secondi);
    init_sem_or_die(&shared->stations[STATION_COFFEE].seat_sem, (unsigned) cfg->num_worker_seats_coffee);
    init_sem_or_die(&shared->stations[STATION_CASSA].seat_sem, (unsigned) cfg->num_worker_seats_cassa);
    for (i = 0; i < MAX_GROUPS; ++i) {
        init_sem_or_die(&shared->groups[i].mutex, 1);
        init_sem_or_die(&shared->groups[i].barrier_sem, 0);
    }
    for (i = 0; i < MAX_USERS; ++i) {
        shared->users[i].id = i;
        init_sem_or_die(&shared->users[i].served_sem, 0);
    }
}

void shared_destroy(shared_state_t *shared) {
    int i;
    semaphore_destroy(shared->init_sem);
    semaphore_destroy(shared->day_start_sem);
    semaphore_destroy(shared->stats_mutex);
    semaphore_destroy(shared->ticket_reader_sem);
    semaphore_destroy(shared->table_sem);
    semaphore_destroy(shared->inventory.mutex);
    for (i = 0; i < STATION_COUNT; ++i) {
        semaphore_destroy(shared->stations[i].mutex);
        semaphore_destroy(shared->stations[i].items);
        semaphore_destroy(shared->stations[i].slots);
        semaphore_destroy(shared->stations[i].seat_sem);
    }
    for (i = 0; i < MAX_GROUPS; ++i) {
        semaphore_destroy(shared->groups[i].mutex);
        semaphore_destroy(shared->groups[i].barrier_sem);
    }
    for (i = 0; i < MAX_USERS; ++i) {
        semaphore_destroy(shared->users[i].served_sem);
    }
}
