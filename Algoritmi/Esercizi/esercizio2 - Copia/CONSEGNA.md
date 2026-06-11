## Esercizio 2 - Tavole hash (con concatenamento)

Si implementi, avvalendosi del supporto di un sistema basato su un Large Language Model, quale, ad esempio, ChatGPT (si veda sotto), una libreria generica che realizza la struttura dati *tavola hash (con concatenamento)* in grado di ospitare un insieme di coppie {<chiave_1,valore_1>,...,<chiave_n,valore_n>}.

La tavola hash deve accettare chiavi e valori di tipi generici (tutte le chiavi hanno uno stesso tipo, tutti i valori hanno uno stesso tipo, ma chiavi e valori possono avere tipi fra loro differenti). Per confrontare le chiavi, la tavola deve accettare una funzione di tipo _compare_, come quelle usate nel precedente esercizio.

La struttura dati deve offrire almeno le seguenti funzionalità (ricavare il significato delle varie funzioni e procedure e dei loro parametri a partire dai loro prototipi e da quanto studiato nella parte di teoria del corso):

```
HashTable* hash_table_create(int (*f1)(const void*,const void*), unsigned long (*f2)(const void*));
void hash_table_put(HashTable*, const void*, const void*);
void* hash_table_get(const HashTable*, const void*);
int hash_table_contains_key(const HashTable*, const void*);
void hash_table_remove(HashTable*, const void*);
int hash_table_size(const HashTable*);
void** hash_table_keyset(const HashTable*);
void hash_table_free(HashTable*);
```

### Unit Testing

Implementare avvalendosi del supporto di un sistema basato su un Large Language Model (LLM), quale, ad esempio, ChatGPT (si veda sotto), gli unit-test degli algoritmi secondo le indicazioni suggerite nel documento [Unit Testing](UnitTesting.md).

### Uso delle funzioni implementate

All'indirizzo:

> [https://datacloud.di.unito.it/index.php/s/Ti4Mz7j4Xtjn3Db](https://datacloud.di.unito.it/index.php/s/Ti4Mz7j4Xtjn3Db)

potete trovare un file (`iliade.txt`) contenente l'Iliade di Omero in inglese.

Avvalendosi di un sistema basato su LLM, scrivere un programma che utilizza l'hash table implementata per calcolare la parola di lunghezza almeno pari ad un valore minimo dato che sia più frequente nel file di testo dato.

### Uso di sistema basato su un Large Language Model

Si richiede che per implementare quanto richiesto dal presente esercizio ci si avvalga del supporto di un sistema basato su un Large Language Model, quale, ad esempio, ChatGPT.

È possibile che il processo di sviluppo risulti iterativo, comportando varie interazioni con il sistema LLM.

**ATTENZIONE**: I Large Language Models spesso forniscono implementazioni che non rispettano le specifiche dell'esercizio, in particolare ignorando la complessità computazionale richiesta per le strutture dati. **Le consegne che non rispettano la complessità computazionale specificata saranno considerate completamente invalide**. È responsabilità del gruppo studiare attentamente il codice generato, verificarne la correttezza algoritmica e assicurarsi che rispetti i vincoli richiesti.

Serve quindi assumere un ruolo critico e di supervisione nel processo, non limitandosi ad accettare passivamente l'output del sistema LLM, ma analizzandolo, validandolo e, se necessario, richiedendo modifiche specifiche per garantire la conformità ai requisiti.

Si documenti, in una relazione (README.md su git), il suddetto processo di sviluppo nei suoi aspetti principali (prompt iniziale, output prodotto dal sistema, analisi critica dell'output, identificazione di eventuali problemi di complessità, raffinamento del prompt, iterazioni successive, ecc.) e si riportino alcune considerazioni generali sull'intero processo, con particolare attenzione alle sfide incontrate nel garantire la correttezza algoritmica.

### Condizioni per la consegna:

- Creare una sottocartella chiamata `ex2` all'interno del repository.
- La consegna deve obbligatoriamente contenere un `Makefile`. Questo file con il comando `make all` deve produrre all'interno di `ex2/bin` due file eseguibili chiamati `main_ex2` e `test_ex2`. Se avete usato librerie esterne (come Unity) includete anche queste per consentire la corretta compilazione.
- L'eseguibile `test_ex2` non deve richiedere nessun parametro e deve eseguire tutti gli unit test automatizzati prodotti.
- L'eseguibile `main_ex2` deve ricevere come parametri il percorso del file di testo da usare e la lunghezza minima delle parole da considerare. Per esempio:

```
$ ./main_ex2 /tmp/data/iliade.txt 6
```