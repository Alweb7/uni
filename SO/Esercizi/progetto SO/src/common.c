#include "common.h"

static int semop_retry(ipc_sem_t semid, short sem_op, const struct timespec *timeout) {
    struct sembuf op = {
        .sem_num = 0,
        .sem_op = sem_op,
        .sem_flg = 0
    };
    for (;;) {
        int rc;
        if (timeout != NULL) {
            rc = semtimedop(semid, &op, 1, timeout);
        } else {
            rc = semop(semid, &op, 1);
        }
        if (rc == 0) {
            return 0;
        }
        if (errno == EINTR) {
            continue;
        }
        return -1;
    }
}

long long get_time_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        exit_with_errno("clock_gettime");
    }
    return (long long) ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

void sleep_ns(long long ns) {
    struct timespec ts;
    if (ns <= 0) {
        return;
    }
    ts.tv_sec = ns / 1000000000LL;
    ts.tv_nsec = ns % 1000000000LL;
    while (nanosleep(&ts, &ts) != 0 && errno == EINTR) {
    }
}

ipc_sem_t semaphore_create(unsigned value) {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
    union semun arg;
    if (semid < 0) {
        exit_with_errno("semget");
    }
    arg.val = (int) value;
    if (semctl(semid, 0, SETVAL, arg) != 0) {
        exit_with_errno("semctl SETVAL");
    }
    return semid;
}

int semaphore_wait(ipc_sem_t *sem) {
    if (semop_retry(*sem, -1, NULL) != 0) {
        exit_with_errno("semop wait");
    }
    return 0;
}

int semaphore_post(ipc_sem_t *sem) {
    if (semop_retry(*sem, 1, NULL) != 0) {
        exit_with_errno("semop post");
    }
    return 0;
}

bool semaphore_wait_timeout(ipc_sem_t *sem, long long timeout_ns) {
    struct timespec timeout;
    if (timeout_ns < 0) {
        timeout_ns = 0;
    }
    timeout.tv_sec = timeout_ns / 1000000000LL;
    timeout.tv_nsec = timeout_ns % 1000000000LL;
    if (semop_retry(*sem, -1, &timeout) == 0) {
        return true;
    }
    if (errno == EAGAIN) {
        return false;
    }
    exit_with_errno("semtimedop");
    return false;
}

void semaphore_set_value(ipc_sem_t sem, unsigned value) {
    union semun arg;
    arg.val = (int) value;
    if (semctl(sem, 0, SETVAL, arg) != 0) {
        exit_with_errno("semctl SETVAL");
    }
}

void semaphore_destroy(ipc_sem_t sem) {
    if (sem >= 0) {
        semctl(sem, 0, IPC_RMID);
    }
}

void exit_with_errno(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void exit_with_message(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
    exit(EXIT_FAILURE);
}

int random_in_range(int min_value, int max_value) {
    if (max_value <= min_value) {
        return min_value;
    }
    return min_value + (rand() % (max_value - min_value + 1));
}

long long simulate_seconds_to_nanoseconds(const config_t *cfg, int sim_seconds) {
    return ((long long) cfg->nanoseconds_per_simulated_minute * sim_seconds) / 60LL;
}

int add_random_variation(int base, int percent) {
    int delta = (base * percent) / 100;
    int min_value = base - delta;
    int max_value = base + delta;
    if (min_value < 1) {
        min_value = 1;
    }
    return random_in_range(min_value, max_value);
}

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
