Niente panico, è un bel salto di livello ma è tutto molto logico. Andiamo a smontare questi concetti uno per uno, partendo da zero.

Queste strutture dati servono a **organizzare le informazioni in memoria in modi specifici**, a seconda di come avremo bisogno di tirarle fuori in seguito.

---

### 1. Le Strutture basate su Array (I "Contenitori rigidi")

Queste tre strutture (`Stack`, `Queue`, `HashTable`) usano come base un normale array in C (un `int* data` o `int* array`), ma impongono delle **regole ferree** su come puoi inserire e togliere i dati.

#### A. Stack (La Pila)

Pensa allo Stack esattamente come a una **pila di piatti da lavare**.

* **Regola d'oro:** LIFO (*Last In, First Out* - L'ultimo ad entrare è il primo ad uscire). Non puoi sfilare un piatto dal fondo, devi per forza prendere quello in cima.
* **Come funziona nel codice:** La struct ha l'array (`data`), la grandezza massima (`size`), e un indice chiamato **`top`**. All'inizio `top` vale -1 (pila vuota).
* Quando fai *Push* (inserisci), aggiungi un elemento nell'array e fai salire `top` di 1.
* Quando fai *Pop* (estrai), prendi l'elemento dove punta `top` e fai scendere `top` di 1.



#### B. Queue (La Coda)

La Queue è esattamente come la **coda alla cassa del supermercato**.

* **Regola d'oro:** FIFO (*First In, First Out* - Il primo ad entrare è il primo ad uscire). Chi arriva primo viene servito primo.
* **Come funziona nel codice:** Poiché si entra da una parte e si esce dall'altra, ci servono due indici: **`front`** (la testa della coda, chi sta pagando alla cassa) e **`rear`** (il fondo della coda, dove si mettono i nuovi arrivati).
* Quando fai *EnQueue* (accodi), metti l'elemento all'indice `rear` e sposti `rear` in avanti.
* Quando fai *DeQueue* (estrai), prendi l'elemento all'indice `front` e sposti `front` in avanti.



#### C. HashTable (La Tabella Hash)

Questa è una struttura magica usata per **trovare dati all'istante**, senza dover scorrere tutto l'array. Pensa a un dizionario.

* **Come funziona nel codice:** Hai un array molto grande. Quando vuoi salvare un numero (es. il numero di matricola `12345`), non lo metti al primo posto libero. Lo passi a una funzione matematica (la *Hash Function*) che fa un calcolo e sputa fuori un indice, ad esempio `7`. Tu andrai a salvare i dati del ragazzo esattamente nella cella `array[7]`.
* **Il vantaggio:** Quando in futuro dovrai cercare la matricola `12345`, ripassi il numero alla Hash Function, lei ti ridice `7`, e tu vai a colpo sicuro in `array[7]`. Tempo di ricerca: quasi zero!

---

### 2. Tipi Opachi e il mistero del `void*`

Nel codice hai visto: `typedef void* queue;` e `typedef void* stack;`.

Cos'è un **`void*`**? In C, un normale puntatore sa esattamente a cosa punta (es. `int*` punta a un intero). Un puntatore `void*` è un puntatore "jolly" o "cieco". **Punta a un pezzo di memoria, ma non gli importa cosa ci sia dentro.** Può essere un intero, una struct, un albero, qualsiasi cosa.

**Perché si usano negli algoritmi (Tipi Opachi):**
Immagina di aver scritto un bellissimo codice per gestire una Coda (Queue) che funziona con i numeri interi.
A un certo punto dell'esame, ti viene chiesto di navigare un **Albero Binario** usando un algoritmo chiamato BFS (Ricerca in ampiezza). Questo algoritmo, per funzionare, ha bisogno di mettere *i Nodi dell'albero* dentro una coda, non dei semplici numeri!

Se la tua coda fosse `int* data`, dovresti riscrivere tutto il codice della coda da capo creando un `btreeNode* data`.
Definendo la coda come `void*`, la rendi **universale**. Puoi "infilarci" dentro puntatori ai nodi dell'albero. Il sistema li accetta perché sono solo indirizzi di memoria, e il tuo codice è salvo.

---

### 3. QueueList (Il meglio dei due mondi)

Ricordi la Queue (coda) basata sugli array vista prima? Ha un difetto enorme: gli array hanno una dimensione fissa (`size`). Se il supermercato si riempie troppo, la coda "esce" dall'array e il programma crasha.

La **`QueueList`** risolve questo problema fondendo la logica della Coda con la struttura della Lista Concatenata:

```c
typedef struct { 
    list front; // Punta al primo nodo della lista
    list rear;  // Punta all'ultimo nodo della lista
} QueueList;

```

* È formata da *Nodi* sparsi nella memoria (quindi **può crescere all'infinito**).
* Mantiene però un puntatore `front` (per sapere chi è il primo e fare *DeQueue* velocemente) e un puntatore `rear`.
* **Il trucco di O(1):** Nelle liste concatenate normali, se vuoi aggiungere un elemento in fondo, devi partire dall'inizio e scorrere tutta la lista. Qui no! Avendo il puntatore `rear` che punta già in automatico all'ultimo vagone, puoi "agganciare" un nuovo nodo in fondo in un istante (che in informatica si dice "tempo costante", ovvero $O(1)$).