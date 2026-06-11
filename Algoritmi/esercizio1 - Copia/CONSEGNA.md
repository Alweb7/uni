
## Esercizio 1 - Algoritmo di ordinamento ibrido QuickSort-SelectionSort

Implementare un algoritmo di ordinamento **ibrido** che combini QuickSort e SelectionSort in base alla dimensione dell'array da ordinare. L'algoritmo deve funzionare nel seguente modo:

- Per array di dimensione maggiore o uguale a una soglia `k`, utilizzare QuickSort.
- Per array di dimensione minore di `k`, utilizzare SelectionSort.

L'ipotesi da verificare è se SelectionSort, pur avendo complessità O(n²), possa risultare più efficiente di QuickSort su array molto piccoli a causa del minor overhead nelle operazioni elementari. Sarà compito vostro determinare sperimentalmente se questa ipotesi è corretta e identificare eventuali valori ottimali della soglia.

### Specifiche dell'implementazione:

1. **QuickSort**: implementare una versione robusta che sia efficiente su tutti i casi di test presenti nel dataset (descritto sotto). La scelta della strategia di partizionamento e del pivot è lasciata agli studenti, che dovranno valutare le prestazioni sui dati reali.
2. **SelectionSort**: implementare la versione classica O(n²).
3. **Algoritmo ibrido**: deve rispettare la soglia `k` e passare automaticamente da un algoritmo all'altro durante la ricorsione.

La libreria deve supportare l'ordinamento di array di diversi tipi di dato tramite funzioni di comparazione, rispettando la seguente definizione:

```c
void hybridsort(void *base,
                size_t nitems,
                size_t size,
                size_t k,
                int (*compar)(const void *, const void *));
```

Interpretare i parametri della funzione `hybridsort` considerando la richiesta per una implementazione generica.

### Unit Testing

Implementare gli unit test per la libreria secondo le indicazioni suggerite nel documento [Unit Testing](UnitTesting.md).

Gli unit test devono coprire:
- Correttezza di eventuali funzioni ausiliarie sviluppate;
- Correttezza dell'ordinamento per tutti i tipi di dato;
- Comportamento con array vuoti e di dimensione piccola;
- Diversi valori della soglia `k`;
- Casi limite, ad esempio, array già ordinati, ordinati inversamente, con elementi duplicati.

### Uso della libreria di ordinamento implementata

#### Dataset

Il file `records.bin.gz` che potete trovare (compresso) all'indirizzo:

> [https://datacloud.di.unito.it/index.php/s/3c9H8ZAFqQWEBcf](https://datacloud.di.unito.it/index.php/s/3c9H8ZAFqQWEBcf)

contiene 20 milioni di record da ordinare in formato binario compresso.

**Formato del file binario:**
Il file compresso deve essere estratto prima dell'uso. Una volta decompresso, `records.bin` è strutturato come una sequenza di record, ognuno dei quali occupa esattamente 36 bytes:
- **Byte 0-7**: `id` (intero senza segno a 64 bit, little-endian)  
- **Byte 8-11**: `field1` (float a 32 bit, IEEE 754, little-endian)
- **Byte 12-19**: `field2` (intero con segno a 64 bit, little-endian)
- **Byte 20-35**: `field3` (stringa di 16 caratteri, padding con '\0' se necessario)

Il campo `field3` contiene parole estratte dalla Divina Commedia (massimo 15 caratteri utili + terminatore). I campi sono generati per testare diversi scenari di ordinamento, inclusi casi che possono portare alcuni algoritmi di ordinamento al caso peggiore.

**Suggerimento:**  Per facilitare la manipolazione del file binario, si consiglia di implementare una libreria di supporto con funzioni come:

```c
int read_record(FILE *file, Record *record);
int write_record(FILE *file, const Record *record);
int load_records_from_file(FILE *file, Record **records, size_t *count);
int save_records_to_file(FILE *file, const Record *records, size_t count);
void print_record(const Record *record);
```

#### Programma principale

Usando l'algoritmo implementato precedentemente, si realizzi la seguente funzione per ordinare i *record* contenuti nel file `records.bin` in ordine non decrescente secondo i valori contenuti nei campi specificati.

```c
void sort_records(FILE *infile, FILE *outfile, size_t field, size_t k);
```

**Parametri:**
- `infile`: il file binario contenente i record da ordinare.
- `outfile`: il file binario nel quale salvare i record ordinati (deve essere diverso da `infile`).
- `field`: può valere 1, 2, 3 o 4 e indica quale campo deve essere usato per ordinare i record:
  - `1`: ordina per `id` (intero senza segno a 64 bit);
  - `2`: ordina per `field1` (float);
  - `3`: ordina per `field2` (intero con segno a 64 bit);
  - `4`: ordina per `field3` (stringa, ordine lessicografico).
- `k`: soglia per l'algoritmo ibrido:
  - se `k=0`: l'intero array sarà ordinato solo con QuickSort;
  - se `k > numero_totale_record`: l'intero array sarà ordinato solo con SelectionSort;
  - altrimenti: usa QuickSort per sottoinsiemi ≥ k, SelectionSort per sottoinsiemi < k.

### Analisi sperimentale

Si misurino i tempi di risposta dell'algoritmo ibrido per ciascuno dei quattro campi che si possono usare come chiave di ordinamento, al variare di `k`.

**Valori di k da testare**: 0, 10, 50, 100, 500, 1000, 5000, 10000, 1000000

**Gestione della memoria**: L'implementazione deve gestire correttamente l'allocazione e la deallocazione della memoria, evitando memory leak e sprechi di memoria. Si consiglia di testare l'applicazione con strumenti come Valgrind.

Si produca una breve relazione (in formato Markdown nel file `README.md`) in cui si riportano:

1. **Tabelle dei tempi**: tempi di esecuzione per ogni combinazione di campo e valore di k.
2. **Analisi critica**:
   - I risultati corrispondono alle aspettative teoriche?
   - Esiste un valore ottimale di k? Se sì, quale e perché?
   - L'approccio ibrido offre vantaggi rispetto agli algoritmi puri?
   - Ci sono differenze significative tra i diversi tipi di campi?

**Vincoli temporali**: Nel caso l'ordinamento non finisca entro 5 minuti, interrompere l'esecuzione e riportare un fallimento dell'operazione nella relazione. Tempi di esecuzione eccessivi possono indicare che il valore di k è troppo elevato o che ci sono problemi nell'implementazione del QuickSort, ad esempio, degenerazione verso il caso peggiore O(n²). In quest'ultimo caso, riflettere sulla strategia di scelta del pivot per sistemare il problema.

**Si ricorda che i file `records.bin.gz`, `records.bin` (e i file compilati) NON DEVONO ESSERE OGGETTO DI COMMIT SU GIT!**

### Condizioni per la consegna

- Creare una sottocartella chiamata `ex1` all'interno del repository.
- La consegna deve obbligatoriamente contenere un `Makefile` che con il comando `make all` produca all'interno di `ex1/bin` due file eseguibili chiamati `main_ex1` e `test_ex1`.
- Se avete usato librerie esterne (come Unity per i test) includete anche queste per consentire la corretta compilazione.
- L'eseguibile `test_ex1` non deve richiedere parametri e deve eseguire tutti gli unit test automatizzati.
- L'eseguibile `main_ex1` deve ricevere come parametri:

```bash
$ ./bin/main_ex1 <input_file> <output_file> <field> <k>
```

**Esempio di utilizzo:**
```bash
$ ./bin/main_ex1 records.bin sorted_output.bin 1 50
```

Questo comando ordinerà i record del file `records.bin` secondo il campo `id` (field=1) usando l'algoritmo ibrido con soglia k=50, salvando il risultato in `sorted_output.bin`.
