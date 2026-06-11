#include "simulation.h"

/** @brief Sceglie un indice casuale valido all'interno del menu richiesto.
 * @param count Il numero di opzioni nel menu.
 * @return L'indice scelto casualmente.
 */
static int choose_random_menu_index(int count) {
    return random_in_range(0, count - 1);
}

/** @brief Sceglie la prossima stazione tra quelle ancora desiderate guardando la lunghezza delle code.
 * @param shared Puntatore allo stato condiviso.
 * @param need_primo Indica se l'utente ha bisogno di un piatto primo.
 * @param need_secondo Indica se l'utente ha bisogno di un piatto secondo.
 * @param need_coffee Indica se l'utente ha bisogno di caffè.
 * @return Il tipo di stazione scelta.
 */
static station_type_t choose_next_station(shared_state_t *shared, bool need_primo, bool need_secondo, bool need_coffee) {
    station_type_t choice = STATION_PRIMI;
    int best_count = INT_MAX;
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

/** @brief Azzera lo stato giornaliero dell'utente prima dell'avvio di una nuova giornata.
 * @param user Puntatore allo stato dell'utente.
 */
static void reset_user_day(user_state_t *user) {
    /* Lo stato persistente dell'utente resta, mentre le preferenze del giorno vengono azzerate. */
    user->attending_today = 0;
    user->daily_outcome_recorded = 0;
    user->dishes_bought = 0;
    user->wanted_primo = -1;
    user->wanted_secondo = -1;
    user->wants_coffee = 0;
    user->total_price_cents = 0;
}

/** @brief Attende la risposta di una stazione fino a fine giornata o shutdown.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 * @return true se l'utente ha ricevuto un servizio, false altrimenti.
 */
static bool wait_for_service(shared_state_t *shared, user_state_t *user) {
    /* timeout_ns è il passo di attesa con cui l'utente ricontrolla day_open e shutdown. */
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&user->served_sem, timeout_ns)) {
            return true;
        }
    }
    return false;
}

/** @brief Invia la richiesta a una stazione e attende l'esito del servizio.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 * @param station Il tipo di stazione a cui inviare la richiesta.
 * @param preferred_dish L'indice del piatto preferito.
 * @return true se la richiesta è stata servita, false altrimenti.
 */
static bool request_station(shared_state_t *shared, user_state_t *user, station_type_t station, int preferred_dish) {
    /* L'utente prepara la richiesta nello shared state e poi si mette in coda alla stazione. */
    user->request_station = station;
    user->request_dish = preferred_dish;
    user->service_successful = 0;
    user->served_dish_index = -1;
    user->request_enqueued_ns = get_time_ns();
    if (!enqueue_user(&shared->stations[station], user->id)) {
        return false;
    }
    if (!wait_for_service(shared, user)) {
        return false;
    }
    return user->service_successful;
}

/** @brief Registra che un membro del gruppo parteciperà alla giornata corrente.
 * @param shared Puntatore allo stato condiviso.
 * @param group_id L'ID del gruppo.
 */
static void group_arrive(shared_state_t *shared, int group_id) {
    group_state_t *group = &shared->groups[group_id];
    semaphore_wait(&group->mutex);
    group->planned_today += 1;
    semaphore_post(&group->mutex);
}

/** @brief Sincronizza i membri presenti del gruppo prima del passaggio finale alla cassa.
 * @param shared Puntatore allo stato condiviso.
 * @param group_id L'ID del gruppo.
 * @return true se tutti i membri sono pronti, false altrimenti.
 */
static bool group_wait_ready(shared_state_t *shared, int group_id) {
    int releases = 0;
    group_state_t *group = &shared->groups[group_id];
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    semaphore_wait(&group->mutex);
    group->ready_today += 1;
    if (group->ready_today == group->planned_today) {
        releases = group->planned_today;
    }
    semaphore_post(&group->mutex);
    while (releases-- > 0) {
        semaphore_post(&group->barrier_sem);
    }
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&group->barrier_sem, timeout_ns)) {
            return true;
        }
    }
    return false;
}

/** @brief Simula il passaggio dell'utente con ticket nella fase di lettura del ticket.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 * @return true se l'utente ha un ticket valido, false altrimenti.
 */
static bool maybe_ticket(shared_state_t *shared, user_state_t *user) {
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    if (!user->has_ticket) {
        return true;
    }
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->ticket_reader_sem, timeout_ns)) {
            sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, 2));
            semaphore_post(&shared->ticket_reader_sem);
            return true;
        }
    }
    return false;
}

/** @brief Attende un posto libero e simula il consumo del pasto ai tavoli della mensa.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 */
static void consume_meal_at_table(shared_state_t *shared, user_state_t *user) {
    /* timeout_ns evita un'attesa infinita sui tavoli quando la giornata termina. */
    long long timeout_ns = simulate_seconds_to_nanoseconds(&shared->cfg, 1);
    while (shared->day_open && !shared->shutdown) {
        if (semaphore_wait_timeout(&shared->table_sem, timeout_ns)) {
            break;
        }
    }
    if (!shared->day_open || shared->shutdown) {
        return;
    }
    semaphore_wait(&shared->stats_mutex);
    shared->table_occupied += 1;
    semaphore_post(&shared->stats_mutex);
    sleep_ns(simulate_seconds_to_nanoseconds(&shared->cfg, user->dishes_bought * 12));
    semaphore_wait(&shared->stats_mutex);
    if (shared->table_occupied > 0) {
        shared->table_occupied -= 1;
    }
    semaphore_post(&shared->stats_mutex);
    semaphore_post(&shared->table_sem);
}


int main(int argc, char **argv) {
    /* shm_fd è il descrittore della memoria condivisa aperta dal processo utente. */
    int shm_fd;
    /* user_id identifica l'utente corrente dentro l'array shared->users. */
    int user_id;
    /* shared è il puntatore allo stato globale della simulazione. */
    shared_state_t *shared;
    /* user è un alias comodo al record condiviso di questo utente. */
    user_state_t *user;
    srand((unsigned) (time(NULL) ^ getpid()));
    if (argc != 3) {
        exit_with_message("uso: user <id_shm> <id_utente>");
    }
    user_id = atoi(argv[2]);
    shm_fd = ipc_open_and_map(argv[1], sizeof(shared_state_t), (void **) &shared);
    user = &shared->users[user_id];
    semaphore_post(&shared->init_sem);
    while (!shared->shutdown) {
        semaphore_wait(&shared->day_start_sem);
        if (shared->shutdown) {
            break;
        }
        reset_user_day(user);
        /* Una quota di utenti può decidere di non presentarsi in mensa quel giorno. */
        if (random_in_range(0, 99) < 20) {
            continue;
        }
        user->attending_today = 1;
        user->wanted_primo = choose_random_menu_index(shared->menu.primi_count);
        user->wanted_secondo = choose_random_menu_index(shared->menu.secondi_count);
        user->wants_coffee = random_in_range(0, 99) < 70;
        group_arrive(shared, user->group_id);
        /* La visita segue l'ordine logico: cibo, sincronizzazione col gruppo, cassa, refezione. */
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
                if (request_station(shared, user, station, preferred)) {
                    user->dishes_bought += 1;
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
        if (!group_wait_ready(shared, user->group_id)) {
            record_unserved_user(shared, user);
            continue;
        }
        if (user->dishes_bought == 0) {
            /* Se non ha ottenuto nulla non passa in cassa e viene marcato non servito. */
            record_unserved_user(shared, user);
            continue;
        }
        if (!maybe_ticket(shared, user)) {
            record_unserved_user(shared, user);
            continue;
        }
        if (user->has_ticket) {
            user->total_price_cents -= (user->total_price_cents * shared->cfg.ticket_discount_percent) / 100;
        }
        if (!request_station(shared, user, STATION_CASSA, 0)) {
            /* Mancare la cassa invalida l'intero percorso di consumo della giornata. */
            record_unserved_user(shared, user);
            continue;
        }
        record_served_user(shared, user);
        consume_meal_at_table(shared, user);
    }
    ipc_unmap_and_close(shm_fd, shared, sizeof(shared_state_t));
    return 0;
}
