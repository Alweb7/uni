/* Primitive di supporto al ciclo giornaliero e alla raccolta statistiche. */
#ifndef SIMULATION_H
#define SIMULATION_H

#include "ipc.h"

/* Prepara le strutture condivise per l'inizio di una nuova giornata di simulazione. */
void setup_day(shared_state_t *shared);
/* Chiude la giornata corrente consolidando statistiche e avanzi. */
void close_day(shared_state_t *shared);
/* Esegue il refill delle stazioni quando il timer simulato raggiunge la prossima soglia prevista. */
void refill_inventory_if_needed(shared_state_t *shared, long long elapsed_ns, long long *next_refill_ns);
/* Assegna i worker alle stazioni secondo la politica definita dal simulatore. */
void assign_workers(shared_state_t *shared);
/* Stampa su stream il report giornaliero o finale della simulazione. */
void print_day_report(FILE *out, const shared_state_t *shared, int day, bool final_report, termination_reason_t reason);
/* Appende una riga CSV con le statistiche giornaliere o finali. */
void append_csv_row(const shared_state_t *shared, int day, bool final_row, termination_reason_t reason);
/* Inserisce un utente nella coda della stazione indicata. */
bool enqueue_user(station_queue_t *queue, int user_id);
/* Estrae un utente dalla coda entro il timeout indicato. */
bool dequeue_user_timed(station_queue_t *queue, int *user_id, long long timeout_ns);
/* Registra il tempo di attesa accumulato da un utente su una stazione. */
void record_wait(shared_state_t *shared, station_type_t station, long long wait_ns);
/* Aggiorna le statistiche di distribuzione per la stazione indicata. */
void record_dish_served(shared_state_t *shared, station_type_t station);
/* Registra un utente come non servito per la giornata corrente. */
void record_unserved_user(shared_state_t *shared, user_state_t *user);
/* Registra un utente come servito per la giornata corrente. */
void record_served_user(shared_state_t *shared, user_state_t *user);
/* Aggiorna le statistiche relative alle pause effettuate da un worker. */
void record_pause(shared_state_t *shared, int worker_id);
/* Aggiunge al totale giornaliero l'incasso di un pagamento andato a buon fine. */
void record_revenue(shared_state_t *shared, int cents);

#endif
