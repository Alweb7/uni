/* API del parser di configurazione e del loader del menu. */
#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

/* Inizializza la configurazione con i valori di default del simulatore. */
void config_defaults(config_t *cfg);
/* Carica la configurazione runtime da file, sovrascrivendo i default. */
void config_load(const char *path, config_t *cfg);
/* Carica il menu giornaliero dal file di configurazione dedicato. */
void menu_load(const char *path, menu_data_t *menu);
/* Verifica che configurazione e menu rispettino i vincoli minimi del progetto. */
void config_validate(const config_t *cfg, const menu_data_t *menu);

#endif
