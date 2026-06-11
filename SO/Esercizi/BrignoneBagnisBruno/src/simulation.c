#include "simulation.h"

/* Accumula una fotografia statistica dentro un totale progressivo. */
static void accumulate_statistics(stats_snapshot_t *dst, const stats_snapshot_t *src) {
    int i;
    dst->served_users += src->served_users;
    dst->unserved_users += src->unserved_users;
    dst->pauses_total += src->pauses_total;
    dst->active_workers_total += src->active_workers_total;
    dst->revenue_cents += src->revenue_cents;
    for (i = 0; i < STATION_COUNT; ++i) {
        dst->dishes_served[i] += src->dishes_served[i];
        dst->leftovers[i] += src->leftovers[i];
        dst->wait_count[i] += src->wait_count[i];
        dst->wait_ns[i] += src->wait_ns[i];
    }
}

/* Calcola il numero complessivo di campioni usati per le attese. */
static long total_wait_count(const stats_snapshot_t *stats) {
    long total = 0;
    for (int i = 0; i < STATION_COUNT; ++i) {
        total += stats->wait_count[i];
    }
    return total;
}

/** @brief Calcola il tempo complessivo passato in coda da tutti gli utenti.
 * @param stats Puntatore allo snapshot statistico.
 * @return Il tempo totale in nanosecondi.
 */
static long long total_wait_ns(const stats_snapshot_t *stats) {
    long long total = 0;
    for (int i = 0; i < STATION_COUNT; ++i) {
        total += stats->wait_ns[i];
    }
    return total;
}

/** @brief Restituisce il numero totale di piatti distribuiti nelle stazioni di servizio.
 * @param stats Puntatore allo snapshot statistico.
 * @return Il numero totale di piatti distribuiti.
 */
static long total_dishes_served(const stats_snapshot_t *stats) {
    return stats->dishes_served[STATION_PRIMI] +
        stats->dishes_served[STATION_SECONDI] +
        stats->dishes_served[STATION_COFFEE];
}

/** @brief Restituisce il totale degli avanzi accumulati nelle stazioni con inventario finito.
 * @param stats Puntatore allo snapshot statistico.
 * @return Il totale degli avanzi.
 */
static long total_leftovers(const stats_snapshot_t *stats) {
    return stats->leftovers[STATION_PRIMI] +
        stats->leftovers[STATION_SECONDI] +
        stats->leftovers[STATION_COFFEE];
}

/** @brief Conta quanti worker hanno lavorato almeno una volta nell'intera simulazione.
 * @param shared Puntatore allo stato condiviso.
 * @return Il numero di worker unici attivi.
 */
static long unique_active_workers(const shared_state_t *shared) {
    long total = 0;
    for (int i = 0; i < shared->cfg.num_workers; ++i) {
        if (shared->worker_ever_active[i]) {
            total++;
        }
    }
    return total;
}


/** @brief Converte il codice di terminazione nella stringa usata nei report.
 * @param reason Il codice di terminazione.
 * @return La stringa corrispondente al codice di terminazione.
 */
static const char *termination_name(termination_reason_t reason) {
    switch (reason) {
        case TERM_TIMEOUT:
            return "tempo_scaduto";
        case TERM_OVERLOAD:
            return "sovraccarico";
        case TERM_SIGNAL:
            return "segnale";
        case TERM_NONE:
        default:
            return "nessuna";
    }
}

bool enqueue_user(station_queue_t *queue, int user_id) {
    bool ok = false;
    semaphore_wait(&queue->slots);
    semaphore_wait(&queue->mutex);
    if (queue->count < MAX_QUEUE) {
        queue->queue[queue->tail] = user_id;
        queue->tail = (queue->tail + 1) % MAX_QUEUE;
        queue->count++;
        ok = true;
    }
    semaphore_post(&queue->mutex);
    if (ok) {
        semaphore_post(&queue->items);
    } else {
        semaphore_post(&queue->slots);
    }
    return ok;
}

bool dequeue_user_timed(station_queue_t *queue, int *user_id, long long timeout_ns) {
    /* Il timeout permette al worker di restare reattivo a fine giornata e shutdown. */
    if (!semaphore_wait_timeout(&queue->items, timeout_ns)) {
        return false;
    }
    semaphore_wait(&queue->mutex);
    *user_id = queue->queue[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE;
    queue->count--;
    semaphore_post(&queue->mutex);
    semaphore_post(&queue->slots);
    return true;
}

void record_wait(shared_state_t *shared, station_type_t station, long long wait_ns) {
    /* Le statistiche sono condivise tra processi distinti, quindi vanno serializzate. */
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.wait_ns[station] += wait_ns;
    shared->day_stats.wait_count[station] += 1;
    semaphore_post(&shared->stats_mutex);
}

void record_dish_served(shared_state_t *shared, station_type_t station) {
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.dishes_served[station] += 1;
    semaphore_post(&shared->stats_mutex);
}

void record_unserved_user(shared_state_t *shared, user_state_t *user) {
    semaphore_wait(&shared->stats_mutex);
    if (!user->daily_outcome_recorded) {
        shared->day_stats.unserved_users += 1;
        user->daily_outcome_recorded = 1;
    }
    semaphore_post(&shared->stats_mutex);
}

void record_served_user(shared_state_t *shared, user_state_t *user) {
    semaphore_wait(&shared->stats_mutex);
    if (!user->daily_outcome_recorded) {
        shared->day_stats.served_users += 1;
        user->daily_outcome_recorded = 1;
    }
    semaphore_post(&shared->stats_mutex);
}

void record_pause(shared_state_t *shared, int worker_id) {
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.pauses_total += 1;
    shared->worker_pauses_day[worker_id] += 1;
    shared->worker_pauses_total[worker_id] += 1;
    semaphore_post(&shared->stats_mutex);
}

void record_revenue(shared_state_t *shared, int cents) {
    semaphore_wait(&shared->stats_mutex);
    shared->day_stats.revenue_cents += cents;
    semaphore_post(&shared->stats_mutex);
}

void assign_workers(shared_state_t *shared) {
    int i = 0;
    int weights[STATION_COUNT] = {
        shared->cfg.avg_service_time_primi_seconds,
        shared->cfg.avg_service_time_secondi_seconds,
        shared->cfg.avg_service_time_coffee_seconds,
        shared->cfg.avg_service_time_cassa_seconds
    };
    int workers_per_station[STATION_COUNT] = {1, 1, 1, 1};
    int remaining = shared->cfg.num_workers - STATION_COUNT;
    while (remaining > 0) {
        int max_idx = 0;
        for (int s = 1; s < STATION_COUNT; ++s) {
            if (weights[s] > weights[max_idx]) {
                max_idx = s;
            }
        }
        workers_per_station[max_idx] += 1;
        weights[max_idx] = weights[max_idx] / 2 + 1;
        remaining--;
    }
    for (int station = 0; station < STATION_COUNT; ++station) {
        shared->stations[station].assigned_workers = workers_per_station[station];
        for (int j = 0; j < workers_per_station[station]; ++j) {
            shared->worker_assignments[i++] = station;
        }
    }
}

void setup_day(shared_state_t *shared) {
    memset(&shared->day_stats, 0, sizeof(shared->day_stats));
    shared->current_day += 1;
    shared->day_open = 1;
    shared->table_occupied = 0;
    assign_workers(shared);
    semaphore_wait(&shared->inventory.mutex);
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        shared->inventory.primi[i] = shared->cfg.avg_refill_primi;
    }
    for (int i = 0; i < shared->menu.secondi_count; ++i) {
        shared->inventory.secondi[i] = shared->cfg.avg_refill_secondi;
    }
    semaphore_post(&shared->inventory.mutex);
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
    for (int i = 0; i < MAX_GROUPS; ++i) {
        semaphore_wait(&shared->groups[i].mutex);
        shared->groups[i].planned_today = 0;
        shared->groups[i].ready_today = 0;
        semaphore_set_value(shared->groups[i].barrier_sem, 0);
        semaphore_post(&shared->groups[i].mutex);
    }
    for (int i = 0; i < shared->cfg.num_workers + shared->living_users; ++i) {
        semaphore_post(&shared->day_start_sem);
    }
}

void close_day(shared_state_t *shared) {
    shared->day_open = 0;
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
    semaphore_wait(&shared->inventory.mutex);
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        shared->day_stats.leftovers[STATION_PRIMI] += shared->inventory.primi[i];
    }
    for (int i = 0; i < shared->menu.secondi_count; ++i) {
        shared->day_stats.leftovers[STATION_SECONDI] += shared->inventory.secondi[i];
    }
    shared->day_stats.leftovers[STATION_COFFEE] = 0;
    semaphore_post(&shared->inventory.mutex);
    semaphore_wait(&shared->stats_mutex);
    accumulate_statistics(&shared->total_stats, &shared->day_stats);
    memset(shared->worker_pauses_day, 0, sizeof(shared->worker_pauses_day));
    semaphore_post(&shared->stats_mutex);
}

void refill_inventory_if_needed(shared_state_t *shared, long long elapsed_ns, long long *next_refill_ns) {
    if (elapsed_ns < *next_refill_ns) {
        return;
    }
    semaphore_wait(&shared->inventory.mutex);
    for (int i = 0; i < shared->menu.primi_count; ++i) {
        int target = shared->inventory.primi[i] + shared->cfg.avg_refill_primi;
        if (target > shared->cfg.max_porzioni_primi) {
            target = shared->cfg.max_porzioni_primi;
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
    *next_refill_ns += simulate_seconds_to_nanoseconds(
        &shared->cfg,
        add_random_variation(shared->cfg.avg_refill_time * 60, 20));
}


/** @brief Calcola la media in millisecondi a partire da tempo totale e numero di campioni.
 * @param total_ns Il tempo totale in nanosecondi.
 * @param count Il numero di campioni usati per la media.
 * @return La media in millisecondi.
 */
static double avg_wait_ms(long long total_ns, long count) {
    if (count == 0) {
        return 0.0;
    }
    return (double) total_ns / 1000000.0 / (double) count;
}

void print_day_report(FILE *out, const shared_state_t *shared, int day, bool final_report, termination_reason_t reason) {
    const stats_snapshot_t *s = final_report ? &shared->total_stats : &shared->day_stats;
    int elapsed_days = final_report && day > 0 ? day : 1;
    long dishes_total = total_dishes_served(s);
    long leftovers_total = total_leftovers(s);
    long wait_samples = total_wait_count(s);
    long long wait_total_ns = total_wait_ns(s);
    long active_workers_sim = unique_active_workers(shared);
    if (final_report) {
        fprintf(out, "\n=== Report FINALE simulazione ===\n");
    } else {
        fprintf(out, "\n=== Report GIORNALIERO giorno %d ===\n", day);
    }
    if (final_report) {
        fprintf(out, "terminazione=%s\n", termination_name(reason));
    }
    fprintf(out, "utenti_serviti=%ld utenti_non_serviti=%ld ricavi=%.2f\n",
        s->served_users, s->unserved_users, s->revenue_cents / 100.0);
    fprintf(out, "media_utenti_giornaliera serviti=%.2f non_serviti=%.2f\n",
        (double) s->served_users / (double) elapsed_days,
        (double) s->unserved_users / (double) elapsed_days);
    fprintf(out, "piatti primi=%ld secondi=%ld coffee=%ld\n",
        s->dishes_served[STATION_PRIMI],
        s->dishes_served[STATION_SECONDI],
        s->dishes_served[STATION_COFFEE]);
    fprintf(out, "piatti_totali=%ld media_piatti_giornaliera=%.2f\n",
        dishes_total, (double) dishes_total / (double) elapsed_days);
    fprintf(out, "avanzi primi=%ld secondi=%ld coffee=%ld totale=%ld media_giornaliera=%.2f\n",
        s->leftovers[STATION_PRIMI],
        s->leftovers[STATION_SECONDI],
        s->leftovers[STATION_COFFEE],
        leftovers_total,
        (double) leftovers_total / (double) elapsed_days);
    fprintf(out, "attesa_media_ms primi=%.2f secondi=%.2f coffee=%.2f cassa=%.2f\n",
        avg_wait_ms(s->wait_ns[STATION_PRIMI], s->wait_count[STATION_PRIMI]),
        avg_wait_ms(s->wait_ns[STATION_SECONDI], s->wait_count[STATION_SECONDI]),
        avg_wait_ms(s->wait_ns[STATION_COFFEE], s->wait_count[STATION_COFFEE]),
        avg_wait_ms(s->wait_ns[STATION_CASSA], s->wait_count[STATION_CASSA]));
    fprintf(out, "attesa_media_ms_totale=%.2f\n", avg_wait_ms(wait_total_ns, wait_samples));
    fprintf(out, "pause=%ld media_pause_giornaliera=%.2f\n",
        s->pauses_total, (double) s->pauses_total / (double) elapsed_days);
    if (final_report) {
        fprintf(out, "operatori_attivi_media_giornaliera=%.2f operatori_attivi_simulazione=%ld\n",
            (double) s->active_workers_total / (double) elapsed_days,
            active_workers_sim);
    } else {
        fprintf(out, "operatori_attivi_giorno=%ld operatori_attivi_simulazione=%ld\n",
            s->active_workers_total,
            active_workers_sim);
    }
    if (final_report) {
        fprintf(out, "ricavi_media_giornaliera=%.2f\n", (s->revenue_cents / 100.0) / (double) elapsed_days);
    }
}

void append_csv_row(const shared_state_t *shared, int day, bool final_row, termination_reason_t reason) {
    /* fp è il file CSV di output aperto in append o in scrittura iniziale. */
    FILE *fp = fopen(shared->cfg.csv_path, day == 1 && !final_row ? "w" : "a");
    /* s seleziona lo snapshot da serializzare nella riga corrente. */
    const stats_snapshot_t *s = final_row ? &shared->total_stats : &shared->day_stats;
    /* elapsed_days, dishes_total, leftovers_total e wait_samples servono per i campi derivati. */
    int elapsed_days = final_row && day > 0 ? day : 1;
    long dishes_total = total_dishes_served(s);
    long leftovers_total = total_leftovers(s);
    long wait_samples = total_wait_count(s);
    if (fp == NULL) {
        return;
    }
    /* La prima riga giornaliera crea o sovrascrive il file e scrive l'header CSV. */
    if (day == 1 && !final_row) {
        fprintf(fp, "row_type,day,termination,served,unserved,served_avg_day,unserved_avg_day,rev_cents,rev_avg_day,primi,secondi,coffee,dishes_total,dishes_avg_day,left_primi,left_secondi,left_coffee,left_total,left_avg_day,wait_primi_ns,wait_secondi_ns,wait_coffee_ns,wait_cassa_ns,wait_total_ns,wait_samples,pauses,pauses_avg_day,active_workers_metric,active_workers_sim\n");
    }
    fprintf(fp, "%s,%d,%s,%ld,%ld,%.2f,%.2f,%ld,%.2f,%ld,%ld,%ld,%ld,%.2f,%ld,%ld,%ld,%ld,%.2f,%lld,%lld,%lld,%lld,%lld,%ld,%ld,%.2f,%ld,%ld\n",
        final_row ? "final" : "daily",
        day,
        termination_name(reason),
        s->served_users,
        s->unserved_users,
        (double) s->served_users / (double) elapsed_days,
        (double) s->unserved_users / (double) elapsed_days,
        s->revenue_cents,
        (double) s->revenue_cents / (double) elapsed_days,
        s->dishes_served[STATION_PRIMI],
        s->dishes_served[STATION_SECONDI],
        s->dishes_served[STATION_COFFEE],
        dishes_total,
        (double) dishes_total / (double) elapsed_days,
        s->leftovers[STATION_PRIMI],
        s->leftovers[STATION_SECONDI],
        s->leftovers[STATION_COFFEE],
        leftovers_total,
        (double) leftovers_total / (double) elapsed_days,
        s->wait_ns[STATION_PRIMI],
        s->wait_ns[STATION_SECONDI],
        s->wait_ns[STATION_COFFEE],
        s->wait_ns[STATION_CASSA],
        total_wait_ns(s),
        wait_samples,
        s->pauses_total,
        (double) s->pauses_total / (double) elapsed_days,
        s->active_workers_total,
        unique_active_workers(shared));
    fclose(fp);
}
