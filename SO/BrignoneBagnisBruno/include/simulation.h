#ifndef SIMULATION_H
#define SIMULATION_H

#include "ipc.h"

/** @brief Prepara le strutture condivise per l'inizio di una nuova giornata di simulazione.
 * @param shared Puntatore allo stato condiviso da preparare.
 */
void setup_day(shared_state_t *shared);


/** @brief Chiude la giornata corrente consolidando statistiche e avanzi.
 * @param shared Puntatore allo stato condiviso.
 */
void close_day(shared_state_t *shared);


/** @brief Esegue il refill delle stazioni quando il timer simulato raggiunge la prossima soglia prevista.
 * @param shared Puntatore allo stato condiviso.
 * @param elapsed_ns Tempo trascorso dalla precedente operazione di refill.
 * @param next_refill_ns Puntatore al timestamp della prossima operazione di refill.
 */
void refill_inventory_if_needed(shared_state_t *shared, long long elapsed_ns, long long *next_refill_ns);


/** @brief Assegna i worker alle stazioni all'inizio di ogni giornata, bilanciando il carico in base ai tempi medi di servizio.
 * @param shared Puntatore allo stato condiviso.
 */
void assign_workers(shared_state_t *shared);


/** @brief Stampa su stream il report giornaliero o finale della simulazione.
 * @param out Puntatore allo stream di output.
 * @param shared Puntatore allo stato condiviso.
 * @param day Numero della giornata.
 * @param final_report Flag che indica se stampare un report finale.
 * @param reason Motivo di terminazione della simulazione.
 */
void print_day_report(FILE *out, const shared_state_t *shared, int day, bool final_report, termination_reason_t reason);


/** @brief Appende una riga CSV con le statistiche giornaliere o finali.
 * @param shared Puntatore allo stato condiviso.
 * @param day Numero della giornata.
 * @param final_row Flag che indica se appendere una riga finale.
 * @param reason Motivo di terminazione della simulazione.
 */
void append_csv_row(const shared_state_t *shared, int day, bool final_row, termination_reason_t reason);


/** @brief Inserisce un utente nella coda della stazione indicata.
 * @param queue Puntatore alla coda della stazione.
 * @param user_id Identificativo dell'utente da inserire.
 * @return true se l'utente è stato inserito con successo, false altrimenti.
 */
bool enqueue_user(station_queue_t *queue, int user_id);


/** @brief Estrae un utente dalla coda entro il timeout indicato.
 * @param queue Puntatore alla coda della stazione.
 * @param user_id Puntatore all'identificativo dell'utente estratto.
 * @param timeout_ns Timeout in nanosecondi.
 * @return true se un utente è stato estratto con successo, false altrimenti.
 */
bool dequeue_user_timed(station_queue_t *queue, int *user_id, long long timeout_ns);


/** @brief Registra il tempo di attesa accumulato da un utente su una stazione.
 * @param shared Puntatore allo stato condiviso.
 * @param station Tipo di stazione.
 * @param wait_ns Tempo di attesa in nanosecondi.
 */
void record_wait(shared_state_t *shared, station_type_t station, long long wait_ns);


/** @brief Aggiorna le statistiche di distribuzione per la stazione indicata.
 * @param shared Puntatore allo stato condiviso.
 * @param station Tipo di stazione.
 */
void record_dish_served(shared_state_t *shared, station_type_t station);


/** @brief Registra un utente come non servito per la giornata corrente.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 */
void record_unserved_user(shared_state_t *shared, user_state_t *user);


/** @brief Registra un utente come servito per la giornata corrente.
 * @param shared Puntatore allo stato condiviso.
 * @param user Puntatore allo stato dell'utente.
 */
void record_served_user(shared_state_t *shared, user_state_t *user);


/** @brief Aggiorna le statistiche relative alle pause effettuate da un worker.
 * @param shared Puntatore allo stato condiviso.
 * @param worker_id Identificativo del worker.
 */
void record_pause(shared_state_t *shared, int worker_id);


/** @brief Aggiunge al totale giornaliero l'incasso di un pagamento andato a buon fine.
 * @param shared Puntatore allo stato condiviso.
 * @param cents Importo in centesimi.
 */
void record_revenue(shared_state_t *shared, int cents);

#endif
