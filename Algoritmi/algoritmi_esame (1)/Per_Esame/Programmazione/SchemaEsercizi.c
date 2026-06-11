/*Esercizi_Liste.c*/

/*1°:
 *Scrivere funzione che ritorni il rango di ogni elemento di una lista, ovvero ogni
 *elemento della lista viene modificato in modo che sia la somma dell'elemento 
 *originario con ogni elemento che segue.
 *Esempio: [1, 2, 3] diventa [6, 5, 3]
*/
    int Rank(list l) {}

/*2°:
 *Scrivere la funzione Reverse che data una lista ne restituisca la lista inversa.
 *Esempio: [1, 2, 3] diventa [3, 2, 1]
*/
    list Reverse(list l) {}

/*3°:
 *Scrivere la funzione Palindrome che data una lista stabilisca se questa e' 
 *palindroma o meno.
 *Suggerimento: usare Reverse dell'esercizio precedente. 
 *Si possono usare funzioni ausiliarie.
 *Esempio: [1, 2, 25, 36, 25, 2, 1] e' palindroma 
 *         [1, 2, 25, 40, 36, 2, 1] non lo e'.
*/
    bool Palindrome(list l) {}
    bool Equal(list l, list r) {}

/*4°:
 *Scrivere una funzione che, data una lista, la riordini in modo da avere tutti
 *i dispari prima di tutti i pari.
 *L'ordine dei dispari e dei pari tra loro deve rimanere invariato.
 *La funzione deve ritornare una nuova lista.
 *preimplementate le funzioni Cons(int x, list l) e Concat(list l, list m).
 *Esempio: [1, 4, 5, 6, 12, 25, 3, 7, 10] diventa [1, 5, 25, 17, 4, 6, 12, 10].
*/
    list OddFirst(list l) {}

/*5°:
 *Supponendo di lavorare con liste di interi ordinate e senza ripetizioni scrivere
 *le seguenti funzioni:
 *Intersection(l, r) che calcola l'intersezione di due liste;
 *Union(l, r) che calcola l'uione di due liste;
 *Difference(l, r) che calcola la differenza (quelli di l ma non di r);
 *SymDifference(l, r) che calcola la differenza simmetrica 
 *                    (unione delle liste tranne loro intersezione).
*/
    list Intersection(list l, list r) {}
    list Union(list l, list r) {}
    list Difference(list l, list r) {}
    list SymDifference(list l, list r) {}

/*6°:
 *Scrivere funzione che dati due parametri, una lista e un intero, cancella tutte
 *le occorrenze di quell'intero e ritorna la lista.
*/
    list DeleteAll(int x, list l) {}

/*7°:
 *Scrivere funzione che inserisce lista l dentro r dopo n elementi di r seguita
 *dai rimanenti elementi di r. 
 *Usare funzione preimplementata Concat.
*/
    list Insert(list l, list r, int n) {}

    /*Esercizi_Alberi_Binari.c*/

/*1°:
 *Scrivere le seguenti funzioni: 
 *      (1)Ritornare puntatore al nodo con chiave minima.
 *      (2)Ritornare l'avo destro piu' prossimo a nodo corrente.
 *      (3)Ritornare puntatore al successore se esiste.
 *(Con 'successore' si intende il minimo dei maggioranti del nodo in questione).
*/
/*(1):*/
    btree MinInBtree(btree bt) {}
/*(2):*/
    btree RightAncestor(btree bt) {}
/*(3):*/
    btree Successor(btree bt) {}

/*2°:
 *Scrivere una funzione che ritorna la lista delle chiavi di bt in ordine 
 *decrescente.
 *Albero binario può essere vuoto.
 *Suggerimento: scrivere funzione ausiliaria che ritorna la lista delle chiavi
 *concatenata con l.
*/
    list DecList(btree bt) {}

/*3°:
 *Scrivere una funzione che ritorna TRUE se bt e' di ricerca.
 *Suggerimento: usare tipo 'Triple' preimplementato e scrivere funzione ausiliaria 
 *che ritorni la tripla (min, isOrdered, max) dove isOrdered e' un boolean, 
 *max e min si capisce.
*/
    bool IsOrdered(btree bt) {ì}
    bool IsOrdered_Aux(btree bt) {}

/*4°:
 *Scrivere una funzione che ritorni l'antenato comune ad 'a' e 'b' più prossimo.
 *'a' e 'b' sono chiavi intere, con a < b.
*/
    btree AntenatoComune(btree bt, int a, int b) {}

/*5°:
*Scrivere funzione Insert(int k, btree bt) che ritorna l'albero ordinato dopo aver 
*inserito chiave k.
*bt puo' essere vuoto.
*/
    btree Insert(int k, btree bt) {}

    /*Esercizi_Alberi_K-ari.c*/

/*1°:
 *Scrivere una funzione che calcoli l'altezza di un albero k-ario, ovvero il 
 *massimo delle lunghezze dei rami (se solo radice non ho rami quindi e' 0).
 *L'albero e' non vuoto.
*/
    int Height(kTree t) {}

/*2°:
 *Scrivere una funzione che calcoli la somma delle etichette sulle foglie.
 *Le etichette sono intere e l'albero puo' essere vuoto.
*/
    int SumLeaf(kTree t) {}

/*3°:
 *Scrivere una funzione che ritorna la lista delle etichette in ordine di visita 
 *BFS dell' albero.
 *Albero puo' essere vuoto.
 *Suggerimento: aggiungere progressivamente etichette con Cons() e poi restituire 
 *inversa della lista con Reverse().
 *(DFS si fa uguale ma con stack al posto della queue, la lista rimane uguale).
*/
    list kTreeBFS(ktree t) {}

/*4°:
 *Scrivere una funzione che ritorni true se l'etichetta di ogni nodo (con almeno 
 *un figlio) e' uguale alla somma delle etichette dei suoi figli, false altrimenti.
 *Albero non vuoto.
*/
    bool Sum(ktree t) {}

/*5°:
 *Scrivere una funzione che aggiunga ad ogni foglia un nuovo ramo contenente la 
 *somma di tutte le etichette presenti lungo il cammino.
 *Albero non vuoto.
 *Suggerimento: Creare funzione ausiliaria che accumuli in una variabile la 
 *              somma fino a quel momento.
*/
    void SommaRamo(ktree t) {}
ì
/*6°:
 *Scrivere una funzione 'NodiProfondi(kTree t, int h)' che restituisca il numero 
 *di nodi dell'albero che si trovano a livello <=h.
 *albero non vuoto.
 *Non si possono usare funzioni ausiliarie. 
*/
    int NodiProfondi(ktree t, int h) {}

/*7°:
 *Scrivere una funzione che ritorni il massimo delle somme delle etichette sullo 
 *stesso ramo.
 *Ovvero prima devo fare la somma di tutte le etichette di ogni ramo e poi devo 
 *ritornare il max di tali somme.
*/
    int MaxSumBranch(kTree t) {}

/*8°:
 *Scrivere una funzione che ritorni il numero di nodi presenti lungo il 
 *ramo più corto.
 *Ramo è cammino da radice a foglia.
 *Albero k-ario non vuoto.
 *(Una DFS).
*/
    int Shortest(kTree t) {}

/*9°:
 *Scrivere una funzione che calcoli il grado di un albero 
 *potenzialmente anche vuoto.
 *Con 'grado' si intende il massimo tra il numero di figli tra tutti i nodi.
 *(BFS modificata per tenere conto del max num di figli).
*/
    int Rank(ktree t) {}

/*10°:
 *Scrivere una funzione che generi e ritorni un albero avente i seguenti dati:
 *altezza il valore 'ht', grado il valore 'rk' e chiave il valore 'key'.
 *Ogni nodo ha chiave identica a ogni altro nodo e vale 'key'.
 *Ogni nodo viene rappresentato con una struct 'node' composta dai campi usuali.
*/
    kTree Generator(int ht, int rk, int key) {}

    /*Esercizi_Vari.c*/

/*1°:
 *Realizzare funzione che implementi algoritmo della bandiera:
 *Prima verdi, poi bianchi infine rossi.
 *Usare il tipo enumerativo 'Color'. 'n' è dimensione array.
 *Funzione swap preimplementata.
*/
    void Bandiera(Color B[], int n) {}

/*2°:
 *Scrivere una funzione che conta il numero di confronti effettuati mentre viene
 *ordinato un array con Bubble Sort.
 *'n' e' dimensione dell'array. 
*/
    int CountBubble(int A[], int n) {}

/*3°:
 *Scrivere la funzione 'Merge' dell'algoritmo Merge Sort.
 *La struttura dati su cui basarsi per scriverlo e' la lista. 
*/
    list Merge(list l, list m) {}

/*4°:
 *Scrivere la funzione 'Partition' dell'algoritmo Quick Sort.
 *La struttura dati su cui basarsi per scriverlo è l'array di interi. 
*/
    int Partition(int A[], int low, int high) {}

/*5°:
 *Scrivere una funzione che implementi la insert in una tabella hash ad 
 *indirizzamento aperto. 'Cons' mette k dopo array[j - 1] e prima di array[j + 1].
 *'HashFun(int k, int i)' è funzione hash preimplementata.
*/
    int Insert(HasTable map, int k) {}

/*6°:
 *Scrivere una funzione che implementi la search in una tabella hash ad 
 *indirizzamento aperto.
 *'HashFun(int k, int i)' è funzione hash preimplementata.
*/
    int Search(HashTable map, int k) {}

/*7°:
 *Scrivere una funzione che implementi l'estrazione del minimo da un MinHeap 'h'
 *avente 'h.size' > 0.
 *(Tolgo elemento iniziale, al suo posto ci metto quello finale e chiamo heapify).
 *(Per semplicita' gli elementi dello heap sono interi).
*/
    int ExctractMin(Heap h) {}

/*8°:
 *Scrivere una funzione per realizzare la struttura dati MaxHeap dato un array
 *non ordinato.
 *(Itera heapify da penultimo livello a radice. Questo perchè il penultimo livello
 * ha ancora figli, l'ultimo no, l'ultimo sono foglie e le foglie sono in 
 * A[length/2..length]).
 *Implementare inoltre l'algoritmo di ordinamento HeapSort.
*/
    void Heapify(int v[], int i) {}
    void BuildHeap(int v[]) {}
    void HeapSort(int v[]) {}

/*9°:
 *Scrivere una funzione che dato un array di interi restituisca la lunghezza
 *maggiore di un sottosegmento dell'array ordinato in senso decrescente.
 *Esempio: 1-5-25-17-6-4-12-10 lunghezza = 4 (sottosegmento = 25-17-6-4).
*/
    int MaxLen(int v[]) {}

/*10°:
 *Scrivere una funzione che esegua l'operazione insert in un MinHeap.
 *(inserimento sempre in ultima posizione e poi risalgo confrontando con parent e 
 * se minore viene scambiato).
 *Heap rappresentato come array.
*/
    void Insert(int h[], int x) {}

/*11°:
 *Scrivere due funzioni (ricorsiva e iterativa) che calcolino l'esponenziale veloce
 *basandosi su:  x^2k = x^k*x^k e x^2k+1 = x^k*x^k*x.
*/
    int ExpRec(int x, int n) {}
    int ExpIter(int x, int n) {}

/*12°:
 *Scrivere due funzioni (ricorsiva e iterativa) che eseguano la ricerca lineare
 *in un array di interi. Parametro 'i' e' indice elemento iniziale.
*/
    int SearchRec(int A[], int i, int a) {}
    int SearchIter(int A[], int i, int a) {}

/*13°:
 *Scrivere due funzioni (ricorsiva e iterativa) che eseguano la ricerca dicotomica
 *in un array ordinato non decrescente di interi.
 *Parametro 'i' e' indice elemento iniziale mentre 'j' è indice elemento finale.
*/
    int SearchRec(int A[], int i, int j, int a) {}
    int SearchRec(int A[], int i, int j, int a) {}

    /*Esercizi_Ultimo_Esame.c*/

/*1°:
 *Data una lista di interi scrivere una funzione che ordini i dispari prima dei pari.
 *L'ordine dei dispari e dei pari tra loro deve rimanere invariato.
 *La funzione deve ritornare una nuova lista.
 *preimplementate le funzioni Cons(int x, list l) e Concat(list l, list m).
 *Esempio: 1-4-5-6-12-25-3-7-10 diventa 1-5-25-17-4-6-12-10.
*/
list OddFirst(list l) {}

/*2°:
 *Realizzare una funzione che conti l'altezza di un albero k-ario.
 *Altezza e' massima distanza tra radice e foglia lungo un ramo. Radice esclusa.
 *Una DFS in sostanza.
*/
int height(kTree t) {}

/*3°:
 *Realizzare una funzione che implementi una visita in ampiezza e che stampi la lista contenente il valore 
 *degli elementi in ordine di lettura.
 *Una BFS.
*/
list BFS(kTree t) {}

/*4°:
 *Realizzare la struttura dati heap massimo dato un array non ordinato.
 *Implementare inoltre l'algoritmo di ordinamento HeapSort. 
*/
void Heapify(int[] v, int i) {}
void BuildHeap(int[] v) {}
void HeapSort(int[] v) {}

/*5°:
 *Scrivere una funzione che dato un array di interi restituisca la lunghezza maggiore di 
 *un sottosegmento dell'array dato ordinato in senso decrescente.
 *Esempio: 1-5-25-17-6-4-12-10 lunghezza = 4 (sottosegmento = 25-17-6-4)
*/
int MaxLen(int[] v) {}

/*6°:
 *Scrivere una funzione che calcoli il grado di un albero k-ario.
 *Con "grado" si intende il massimo tra il numero di figli di ogni nodo.
 *(BFS modificata in modo da tenere conto del max dei figli).
*/
int Rank(kTree t) {}

/*7°:
 *Scrivere una funzione che generi e ritorni un albero k-ario avente i seguenti dati:
 *Come altezza il valore 'ht', come grado il valore 'rk' e come chiave il valore 'key'. 
 *Ogni nodo dell'albero ha chiave identica a ogni altro nodo.
 *Ogni nodo viene rappresentato con una struct 'node' contenente i soliti attributi.
*/
kTree Gen(int ht, int rk, int key) {}

/*8°:
 *Realizzare una funzione che esegua l'operazione di insert in un min heap.
 *Inserimento sempre in ultima posizione e poi risalgo confrontandocon parent e se minore viene scambiato.
 *Heap rappresentato come array.
*/
void Insert(int[] h, int x) {}

/*9°:
 *Realizzare una funzione che controlli se un albero di ricerca e' ordinato o meno.
 *Ritorna true se ordinato, false altrimenti.
 *Suggerimento: Usare il tipo preimplementato 'Triple':
 *struct triple {
 *   bool    isOrdered;
 *   int     min, max;
 *};
 *Scrivere funzione ausiliaria: Triple isOrderedBTreeAux(btree bt).
*/
bool isOrdered(btree bt) {}
Triple isOrderedBTreeAux(btree bt) {}