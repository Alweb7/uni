/* API del parser di configurazione e del loader del menu. */
#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

/** @brief Inizializza la configurazione con i valori di default del simulatore. 
 * @param cfg Puntatore alla struttura di configurazione da inizializzare.
 */
void config_defaults(config_t *cfg);


/** @brief Carica la configurazione runtime da file, sovrascrivendo i default. 
 * @param path Percorso del file di configurazione.
 * @param cfg Puntatore alla struttura di configurazione da aggiornare.
 */
void config_load(const char *path, config_t *cfg);


/** @brief Carica il menu giornaliero dal file di configurazione dedicato. 
 * @param path Percorso del file di configurazione del menu.
 * @param menu Puntatore alla struttura di dati del menu da aggiornare.
 */
void menu_load(const char *path, menu_data_t *menu);


/** @brief Verifica che configurazione e menu rispettino i vincoli minimi del progetto. 
 * @param cfg Puntatore alla struttura di configurazione.
 * @param menu Puntatore alla struttura di dati del menu.
 */
void config_validate(const config_t *cfg, const menu_data_t *menu);

#endif
