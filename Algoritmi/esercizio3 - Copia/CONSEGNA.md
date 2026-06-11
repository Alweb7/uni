
## Esercizio 3 - Heap ternario

Si implementi la struttura dati *coda con priorità (PriorityQueue)* tramite **heap ternario**, che è strutturato come lo heap binario e con la stessa proprietà di heap (il nodo padre deve avere priorità maggiore di tutti i suoi figli) ma in cui ogni nodo ha tre figli invece che due.

La struttura dati deve consentire un numero qualunque e non noto a priori di elementi e accettare valori di tipo generico. Per confrontarli, la coda deve accettare una funzione di tipo _compare_, come quelle usate nei precedenti esercizi, che confronti la priorità degli elementi.

La struttura dati deve offrire almeno le seguenti funzionalità (ricavare il significato delle varie funzioni e procedure e dei loro parametri a partire dai loro prototipi e da quanto studiato nella parte di teoria del corso):

```
PriorityQueue* priority_queue_create(int (*f1)(const void*,const void*), unsigned long (*f2)(const void*));
int priority_queue_push(PriorityQueue*, void*);
int priority_queue_contains(const PriorityQueue*, const void*);
void* priority_queue_top(const PriorityQueue*);
void priority_queue_pop(PriorityQueue*);
int priority_queue_remove(PriorityQueue*, const void*);
int priority_queue_size(const PriorityQueue*);
void priority_queue_free(PriorityQueue*);
```

Le funzionalità devono tutte avere la complessità ottima consentita dall'heap ternario (in particolare, nessuna deve avere complessità lineare). Dove opportuno, si consideri di restituire `-1` per indicare un valore invalido degli argomenti.

*Suggerimento*: per implementare i metodi `contains` e `remove` è necessario usare una struttura dati di appoggio oltre allo heap (da non implementare da zero).

### Unit Testing

Implementare gli unit-test degli algoritmi secondo le indicazioni suggerite nel documento [Unit Testing](UnitTesting.md).

### Uso delle funzioni implementate

Si implementi un programma che utilizza la `PriorityQueue` per simulare un **sistema di scheduling** di processi (o task) secondo un modello **non preemptive con priorità**.

Il programma principale dovrà leggere un file CSV che descrive l’insieme dei task da pianificare, **già ordinati per tempo di inizio**. Ogni riga descrive un task con i seguenti campi:

* `ID`: (int) identificatore univoco del task;
* `Start`: (int) tempo di arrivo del task nel sistema;
* `Length`: (int) durata di esecuzione del task;
* `Priority`: (int) priorità del task (più alto = più urgente).

Il programma dovrà simulare un **esecutore singolo** che segue questa logica:

1. Al tempo iniziale `t = 0`, l’esecutore è libero.
2. Quando è libero, se non ci sono task da eseguire, attende fino al tempo seguente `t + 1` senza agire.
3. Altrimenti, sceglie di eseguire il task con **priorità massima** tra quelli **già arrivati** (cioè con `Start <= t`) e non ancora eseguiti.
4. Esegue quindi il task fino alla sua conclusione (`t = t + Length`), **senza interruzioni**.
5. Quando un task termina, l'esecutore torna libero e riprende il procedimento appena descritto.
5. La simulazione termina quando tutti i task sono stati eseguiti.

Il risultato della simulazione dovrà essere scritto su un nuovo file CSV, in cui le righe corrispondono ai task eseguiti nell'ordine in cui vengono eseguiti, e deve contenere:

* `ID`: (int) l'identificatore univoco del task;
* `StartTime`: (int) il tempo in cui l’esecuzione del task è effettivamente iniziata (che può essere maggiore del suo tempo di arrivo);
* `EndTime`: (int) il tempo di completamento del task.

Il programma dovrà anche essere usato con i dati contenuti nel file `tasks.csv`, che potete recuperare all'indirizzo:

> [https://datacloud.di.unito.it/index.php/s/xFZcRaxodcakSo4](https://datacloud.di.unito.it/index.php/s/xFZcRaxodcakSo4)

### Condizioni per la consegna:

- Creare una sottocartella chiamata `ex3` all'interno del repository.
- La consegna deve obbligatoriamente contenere un `Makefile`. Questo file con il comando `make all` deve produrre all'interno di `ex3/bin` due file eseguibili chiamati `main_ex3` e `test_ex3`. Se avete usato librerie esterne (come Unity) includete anche queste per consentire la corretta compilazione.
- L'eseguibile `test_ex3` non deve richiedere nessun parametro e deve eseguire tutti gli unit test automatizzati prodotti.
- L'eseguibile `main_ex3` deve ricevere come parametri:

```
$ ./main_ex3 <input_file> <output_file>
```

Si documenti brevemente, in una relazione (README.md su git), le scelte implementative effettuate e i risultati e tempi dell'esecuzione dell'algoritmo rispetto a quanto atteso.