#include "simulation.h"

/** @brief Registra che il worker ha preso possesso di una postazione operativa per la giornata.
 * @param shared Puntatore allo stato condiviso.
 * @param worker_id L'ID del worker.
 * @param station Il tipo di stazione.
 */
static void mark_worker_active(shared_state_t *shared, int worker_id, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    if (!shared->worker_ever_active[worker_id]) {
        shared->worker_ever_active[worker_id] = 1;
    }
    shared->day_stats.active_workers_total += 1;
    shared->stations[station].active_workers += 1;
    semaphore_post(&shared->stats_mutex);
}

/** @brief Aggiorna lo stato condiviso quando un worker smette di presidiare una stazione.
 * @param shared Puntatore allo stato condiviso.
 * @param station Il tipo di stazione.
 */
static void mark_worker_inactive(shared_state_t *shared, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    if (shared->stations[station].active_workers > 0) {
        shared->stations[station].active_workers -= 1;
    }
    semaphore_post(&shared->stats_mutex);
}

/** @brief Preleva dal magazzino il piatto richiesto o una valida alternativa dello stesso tipo.
 * @param shared Puntatore allo stato condiviso.
 * @param station Il tipo di stazione.
 * @param preferred L'indice del piatto preferito.
 * @return L'indice del piatto effettivamente assegnato, oppure -1 se assente.
 */
static int take_dish_from_inventory(shared_state_t *shared, station_type_t station, int preferred) {
    int found = -1;
    semaphore_wait(&shared->inventory.mutex);
    if (station == STATION_PRIMI) {
        if (preferred >= 0 && preferred < shared->menu.primi_count && shared->inventory.primi[preferred] > 0) {
            shared->inventory.primi[preferred]--;
            found = preferred;
        } else {
            for (int i = 0; i < shared->menu.primi_count; ++i) {
                if (shared->inventory.primi[i] > 0) {
                    shared->inventory.primi[i]--;
                    found = i;
                    break;
                }
            }
        }
    } else if (station == STATION_SECONDI) {
        if (preferred >= 0 && preferred < shared->menu.secondi_count && shared->inventory.secondi[preferred] > 0) {
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
        found = preferred >= 0 ? preferred : 0;
    }
    semaphore_post(&shared->inventory.mutex);
    return found;
}

/** @brief Gestisce una pausa del worker senza lasciare la stazione completamente scoperta.
 * @param shared Puntatore allo stato condiviso.
 * @param worker_id L'ID del worker.
 * @param station Il tipo di stazione.
 * @param pauses_done Puntatore al contatore dei periodi di pausa già effettuati.
 */
static void maybe_pause(shared_state_t *shared, int worker_id, station_type_t station, int *pauses_done) {
    if (*pauses_done >= shared->cfg.max_pauses_per_day || random_in_range(0, 99) > 4) {
        return;
    }
    semaphore_wait(&shared->stats_mutex);
    if (shared->stations[station].active_workers <= 1) {
        semaphore_post(&shared->stats_mutex);
        return;
    }
    shared->stations[station].active_workers -= 1;
    semaphore_post(&shared->stats_mutex);
    semaphore_post(&shared->stations[station].seat_sem);
    record_pause(shared, worker_id);
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, add_random_variation(shared->cfg.avg_pause_seconds, 40)));
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->stations[station].seat_sem, simulate_seconds_to_nanoseconds(&shared->cfg, 1))) {
            break;
        }
    }
    if (!shared->day_open || shared->shutdown) {
        return;
    }
    semaphore_wait(&shared->stats_mutex);
    shared->stations[station].active_workers += 1;
    semaphore_post(&shared->stats_mutex);
    (*pauses_done)++;
}

/** @brief Eroga il servizio richiesto dall'utente e aggiorna le statistiche della stazione.
 * @param shared Puntatore allo stato condiviso.
 * @param user_id L'ID dell'utente.
 * @param station Il tipo di stazione.
 */
static void serve_request(shared_state_t *shared, int user_id, station_type_t station) {
    user_state_t *user = &shared->users[user_id];
    long long wait_ns = get_time_ns() - user->request_enqueued_ns;
    int service_seconds = 1;
    int served_dish_index = -1;
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
        served_dish_index = 1;
        record_revenue(shared, user->total_price_cents);
    }
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, service_seconds));
    user->service_successful = (served_dish_index >= 0);
    user->served_dish_index = served_dish_index;
    if (user->service_successful && station != STATION_CASSA) {
        record_dish_served(shared, station);
    }
    record_wait(shared, station, wait_ns);
    semaphore_post(&user->served_sem);
}

/* Implementa il ciclo di vita di un processo worker assegnato a una stazione della mensa. */
int main(int argc, char **argv) {
    /* shm_fd è il descrittore della shared memory aperta dal worker. */
    int shm_fd;
    /* shared punta alla struttura globale condivisa tra tutti i processi. */
    shared_state_t *shared;
    /* worker_id identifica questo processo nel vettore delle assegnazioni. */
    int worker_id;
    srand((unsigned) (time(NULL) ^ getpid()));
    if (argc != 3) {
        exit_with_message("uso: worker <id_shm> <id_worker>");
    }
    worker_id = atoi(argv[2]);
    shm_fd = ipc_open_and_map(argv[1], sizeof(shared_state_t), (void **) &shared);
    semaphore_post(&shared->init_sem);
    while (!shared->shutdown) {
        /* pauses_done conta quante pause il worker ha già consumato oggi. */
        int pauses_done = 0;
        /* user_id riceve l'utente estratto dalla coda della stazione. */
        int user_id;
        /* station è la stazione a cui il worker è assegnato per la giornata. */
        station_type_t station;
        /* wait_tick_ns è il timeout breve usato per restare reattivi a chiusure e stop. */
        long long wait_tick_ns;
        semaphore_wait(&shared->day_start_sem);
        if (shared->shutdown) {
            break;
        }
        /* L'assegnazione alla stazione viene ridefinita dal manager a ogni nuova giornata. */
        station = (station_type_t) shared->worker_assignments[worker_id];
        wait_tick_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
        /* Extra workers wait on the seat semaphore, but wake up periodically to notice day end/shutdown. */
        while (shared->day_open && !shared->shutdown) {
            if (semaphore_wait_timeout(&shared->stations[station].seat_sem, wait_tick_ns)) {
                break;
            }
        }
        if (!shared->day_open || shared->shutdown) {
            continue;
        }
        mark_worker_active(shared, worker_id, station);
        /* Da qui il worker resta in servizio finché la giornata non termina o non arriva shutdown. */
        while (shared->day_open && !shared->shutdown) {
            if (station == STATION_CASSA &&
                shared->disorder_active &&
                get_time_ns() < shared->disorder_until_ns) {
                sleep_ns(wait_tick_ns);
                continue;
            }
            /* Waiting on the queue with timeout removes the old polling loop while keeping day-end responsiveness. */
            if (dequeue_user_timed(&shared->stations[station], &user_id, wait_tick_ns)) {
                serve_request(shared, user_id, station);
            } else {
                maybe_pause(shared, worker_id, station, &pauses_done);
            }
        }
        mark_worker_inactive(shared, station);
        semaphore_post(&shared->stations[station].seat_sem);
    }
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    return 0;
}
