# Progetto SO 2025/26 — Oasi del Golfo

## 1. Obiettivo

Il progetto simula il servizio mensa **Oasi del Golfo** con un modello **multiprocesso**: un processo *manager* coordina la simulazione, processi *worker* presidiano le stazioni, processi *user* rappresentano i clienti. La comunicazione avviene tramite **memoria condivisa System V**, **semafori System V** e, nella versione completa, una **FIFO** per comandi esterni.

L’implementazione rispetta i requisiti della versione completa (voto fino a 30): utenti con e senza ticket, gruppi che pagano insieme, refill del magazzino, statistiche su CSV, eseguibili `disorder` e `addusers`, terminazione per timeout o overload.

---

## 2. Architettura dei processi

| Eseguibile   | Ruolo |
|-------------|--------|
| `manager`   | Crea IPC, carica configurazione e menu, genera worker e utenti, avanza il tempo simulato, gestisce la FIFO, stampa report e CSV, termina e dealloca le risorse |
| `worker`    | Opera su una stazione (primi, secondi, coffee, cassa) assegnata giornalmente |
| `user`      | Simula una giornata: scelta menu, code, sincronizzazione di gruppo, cassa, pasto ai tavoli |
| `disorder`  | Invia `DISORDER` sulla FIFO: blocca temporaneamente i pagamenti in cassa |
| `addusers`  | Invia `ADD_USERS N` sulla FIFO: aggiunge N utenti in esecuzione |

Ogni processo figlio viene avviato con `fork()` + `execl()` passando l’identificativo del segmento di memoria condivisa e il proprio id logico (`worker` / `user`). Non si usa un unico eseguibile con `argv[0]` diverso: la divisione in moduli è esplicita nel `Makefile`.

Struttura sorgenti principale:

- `include/common.h`, `config.h`, `ipc.h`, `simulation.h` — tipi e API condivise
- `src/manager.c` — ciclo di vita globale
- `src/worker.c`, `src/user.c` — comportamento operativo
- `src/ipc.c`, `src/simulation.c`, `src/config.c`, `src/common.c` — infrastruttura
- `src/disorder.c`, `src/addusers.c` — client della FIFO

---

## 3. Meccanismi IPC e sincronizzazione

### 3.1 Memoria condivisa

Un unico segmento `shared_state_t` contiene:

- configurazione e menu del giorno;
- code per stazione, inventario primi/secondi, stato di ogni utente e gruppo;
- flag globali (`shutdown`, `day_open`, `disorder_active`, …);
- statistiche giornaliere (`day_stats`) e cumulative (`total_stats`).

Il manager crea la SHM con `shmget(IPC_PRIVATE, …)`; i figli la attaccano con `shmat` usando l’id passato su riga di comando. A fine simulazione: `shmdt`, `shmctl(IPC_RMID)`.

### 3.2 Semafori System V

Sono usati per evitare **attesa attiva**:

- **Code bounded buffer** per stazione: `mutex`, `items`, `slots` (produttore-consumatore tra utenti e worker);
- **Postazioni worker** (`seat_sem`): limitano quanti worker possono servire contemporaneamente su una stazione;
- **Bootstrap** (`init_sem`): ogni figlio segnala al manager di essere pronto;
- **Inizio giornata** (`day_start_sem`): il manager rilascia tutti i processi all’apertura del giorno;
- **Semaforo per utente** (`served_sem`): il worker sblocca l’utente al termine del servizio;
- **Gruppi**: `mutex` + `barrier_sem` per la barriera prima della cassa, con attesa a timeout per reagire alla chiusura giornata;
- **Lettore ticket** (`ticket_reader_sem`): capacità configurabile (`ticket_reader_capacity`) e acquisizione con timeout;
- **Tavoli** (`table_sem`): posti a sedere limitati;
- **Statistiche e inventario**: `stats_mutex`, `inventory.mutex`.

Le attese con timeout usano `semtimedop` (`semaphore_wait_timeout`) così worker e utenti reagiscono a chiusura giornata e `shutdown` senza loop di polling. All’inizio di ogni nuova giornata il manager riallinea anche i semafori contatore delle code (`items=0`, `slots=MAX_QUEUE`) oltre a `head`, `tail` e `count`, evitando residui della giornata precedente.

### 3.3 FIFO (versione completa)

Il manager crea una FIFO (`mkfifo`, path da configurazione, es. `/tmp/oasi_fifo`) e la legge in modo **non bloccante** nel loop temporale della giornata. Comandi supportati:

- `DISORDER` — blocca la cassa per `stop_duration_seconds`;
- `DISORDER <sec>` — durata personalizzata;
- `ADD_USERS <n>` — crea fino a `n` nuovi processi `user` (entro `num_users + max_dynamic_users`).

Gli eseguibili `disorder` e `addusers` aprono la FIFO in scrittura e inviano una riga di testo.

---

## 4. Modello temporale

Il tempo simulato è scalato sul tempo reale tramite `nanoseconds_per_simulated_minute`: ogni minuto simulato corrisponde a quel numero di nanosecondi reali, convertito dalla funzione `simulate_seconds_to_nanoseconds`. Una giornata dura `day_duration_minutes` minuti simulati.

Il manager, a ogni tick, può:

- leggere comandi dalla FIFO;
- eseguire refill dell’inventario se è scattato l’intervallo;
- disattivare `disorder` se è scaduto `disorder_until_ns`.

Alla fine di ogni giornata: `close_day`, report su stdout, riga su `stats.csv`. La simulazione termina se:

1. **Timeout** — completati `sim_duration_days` giorni (`config/config_timeout.conf`: 3 giorni, soglia overload molto alta);
2. **Overload** — somma delle lunghezze delle code (primi + secondi + coffee + cassa) a fine giornata supera `overload_threshold` (`config/config_overload.conf`: molti utenti, giornate brevi, servizi lenti, soglia bassa);
3. **Segnale** — `SIGINT` / `SIGTERM` sul manager (`TERM_SIGNAL`).

Il report finale indica la causa (`tempo_scaduto`, `sovraccarico`, `segnale`).

---

## 5. Comportamento funzionale

### 5.1 Stazioni e magazzino

Quattro stazioni: primi, secondi, coffee, cassa. Primi e secondi attingono da un **inventario** finito per variante di menu; il coffee è trattato come illimitato. A inizio giornata l’inventario viene riempito con `avg_refill_primi` / `avg_refill_secondi` per piatto; durante il giorno il manager esegue **refill periodici** ogni `avg_refill_time` minuti simulati (±20% calcolato con `add_random_variation`), senza superare `max_porzioni_*`. Se il piatto richiesto non c’è, il worker prova un’altra variante dello stesso reparto. I prezzi applicati in cassa non sono letti dal menu, ma dai parametri `price_primi`, `price_secondi` e `price_coffee` nel file di configurazione.

### 5.2 Assegnazione operatori

Ogni giornata il manager ricalcola `worker_assignments`: **almeno un worker per stazione**; i restanti vengono assegnati iterativamente alla stazione con peso maggiore, inizialmente derivato dai tempi medi di servizio `avg_service_time_primi_seconds`, `avg_service_time_secondi_seconds`, `avg_service_time_coffee_seconds` e `avg_service_time_cassa_seconds`, dimezzando il peso dopo ogni assegnazione per evitare concentrazioni infinite.

I worker competono per le `seat_sem` della propria stazione; possono fare **pause** casuali (limite `max_pauses_per_day` al giorno) solo se resta almeno un altro operatore attivo sulla stazione.

### 5.3 Processo utente

Per ogni giornata simulata:

1. Con probabilità 20% l’utente **non si presenta**.
2. Sceglie casualmente primo, secondo e (70%) coffee dal menu.
3. Visita le stazioni cibo ancora necessarie scegliendo ogni volta quella con **coda più corta** (`choose_next_station`).
4. Si sincronizza col **gruppo** (`group_arrive` / `group_wait_ready`): tutti i presenti del gruppo devono aver finito le stazioni cibo prima che qualcuno vada in cassa.
5. Se non ha ottenuto piatti, è conteggiato come non servito.
6. Eventuale **lettura ticket**, poi **cassa** (con sconto se ha ticket).
7. **Pasto ai tavoli** (`table_sem`), tempo proporzionale ai piatti acquistati.

I tempi di servizio e pausa usano `add_random_variation` rispetto alle medie di configurazione.

### 5.4 Utenti con ticket e senza ticket (requisito versione completa)

La distinzione è modellata nel campo `has_ticket` di `user_state_t`:

- All’**avvio**, circa **80%** degli utenti riceve `has_ticket = 1` (estrazione uniforme 0–99).
- Gli utenti **con ticket** devono prima passare dal **lettore ticket**: semaforo a capacità `ticket_reader_capacity` (nelle config fornite: 1 lettore), con tempo di lettura simulato (2 secondi simulati). Solo dopo vanno in cassa.
- In cassa applicano lo **sconto** `ticket_discount_percent` (25% nelle config di esempio) sul totale accumulato dalle stazioni cibo.
- Gli utenti **senza ticket** saltano la fase lettura e pagano il prezzo pieno.

Questo introduce una coda implicita aggiuntiva (contesa sul semaforo lettore) oltre alle code delle stazioni, come richiesto dal diagramma della specifica (stazione “ticket” logica prima della cassa).

### 5.5 Gruppi

All’avvio il manager forma gruppi consecutivi di dimensione casuale in `[1, max_users_per_group]`. I membri condividono `group_id` e una **barriera** prima della cassa: solo quando tutti i partecipanti presenti quel giorno hanno completato primi/secondi/coffee procedono insieme al pagamento. La barriera usa attese temporizzate, quindi se la giornata termina prima che il gruppo sia completo gli utenti escono dal percorso e vengono conteggiati come non serviti.

Gli utenti aggiunti dinamicamente con `addusers` formano **gruppi monoutente** (`group_id = id`) per non alterare i gruppi già formati.

### 5.6 Communication disorder

`disorder` imposta `disorder_active` e `disorder_until_ns` nella memoria condivisa. I worker della **cassa** non prelevano utenti dalla coda finché il disorder è attivo: attendono con `sleep_ns` a tick brevi, simulando l’interruzione dei pagamenti automatici per `stop_duration_seconds` (o durata passata sulla FIFO).

### 5.7 Statistiche e CSV

A fine giornata e a fine simulazione il manager stampa su stdout utenti serviti/non serviti, piatti per stazione, avanzi, tempi medi di attesa in coda (ms), pause, operatori attivi, ricavi.

Le stesse metriche (più campi grezzi per le attese) sono appendate in `stats.csv` (`csv_path` in configurazione): righe `daily` per ogni giorno e riga `final` con causa di terminazione.

---

## 6. Configurazione

Parametri in file testo `chiave=valore` (commenti `#` ignorati). Esempi consegnati:

| File | Scopo |
|------|--------|
| `config/config_timeout.conf` | `sim_duration_days=3`, `overload_threshold=999` → termine per **scadenza giorni** |
| `config/config_overload.conf` | Molti utenti, giornate corte, servizi lenti, `overload_threshold=6` → termine per **sovraccarico code** |

Altri parametri rilevanti: `num_workers`, `num_users`, `max_dynamic_users`, prezzi, tempi medi servizio/refill, postazioni worker per stazione (`num_worker_seats_*`), `fifo_path`, `menu_path`.

Il menu è in `config/menu.txt` (sezioni `PRIMI:`, `SECONDI:`, `COFFEE:` con i nomi delle alternative disponibili). Il parser valida che siano presenti almeno due primi, due secondi e quattro varianti coffee; i prezzi di categoria restano nel file di configurazione.

**Nota:** la specifica consente anche variabili d’ambiente; nell’implementazione attuale i parametri sono letti **solo dal file** passato al manager (`./manager config/...`). Cambiare configurazione non richiede ricompilazione.

---

## 7. Compilazione ed esecuzione

```bash
make
./manager config/config_timeout.conf
```

In un secondo terminale (stesso `fifo_path` del file di config):

```bash
./disorder /tmp/oasi_fifo
./addusers /tmp/oasi_fifo 5
```

Per test overload:

```bash
./manager config/config_overload.conf
```

Dettaglio comandi: `legenda_comandi.txt`.

---

## 8. Scelte progettuali — sintesi

| Aspetto | Scelta |
|--------|--------|
| IPC | SHM unica + semafori System V + FIFO named pipe |
| Code | Bounded buffer con semafori contatore (no busy-wait) |
| Scelta stazione utente | Minimizza lunghezza coda osservata in SHM |
| Worker | Riassegnazione giornaliera; priorità a servizi più lenti |
| Ticket | 80% utenti; semaforo lettore + sconto in cassa |
| Gruppi | Barriera prima cassa; cardinalità casuale all’avvio |
| Inventario | Finito primi/secondi, refill periodico ±20%, coffee illimitato |
| Terminazione | Giorni simulati / overload code / segnale |
| Modularità | Eseguibili separati, `make`, deallocazione IPC in uscita manager |
| Estensioni | `disorder`, `addusers`, export `stats.csv` |

---

## 9. Requisiti implementativi (checklist)

- **Niente attesa attiva** — attese su semafori e `nanosleep` / `semtimedop`
- **SHM + semafori + pipe** — FIFO per controllo esterno
- **Processi distinti** — `exec` su `manager`, `worker`, `user`, …
- **Make** — target `all` e `clean`
- **Concorrenza** — tutti i worker e utenti operano in parallelo sulle rispettive code
- **Cleanup IPC** — `shared_destroy`, rimozione SHM e unlink FIFO a termine simulazione
- **Flag richiesti** — compilazione con `-D_GNU_SOURCE`, `-Wvla`, `-Wextra`, `-Werror`

---

## 10. Confronto con la consegna

| Requisito della consegna | Stato nel progetto |
|--------------------------|--------------------|
| Responsabile mensa che crea IPC, worker, utenti, stazioni, tavoli e statistiche | Implementato in `manager` |
| Simulazione avviata solo dopo inizializzazione dei figli | Implementato con `init_sem` |
| Almeno 4 stazioni: primi, secondi, coffee, cassa | Implementato |
| Postazioni worker per stazione configurabili | Implementato con `seat_sem` |
| Almeno un worker per stazione e assegnazione giornaliera | Implementato da `assign_workers` |
| Tempi medi di servizio con variazione richiesta | Implementato: ±50% primi/secondi, ±80% coffee, ±20% cassa |
| Pause worker senza lasciare stazione non presidiata | Implementato in `maybe_pause` |
| Menu modificabile a runtime con almeno 2 primi, 2 secondi, 4 coffee | Implementato e validato |
| Inventario finito primi/secondi e refill periodico ±20% | Implementato |
| Utente sceglie menu, code, cassa, tavoli | Implementato |
| Rinuncia a fine giornata se non servito | Implementato in `close_day` e nei timeout utente |
| Terminazione per timeout | Dimostrata da `config/config_timeout.conf` |
| Terminazione per overload | Dimostrata da `config/config_overload.conf` |
| Causa di terminazione e statistiche finali | Implementato nel report finale |
| Eseguibile `disorder` per blocco cassa | Implementato tramite FIFO |
| Eseguibile `addusers` per aggiungere utenti | Implementato tramite FIFO |
| Statistiche salvate in CSV | Implementato in `stats.csv` |
| Utenti con ticket più numerosi, sconto e lettore ticket | Implementato con probabilità 80%, `ticket_reader_sem` e sconto |
| Gruppi fino a `max_users_per_group` prima della cassa | Implementato con barriera di gruppo |
| Configurazione a runtime senza ricompilare | Implementato tramite file `key=value`; non sono usate variabili d'ambiente |

---

*Relazione del progetto laboratorio SO 2025/26 — simulazione mensa Oasi del Golfo.*
