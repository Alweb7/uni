
## Esercizio 4 - Grafi sparsi e Visita in Ampiezza

Si implementi una libreria che realizza la struttura dati *Grafo* in modo che sia ottimale per dati sparsi
(**attenzione**: le scelte implementative che farete dovranno essere giustificate in relazione alle nozioni presentate
durante le lezioni in aula).

È richiesto che l'implementazione sfrutti la Tavola Hash implementata nell'Esercizio 2.

L'implementazione deve essere generica sia per quanto riguarda il tipo dei nodi, sia per quanto riguarda le etichette
degli archi, implementando le funzioni riportate nel seguente header file (con requisiti minimi di complessità; dove _N_ può indicare il numero di nodi o il numero di archi, a seconda del contesto):

### graph.h

#### Strutture Dati

```c
typedef struct graph *Graph;

typedef struct edge {
    void* source;   // nodo d'origine
    void* dest;     // nodo di destinazione
    void* label;    // etichetta dell'arco
} Edge;
```
---
#### Creazione e Configurazione

```c
Graph graph_create(int labelled,
                  int directed,
                  int (*compare)(const void*, const void*),
                  unsigned long (*hash)(const void*));
```
Crea un grafo vuoto, etichettato se `labelled == 1` e diretto se `directed == 1`. Le funzioni `compare` e `hash` sono necessarie per la costruzione della tavola hash che deve essere usata dalla libreria.  
**Complessità**: O(1)

```c
int graph_is_directed(const Graph gr);
```
Dice se il grafo è diretto o meno.  
**Complessità**: O(1)

```c
int graph_is_labelled(const Graph gr);
```
Dice se il grafo è etichettato o meno.  
**Complessità**: O(1)

---
#### Gestione Nodi

```c
int graph_add_node(Graph gr, const void* node);
```
Aggiunge un nodo al grafo.  
**Complessità**: O(1)

```c
int graph_contains_node(const Graph gr, const void* node);
```
Controlla se un nodo è presente nel grafo.  
**Complessità**: O(1)

```c
int graph_remove_node(Graph gr, const void* node);
```
Rimuove un nodo dal grafo.  
**Complessità**: O(N)

```c
int graph_num_nodes(const Graph gr);
```
Restituisce il numero di nodi nel grafo.  
**Complessità**: O(1)

```c
void** graph_get_nodes(const Graph gr);
```
Recupera tutti i nodi del grafo.  
**Complessità**: O(N)

---
#### Gestione Archi

```c
int graph_add_edge(Graph gr, const void* node1, const void* node2, const void* label);
```
Aggiunge un arco dati gli estremi e l'etichetta.  
**Complessità**: O(1) (*)

```c
int graph_contains_edge(const Graph gr, const void* node1, const void* node2);
```
Controlla se un arco è presente nel grafo.  
**Complessità**: O(1) (*)

```c
int graph_remove_edge(Graph gr, const void* node1, const void* node2);
```
Rimuove un arco dal grafo.  
**Complessità**: O(1) (*)

```c
int graph_num_edges(const Graph gr);
```
Restituisce il numero di archi nel grafo.  
**Complessità**: O(N)

```c
Edge** graph_get_edges(const Graph gr);
```
Recupera tutti gli archi del grafo.  
**Complessità**: O(N)

```c
void* graph_get_label(const Graph gr, const void* node1, const void* node2);
```
Recupera l'etichetta di un arco.  
**Complessità**: O(1) (*)

---
#### Adiacenza

```c
void** graph_get_neighbours(const Graph gr, const void* node);
```
Recupera i nodi adiacenti a un dato nodo.  
**Complessità**: O(1) (*)

```c
int graph_num_neighbours(const Graph gr, const void* node);
```
Recupera il numero di nodi adiacenti a un dato nodo.  
**Complessità**: O(1)

---
#### Distruzione

```c
void graph_free(Graph gr);
```
Libera la memoria allocata per il grafo.

---

**Nota**: Le complessità contrassegnate con (*) assumono un'implementazione efficiente della tavola hash sottostante, e quando il grafo è veramente sparso, assumendo che l'operazione venga effettuata su un nodo la cui lista di adiacenza ha una lunghezza in O(1).

La struttura ```struct graph``` deve essere decisa prendendo in considerazione la richiesta di usare la Tabella Hash dell'esercizio precedente.

*Suggerimento*:  un grafo non diretto può essere rappresentato usando un'implementazione per grafi diretti modificata
per garantire che, per ogni arco *(a,b)* etichettato *w*, presente nel grafo, sia presente nel grafo anche l'arco *(b,a)*
etichettato *w*. Ovviamente, il grafo dovrà mantenere l'informazione che specifica se esso è un grafo diretto o non diretto.
Similmente, un grafo non etichettato può essere rappresentato usando l'implementazione per grafi etichettati modificata per garantire
che le etichette siano sempre `null` (che invece non devono mai essere `null` per i grafi etichettati).

### Unit Testing

Implementare gli unit-test degli algoritmi secondo le indicazioni suggerite nel documento [Unit Testing](UnitTesting.md).

### Uso della libreria che implementa la struttura dati Grafo

Si implementi l'algoritmo di visita in ampiezza secondo il seguente prototipo di funzione

```c
void** breadth_first_visit(Graph gr,
                          void* start,
                          int (*compare)(const void*, const void*),
                          unsigned long (*hash)(const void*));
```
Start è il nodo di partenza da cui cominciare la visita, la funzione restituisce l'array dei nodi nell'ordine di visita. Eventualmente, la funzione restituisce null se il nodo start non è presente nel grafo. L'implementazione dell'algoritmo di visita in ampiezza dovrà utilizzare la libreria sui grafi appena implementata.

L'algoritmo dovrà poi essere usato con i dati contenuti nel file `italian_dist_graph.csv`, che potete recuperare all'indirizzo:

> [https://datacloud.di.unito.it/index.php/s/FqneW99EGWLSRpY](https://datacloud.di.unito.it/index.php/s/FqneW99EGWLSRpY)

Tale file contiene le distanze in metri tra varie località italiane e una frazione delle località a loro più vicine. Il formato è un CSV standard: i campi sono separati da virgole; i record sono separati dal carattere di fine riga (`\n`).

Ogni record contiene i seguenti dati:

- `place1`: (tipo stringa) nome della località "sorgente" (la stringa può contenere spazi ma non può contenere virgole);
- `place2`: (tipo stringa) nome della località "destinazione" (la stringa può contenere spazi ma non può contenere virgole);
- `distance`: (tipo float) distanza in metri tra le due località.

**Note:**

- Potete interpretare le informazioni presenti nelle righe del file come archi **non diretti**.
- Il file è stato creato a partire da un dataset poco accurato. I dati riportati contengono inesattezze e imprecisioni.

**Si ricorda che il file `italian_dist_graph.csv` (e i file compilati) NON DEVONO ESSERE OGGETTO DI COMMIT SU GIT!**

### Condizioni per la consegna:

- Creare una sottocartella chiamata `ex4` all'interno del repository, che conterrà i file relativi a questo esercizio.
- Includete nella consegna anche un `Makefile` che con il comando `make all` deve produrre all'interno di `ex4/bin` due file eseguibili chiamati `main_ex4` e `test_ex4`. Se avete usato librerie esterne (come Unity) includete anche queste per consentire la corretta compilazione.
- L'eseguibile `test_ex4` non deve richiedere nessun parametro e deve eseguire tutti gli unit test automatizzati prodotti.
- L'eseguibile `main_ex4` deve ricevere come parametri il percorso del file `italian_dist_graph.csv`, il nome della città di partenza e il nome di un file di output, e salvare in quest'ultimo i nomi delle località visitate durante una visita in ampiezza del grafo, un nome per riga, partendo da un nodo di partenza specificato. I nomi dei file non devono essere hardcoded, ma devono essere passati come argomenti da linea di comando.

```
$ ./main_ex4 italian_dist_graph.csv torino output.txt
```

Si documenti brevemente, in una relazione (README.md su git), le scelte implementative effettuate e i risultati e tempi dell'esecuzione dell'algoritmo rispetto a quanto atteso.
