#include "config.h"

/* Rimuove spazi e terminatori superflui dall'inizio e dalla fine di una stringa. */
static void trim(char *s) {
    /* len serve per scorrere all'indietro la coda della stringa. */
    size_t len;
    /* start punta al primo carattere non-spazio trovato in testa. */
    char *start = s;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }
    len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

/* Converte una stringa in intero e termina il programma se il valore non è valido. */
static int parse_int_or_die(const char *key, const char *value) {
    char *end = NULL;
    long parsed;
    if (value[0] == '\0') {
        exit_with_message("config: missing value for %s", key);
    }
    errno = 0;
    parsed = strtol(value, &end, 10);
    if (errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX || end == value || *end != '\0') {
        exit_with_message("config: invalid integer for %s: %s", key, value);
    }
    return (int) parsed;
}

/* Copia un valore stringa assicurandosi che non sia vuoto né troncato. */
static void copy_string_field(char *dst, size_t dst_size, const char *field_name, const char *value) {
    if (value[0] == '\0') {
        exit_with_message("config: missing value for %s", field_name);
    }
    if (strlen(value) >= dst_size) {
        exit_with_message("config: value too long for %s", field_name);
    }
    snprintf(dst, dst_size, "%s", value);
}

void config_defaults(config_t *cfg) {
    /* Si parte sempre da una base coerente così i file parziali restano validi. */
    memset(cfg, 0, sizeof(*cfg));
    cfg->sim_duration_days = 3;
    cfg->nanoseconds_per_simulated_minute = 50000000;
    cfg->day_duration_minutes = 120;
    cfg->num_workers = 6;
    cfg->num_users = 18;
    cfg->max_dynamic_users = 16;
    cfg->max_users_per_group = 3;
    cfg->overload_threshold = 8;
    cfg->max_pauses_per_day = 2;
    cfg->avg_pause_seconds = 5;
    cfg->stop_duration_seconds = 15;
    cfg->ticket_discount_percent = 25;
    cfg->ticket_reader_capacity = 1;
    cfg->avg_service_time_primi_seconds = 6;
    cfg->avg_service_time_secondi_seconds = 8;
    cfg->avg_service_time_coffee_seconds = 3;
    cfg->avg_service_time_cassa_seconds = 4;
    cfg->avg_refill_primi = 12;
    cfg->avg_refill_secondi = 12;
    cfg->avg_refill_time = 10;
    cfg->max_porzioni_primi = 20;
    cfg->max_porzioni_secondi = 20;
    cfg->num_worker_seats_primi = 2;
    cfg->num_worker_seats_secondi = 2;
    cfg->num_worker_seats_coffee = 1;
    cfg->num_worker_seats_cassa = 1;
    cfg->num_table_seats = 10;
    cfg->price_primi = 650;
    cfg->price_secondi = 850;
    cfg->price_coffee = 120;
    snprintf(cfg->menu_path, sizeof(cfg->menu_path), "config/menu.txt");
    snprintf(cfg->fifo_path, sizeof(cfg->fifo_path), "/tmp/oasi_fifo");
    snprintf(cfg->csv_path, sizeof(cfg->csv_path), "stats.csv");
}

/* Aggiorna un campo intero della configurazione in base alla chiave letta dal file. */
static bool set_int_field(config_t *cfg, const char *key, const char *value) {
#define SET_INT(name) \
    /* Mappa in modo compatto nome testuale -> campo numerico della struct. */ \
    if (strcmp(key, #name) == 0) { \
        cfg->name = parse_int_or_die(#name, value); \
        return true; \
    }
    SET_INT(sim_duration_days)
    SET_INT(nanoseconds_per_simulated_minute)
    SET_INT(day_duration_minutes)
    SET_INT(num_workers)
    SET_INT(num_users)
    SET_INT(max_dynamic_users)
    SET_INT(max_users_per_group)
    SET_INT(overload_threshold)
    SET_INT(max_pauses_per_day)
    SET_INT(avg_pause_seconds)
    SET_INT(stop_duration_seconds)
    SET_INT(ticket_discount_percent)
    SET_INT(ticket_reader_capacity)
    SET_INT(avg_service_time_primi_seconds)
    SET_INT(avg_service_time_secondi_seconds)
    SET_INT(avg_service_time_coffee_seconds)
    SET_INT(avg_service_time_cassa_seconds)
    SET_INT(avg_refill_primi)
    SET_INT(avg_refill_secondi)
    SET_INT(avg_refill_time)
    SET_INT(max_porzioni_primi)
    SET_INT(max_porzioni_secondi)
    SET_INT(num_worker_seats_primi)
    SET_INT(num_worker_seats_secondi)
    SET_INT(num_worker_seats_coffee)
    SET_INT(num_worker_seats_cassa)
    SET_INT(num_table_seats)
    SET_INT(price_primi)
    SET_INT(price_secondi)
    SET_INT(price_coffee)
#undef SET_INT
    return false;
}

void config_load(const char *path, config_t *cfg) {
    /* fp è il file di configurazione aperto in lettura. */
    FILE *fp;
    /* line contiene una riga del file alla volta. */
    char line[256];
    /* line_no tiene traccia della riga corrente per messaggi d'errore più chiari. */
    int line_no = 0;
    config_defaults(cfg);
    fp = fopen(path, "r");
    if (fp == NULL) {
        exit_with_errno("fopen config");
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* eq individua il separatore '=' tra chiave e valore. */
        char *eq;
        /* known segnala se la chiave letta appartiene davvero alla configurazione supportata. */
        bool known = false;
        line_no++;
        /* Ogni riga valida segue il formato chiave=valore con spazi opzionali. */
        trim(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        eq = strchr(line, '=');
        if (eq == NULL) {
            exit_with_message("config: invalid line %d, expected key=value", line_no);
        }
        *eq = '\0';
        trim(line);
        trim(eq + 1);
        known = set_int_field(cfg, line, eq + 1);
        /* I campi stringa vengono gestiti separatamente rispetto ai campi interi. */
        if (strcmp(line, "menu_path") == 0) {
            copy_string_field(cfg->menu_path, sizeof(cfg->menu_path), "menu_path", eq + 1);
            known = true;
        } else if (strcmp(line, "fifo_path") == 0) {
            copy_string_field(cfg->fifo_path, sizeof(cfg->fifo_path), "fifo_path", eq + 1);
            known = true;
        } else if (strcmp(line, "csv_path") == 0) {
            copy_string_field(cfg->csv_path, sizeof(cfg->csv_path), "csv_path", eq + 1);
            known = true;
        }
        if (!known) {
            exit_with_message("config: unknown key at line %d: %s", line_no, line);
        }
    }
    fclose(fp);
}

/* Interpreta una riga del file menu e popola la sezione corrispondente della struttura menu. */
static void parse_menu_line(menu_data_t *menu, char *line) {
    /* colon separa il nome della sezione dalla lista dei piatti. */
    char *colon = strchr(line, ':');
    /* token scorre i piatti separati da virgola nella sezione corrente. */
    char *token;
    /* count punta al contatore della sezione scelta, target al relativo array. */
    int *count = NULL;
    dish_t *target = NULL;
    /* La sezione corrente determina quale array di piatti andremo a popolare. */
    if (colon == NULL) {
        return;
    }
    *colon = '\0';
    trim(line);
    trim(colon + 1);
    if (strcmp(line, "PRIMI") == 0) {
        count = &menu->primi_count;
        target = menu->primi;
    } else if (strcmp(line, "SECONDI") == 0) {
        count = &menu->secondi_count;
        target = menu->secondi;
    } else if (strcmp(line, "COFFEE") == 0) {
        count = &menu->coffee_count;
        target = menu->coffee;
    } else {
        return;
    }
    /* I piatti sono separati da virgole e vengono copiati in ordine di apparizione. */
    token = strtok(colon + 1, ",");
    while (token != NULL && *count < MAX_DISHES_PER_TYPE) {
        trim(token);
        snprintf(target[*count].name, sizeof(target[*count].name), "%s", token);
        (*count)++;
        token = strtok(NULL, ",");
    }
}

void menu_load(const char *path, menu_data_t *menu) {
    /* fp è il file menu, line la riga corrente letta dal parser. */
    FILE *fp;
    char line[256];
    memset(menu, 0, sizeof(*menu));
    fp = fopen(path, "r");
    if (fp == NULL) {
        exit_with_errno("fopen menu");
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        trim(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        parse_menu_line(menu, line);
    }
    fclose(fp);
}

void config_validate(const config_t *cfg, const menu_data_t *menu) {
    /* Questi controlli impediscono di avviare una simulazione strutturalmente impossibile. */
    if (cfg->sim_duration_days <= 0) {
        exit_with_message("sim_duration_days must be > 0");
    }
    if (cfg->nanoseconds_per_simulated_minute <= 0) {
        exit_with_message("nanoseconds_per_simulated_minute must be > 0");
    }
    if (cfg->day_duration_minutes <= 0) {
        exit_with_message("day_duration_minutes must be > 0");
    }
    if (cfg->num_workers < STATION_COUNT || cfg->num_workers > MAX_WORKERS) {
        exit_with_message("num_workers must be between %d and %d", STATION_COUNT, MAX_WORKERS);
    }
    if (cfg->num_users <= 0 || cfg->num_users > MAX_USERS) {
        exit_with_message("num_users must be between 1 and %d", MAX_USERS);
    }
    if (cfg->max_dynamic_users < 0) {
        exit_with_message("max_dynamic_users must be >= 0");
    }
    if ((long long) cfg->num_users + cfg->max_dynamic_users > MAX_USERS) {
        exit_with_message("num_users + max_dynamic_users must be <= %d", MAX_USERS);
    }
    if (cfg->max_users_per_group <= 0 || cfg->max_users_per_group > MAX_USERS) {
        exit_with_message("max_users_per_group must be between 1 and %d", MAX_USERS);
    }
    if (cfg->overload_threshold < 0) {
        exit_with_message("overload_threshold must be >= 0");
    }
    if (cfg->max_pauses_per_day < 0) {
        exit_with_message("max_pauses_per_day must be >= 0");
    }
    if (cfg->avg_pause_seconds < 0) {
        exit_with_message("avg_pause_seconds must be >= 0");
    }
    if (cfg->max_pauses_per_day > 0 && cfg->avg_pause_seconds == 0) {
        exit_with_message("avg_pause_seconds must be > 0 when max_pauses_per_day > 0");
    }
    if (cfg->stop_duration_seconds < 0) {
        exit_with_message("stop_duration_seconds must be >= 0");
    }
    if (cfg->ticket_discount_percent < 0 || cfg->ticket_discount_percent > 100) {
        exit_with_message("ticket_discount_percent must be between 0 and 100");
    }
    if (cfg->ticket_reader_capacity <= 0) {
        exit_with_message("ticket_reader_capacity must be > 0");
    }
    if (cfg->avg_service_time_primi_seconds <= 0) {
        exit_with_message("avg_service_time_primi_seconds must be > 0");
    }
    if (cfg->avg_service_time_secondi_seconds <= 0) {
        exit_with_message("avg_service_time_secondi_seconds must be > 0");
    }
    if (cfg->avg_service_time_coffee_seconds <= 0) {
        exit_with_message("avg_service_time_coffee_seconds must be > 0");
    }
    if (cfg->avg_service_time_cassa_seconds <= 0) {
        exit_with_message("avg_service_time_cassa_seconds must be > 0");
    }
    if (cfg->avg_refill_primi <= 0) {
        exit_with_message("avg_refill_primi must be > 0");
    }
    if (cfg->avg_refill_secondi <= 0) {
        exit_with_message("avg_refill_secondi must be > 0");
    }
    if (cfg->avg_refill_time <= 0) {
        exit_with_message("avg_refill_time must be > 0");
    }
    if (cfg->max_porzioni_primi <= 0) {
        exit_with_message("max_porzioni_primi must be > 0");
    }
    if (cfg->max_porzioni_secondi <= 0) {
        exit_with_message("max_porzioni_secondi must be > 0");
    }
    if (cfg->avg_refill_primi > cfg->max_porzioni_primi) {
        exit_with_message("avg_refill_primi must be <= max_porzioni_primi");
    }
    if (cfg->avg_refill_secondi > cfg->max_porzioni_secondi) {
        exit_with_message("avg_refill_secondi must be <= max_porzioni_secondi");
    }
    if (cfg->num_worker_seats_primi <= 0) {
        exit_with_message("num_worker_seats_primi must be > 0");
    }
    if (cfg->num_worker_seats_secondi <= 0) {
        exit_with_message("num_worker_seats_secondi must be > 0");
    }
    if (cfg->num_worker_seats_coffee <= 0) {
        exit_with_message("num_worker_seats_coffee must be > 0");
    }
    if (cfg->num_worker_seats_cassa <= 0) {
        exit_with_message("num_worker_seats_cassa must be > 0");
    }
    if (cfg->num_table_seats <= 0) {
        exit_with_message("num_table_seats must be > 0");
    }
    if (cfg->price_primi < 0) {
        exit_with_message("price_primi must be >= 0");
    }
    if (cfg->price_secondi < 0) {
        exit_with_message("price_secondi must be >= 0");
    }
    if (cfg->price_coffee < 0) {
        exit_with_message("price_coffee must be >= 0");
    }
    if (cfg->menu_path[0] == '\0') {
        exit_with_message("menu_path must not be empty");
    }
    if (cfg->fifo_path[0] == '\0') {
        exit_with_message("fifo_path must not be empty");
    }
    if (cfg->csv_path[0] == '\0') {
        exit_with_message("csv_path must not be empty");
    }
    if (menu->primi_count < 2 || menu->secondi_count < 2 || menu->coffee_count < 4) {
        exit_with_message("menu must contain at least 2 primi, 2 secondi and 4 coffee variants");
    }
}
