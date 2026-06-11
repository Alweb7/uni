# Relazione - Esercizio 3

## Obiettivo

L'obiettivo dell'esercizio e' implementare una struttura dati `PriorityQueue` generica basata su heap ternario massimo e usarla per simulare uno scheduler non preemptive a priorita'. Il progetto comprende:

- la libreria della coda con priorita'
- un programma principale che legge un file CSV di task e produce il piano di esecuzione
- un eseguibile separato per gli unit test

## Struttura del progetto

- `include/pq.h`: interfaccia pubblica della priority queue
- `include/hash.h`: interfaccia della tabella hash di supporto
- `src/pq.c`: implementazione della priority queue con heap ternario
- `src/hash.c`: implementazione della tabella hash
- `src/main.c`: simulazione dello scheduler
- `src/test.c`: unit test automatici
- `Makefile`: compilazione degli eseguibili richiesti

## Scelte implementative

### Priority queue

La `PriorityQueue` e' implementata tramite:

- un array dinamico di `void*` che rappresenta lo heap ternario
- una funzione `compare` fornita dal chiamante per stabilire la priorita' tra due elementi
- una funzione `hash` fornita dal chiamante per associare ogni elemento alla sua posizione nello heap

L'heap e' massimo: l'elemento in testa e' sempre quello con priorita' piu' alta.

La scelta di usare `void*` rende la struttura generica, mentre l'array dinamico consente di gestire un numero di elementi non noto a priori.

### Supporto con tabella hash

Per rendere efficienti `contains` e `remove`, oltre allo heap viene usata una tabella hash con indirizzamento aperto e linear probing. La tabella memorizza per ogni elemento il suo indice corrente nello heap.

Questa scelta evita ricerche lineari e permette di mantenere le complessita' richieste dalla consegna. La hash table effettua anche il ridimensionamento automatico quando il fattore di carico cresce troppo.

Per evitare ambiguita', lo stesso puntatore non puo' essere inserito due volte nella coda.

### Scheduler

Il programma principale legge un file CSV nel formato:

```text
ID,Start,Length,Priority
```

I task devono essere gia' ordinati per `Start`, come richiesto dalla consegna. Durante la simulazione:

1. tutti i task con `Start <= t` vengono inseriti nella priority queue
2. se la coda e' vuota, il tempo viene portato direttamente al prossimo arrivo
3. altrimenti si estrae il task con priorita' massima
4. il task viene eseguito senza interruzioni fino a `t + Length`

L'output viene scritto nel formato:

```text
ID,StartTime,EndTime
```

## Complessita'

Le operazioni principali della priority queue hanno le seguenti complessita':

- `priority_queue_top`: `O(1)`
- `priority_queue_size`: `O(1)`
- `priority_queue_contains`: `O(1)` atteso
- `priority_queue_push`: `O(log n)`
- `priority_queue_pop`: `O(log n)`
- `priority_queue_remove`: `O(log n)`

La simulazione completa dello scheduler ha complessita' `O(n log n)` nel caso generale, con uso di memoria `O(n)` nel caso peggiore.

## Gestione degli errori

Sono stati gestiti i principali casi di errore:

- argomenti nulli nelle funzioni della libreria
- inserimento duplicato dello stesso puntatore nella coda
- errori di allocazione dinamica
- file CSV malformato
- file di input non ordinato per `Start`
- lunghezze dei task negative

Quando appropriato, le funzioni della libreria restituiscono `-1` per segnalare argomenti non validi o operazioni non eseguibili.

## Unit test

L'eseguibile `test_ex3` esegue una serie di test automatici su:

- inserimento, accesso al massimo e rimozione dalla coda
- correttezza di `contains` e `remove`
- gestione di input non validi
- comportamento su migliaia di elementi, per verificare ridimensionamento e mantenimento dell'ordine

I test sono stati scritti con `assert`, in modo da non richiedere librerie esterne.

## Compilazione ed esecuzione

Per compilare:

```bash
make all
```

Questo comando produce nella cartella `bin` i due eseguibili richiesti:

- `main_ex3`
- `test_ex3`

Per eseguire il programma principale:

```bash
./bin/main_ex3 tasks.csv out.csv
```

Per eseguire i test:

```bash
./bin/test_ex3
```

## Risultati sperimentali

Nel workspace corrente e' stata eseguita una prova sul file `tasks.csv` contenente `1.000.000` di righe. Il programma ha generato correttamente il file di output in circa `2.96 s`.

Questo risultato e' coerente con quanto atteso da una soluzione basata su heap ternario e tabella hash di supporto: le operazioni di selezione del task successivo e di aggiornamento della struttura restano efficienti anche su input molto grandi.

## Considerazioni finali

La soluzione soddisfa i requisiti richiesti dalla consegna:

- priority queue generica
- heap ternario
- supporto efficiente a `contains` e `remove`
- programma di scheduling non preemptive a priorita'
- unit test separati
- `Makefile` per produrre gli eseguibili richiesti

Particolare attenzione e' stata data a leggibilita', robustezza e separazione tra interfacce pubbliche e dettagli interni di implementazione.
