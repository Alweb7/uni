/*=============================================================================
 * TUTTO_ESAME.c — Tutti i codici da sapere per il CodeRunner
 * Organizzato per argomento. Studia in ordine.
 *=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/*=============================================================================
 * TIPI USATI IN TUTTO IL FILE
 *=============================================================================
 *
 * Color: enumerazione per la Bandiera (verde < bianco < rosso in memoria)
 *
 * list: ATTENZIONE — list è GIÀ un puntatore!
 *   typedef struct listNode { int info; struct listNode* next; } *list;
 *   Quindi: list l;  equivale a  struct listNode* l;
 *   Quando fai l->info non serve il & prima di l.
 *
 * btree: albero binario con puntatore al padre (parent).
 *   Utile per Successor e RightAncestor che risalgono verso la radice.
 *
 * kTree: albero k-ario con rappresentazione figlio-fratello.
 *   child   = primo figlio
 *   sibling = fratello successivo (stesso livello, stesso padre)
 *   Per scorrere tutti i figli di t:
 *       kTree c = t->child;
 *       while (c != NULL) { ... c = c->sibling; }
 *
 * Triple: struct ausiliaria per IsOrdered_aux.
 *   isOrdered = l'albero rispetta la proprietà BST?
 *   min, max  = chiave minima e massima nel sottoalbero
 *
 * Stack (array): top = indice dell'elemento in cima (-1 se vuoto)
 * Queue (array circolare): front = primo elemento, rear = prossima posizione libera
 * HashTable: puntatore a { int* array; int dim; }
 *
 * queue/stack opachi: usati nelle funzioni su alberi k-ari,
 *   gestiti con NewQueue/EnQueue/DeQueue/isEmpty e NewStack/Push/Pop.
 *=============================================================================*/

typedef enum { verde, bianco, rosso } Color;

typedef struct listNode {
    int info;
    struct listNode* next;
} *list;

typedef struct btreeNode {
    int info;
    struct btreeNode* left;
    struct btreeNode* right;
    struct btreeNode* parent;
} *btree;

typedef struct kTreeNode {
    int info;
    struct kTreeNode* child;
    struct kTreeNode* sibling;
} *kTree;

typedef struct {
    bool isOrdered;
    int  min;
    int  max;
} Triple;

typedef struct { int* data; int top;   int size;          } Stack;
typedef struct { int* data; int front; int rear; int size; } Queue;
typedef struct { int* array; int dim; }* HashTable;

typedef void* queue;   /* coda opaca usata negli alberi */
typedef void* stack;   /* pila opaca usata negli alberi */
typedef struct kTreeNode node;  /* alias usato in malloc(sizeof(node)) */
typedef struct { list front; list rear; } QueueList;  /* coda lista-based */

/*=============================================================================
 * FUNZIONI PREIMPLEMENTATE DAL CODERUNNER
 *=============================================================================
 * Non devi scriverle tu. Il CodeRunner le fornisce già compilate.
 *
 * swap(A, i, j)      — scambia A[i] e A[j] in un array di int
 * Cons(x, l)         — alloca un nuovo nodo con info=x e next=l, restituisce il puntatore
 * Concat(a, b)       — concatena due liste (modifica il next dell'ultimo nodo di a)
 * Reverse(l)         — inverte una lista, restituisce la nuova testa
 * copyList(l)        — copia profonda di una lista
 * length(s)          — lunghezza: per liste conta i nodi, per heap/stack la dimensione
 * max(a, b)          — ATTENZIONE: negli heap restituisce l'INDICE del massimo, non il valore!
 * min(a, b)          — idem per il minimo
 * NewQueue/EnQueue/DeQueue/isEmpty — coda generica per puntatori (usata in alberi)
 * NewStack/Push/Pop  — pila generica per puntatori (usata in alberi)
 * hashFun(k, i)      — funzione hash con sondaggio i per indirizzamento aperto
 * h(k)               — funzione hash semplice per concatenamento
 * NewNode()          — alloca un nuovo nodo btree con campi azzerati
 *=============================================================================*/
void   swap(int A[], int i, int j);
list   Cons(int x, list l);
list   Concat(list a, list b);
list   Reverse(list l);
list   copyList(list l);
int    length(void* s);
int    max(int a, int b);
int    min(int a, int b);
queue  NewQueue(void);
void   EnQueue(void* elem, queue q);
void*  DeQueue(queue q);
bool   isEmpty(queue q);
stack  NewStack(void);
void   Push(void* elem, stack s);
void*  Pop(stack s);
int    hashFun(int k, int i);
int    h(int k);
btree  NewNode(void);


/*=============================================================================
 * 1. ARRAY
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * BANDIERA TRICOLORE (Dutch National Flag problem)
 *
 * SCOPO: riordinare un array di Color in tre zone: [ verde | bianco | rosso ]
 *        in un singolo passaggio O(n), senza array ausiliario.
 *
 * COME FUNZIONA:
 *   Si mantengono tre zone delimitate da tre indici:
 *     i = confine tra zona verde e zona bianca (primo bianco non ancora sistemato)
 *     j = cursore che avanza (elemento corrente da classificare)
 *     k = confine tra zona bianca e zona rossa (primo rosso non ancora sistemato)
 *   Invariante:
 *     B[0..i-1]  = verdi  (definitivi)
 *     B[i..j-1]  = bianchi (definitivi)
 *     B[j..k]    = sconosciuti (da classificare)
 *     B[k+1..n-1]= rossi  (definitivi)
 *
 * NOTA: in questo codice k non viene usato come indice separato ma j scorre
 *       fino a n-1 e i,j vengono aggiornati in base al colore trovato.
 *
 * ATTENZIONE: in questa versione:
 *   - verde:  due swap (j<->k poi i<->j) perché deve andare prima di tutti
 *   - bianco: un solo swap (j<->k) perché va dopo i verdi
 *   - rosso:  nessun swap, rimane dove sta
 *-----------------------------------------------------------------------------*/
void Bandiera(Color B[], int n) {
    int i = 0, j = 0;              /* i = confine verde/bianco, j = cursore */
    for (int k = 0; k < n; k++) {  /* k scorre tutto l'array */
        if (B[k] == bianco) {
            swap(B, j, k);         /* porta il bianco nella zona bianca */
            j++;                   /* espandi la zona bianca */
        }
        else if (B[k] == verde) {
            swap(B, j, k);         /* prima porta in posizione j (inizio zona bianca) */
            swap(B, i, j);         /* poi porta in posizione i (inizio zona verde) */
            i++;                   /* espandi la zona verde */
            j++;                   /* la zona bianca si sposta di conseguenza */
        }
        /* rosso: non fare nulla, rimane nella zona destra */
    }
}

/*-----------------------------------------------------------------------------
 * COUNTBUBBLE (BubbleSort con contatore di confronti)
 *
 * SCOPO: ordina l'array A in ordine crescente con BubbleSort e restituisce
 *        il numero totale di confronti effettuati.
 *
 * COME FUNZIONA:
 *   BubbleSort fa "bollire" i grandi elementi verso destra.
 *   Ogni passata i porta l'i-esimo massimo in posizione i.
 *   Ottimizzazione: se in un passata non c'è stato nessuno swap (flag=false),
 *   l'array è già ordinato → fermarsi subito.
 *
 * RIGA PER RIGA:
 *   flag = true     → usato per entrare nel primo ciclo
 *   i da n-1 a 1    → ogni passata riduce la zona da ordinare di uno
 *   flag = false    → assume che non serva altro swap
 *   j da 0 a i-1    → confronta coppie adiacenti nella zona non ordinata
 *   count++         → conta OGNI confronto, anche quelli senza swap
 *   if A[j]>A[j+1]  → se fuori ordine: swap e flag=true (passata non inutile)
 *-----------------------------------------------------------------------------*/
int CountBubble(int A[], int n) {
    int count = 0;       /* contatore confronti */
    bool flag = true;    /* true = continua, false = già ordinato */
    for (int i = n - 1; i > 0 && flag; i--) {  /* i = dimensione zona da ordinare */
        flag = false;                            /* ottimismo: forse è già ordinato */
        for (int j = 0; j < i; j++) {           /* confronta coppie adiacenti */
            count++;                             /* conta il confronto */
            if (A[j] > A[j + 1]) {
                swap(A, j, j + 1);              /* fuori ordine: scambia */
                flag = true;                     /* c'è stato almeno uno swap */
            }
        }
    }
    return count;
}

/*-----------------------------------------------------------------------------
 * PARTITION (Lomuto, pivot = ultimo elemento)
 *
 * SCOPO: riorganizza A[low..high] in modo che:
 *   - tutti gli elementi < pivot siano a sinistra del pivot
 *   - tutti gli elementi > pivot siano a destra del pivot
 *   - il pivot sia nella sua posizione definitiva
 *   Restituisce l'indice finale del pivot.
 *   È il cuore di QuickSort.
 *
 * COME FUNZIONA:
 *   Si mantengono due zone in A[low..high-1]:
 *     A[low..i]   = elementi ≤ pivot (zona "piccoli")
 *     A[i+1..j-1] = elementi > pivot (zona "grandi")
 *   j scorre da low a high-1. Quando trova un elemento < pivot,
 *   allarga la zona piccoli (i++) e ci porta l'elemento (swap i,j).
 *   Alla fine, il pivot va in posizione i+1 (tra piccoli e grandi).
 *
 * ATTENZIONE BUG CLASSICO:
 *   i++ va fatto PRIMA di swap(A,i,j), non dopo!
 *   Sbagliato:  swap(A, i, j); i++;
 *   Corretto:   i++;  swap(A, i, j);
 *
 * RIGA PER RIGA:
 *   pivot = A[high]  → scegli ultimo come pivot
 *   i = low - 1      → zona piccoli inizialmente vuota (i "prima" di low)
 *   j da low a high-1→ esamina ogni elemento tranne il pivot
 *   if A[j] < pivot  → questo elemento appartiene alla zona piccoli
 *     i++            → PRIMA sposta il confine della zona piccoli
 *     swap(A,i,j)    → POI porta A[j] nella zona piccoli
 *   swap(A,i+1,high) → metti il pivot tra piccoli e grandi
 *   return i+1       → indice finale del pivot
 *-----------------------------------------------------------------------------*/
int Partition(int A[], int low, int high) {
    int pivot = A[high];  /* scegli il pivot come ultimo elemento */
    int i = low - 1;      /* i = indice dell'ultimo elemento sicuramente ≤ pivot (inizia "prima" di low) */

    for (int j = low; j < high; j++) {   /* j esamina tutti gli elementi tranne il pivot */
        if (A[j] < pivot) {
            i++;                          /* PRIMA incrementa i (sposta il confine a destra) */
            swap(A, i, j);               /* POI porta A[j] nella zona "piccoli" */
        }
        /* se A[j] >= pivot: non fare nulla, rimane nella zona "grandi" */
    }

    swap(A, i + 1, high);   /* metti il pivot tra i "piccoli" e i "grandi" */
    return i + 1;            /* restituisce la posizione finale del pivot */
}

/*-----------------------------------------------------------------------------
 * MERGE (fusione di due liste ordinate — usato in MergeSort su liste)
 *
 * SCOPO: date due liste già ordinate l e m, restituisce una lista ordinata
 *        contenente tutti gli elementi di entrambe.
 *        Non crea nodi nuovi: riusa i nodi esistenti modificando i next.
 *
 * COME FUNZIONA (ricorsione):
 *   Confronta le teste delle due liste.
 *   La testa minore diventa la testa del risultato.
 *   Il suo next viene calcolato ricorsivamente sul resto.
 *
 * RIGA PER RIGA:
 *   if l==NULL → l è esaurita: il risultato è tutto m (già ordinato)
 *   if m==NULL → m è esaurita: il risultato è tutto l
 *   if l->info <= m->info:
 *     la testa di l è il minimo → l rimane in testa
 *     l->next = Merge(l->next, m)  → il prossimo è il merge del resto di l con m
 *     return l                     → restituisce l come nuova testa
 *   else:
 *     la testa di m è il minimo → m viene in testa
 *     m->next = Merge(l, m->next)  → il prossimo è il merge di l con il resto di m
 *     return m
 *
 * ESEMPIO:
 *   Merge([1,3,5], [2,4]) → [1,2,3,4,5]
 *   Merge([1,3,5], [2,4]):
 *     1 < 2 → 1->next = Merge([3,5],[2,4])
 *       3 > 2 → 2->next = Merge([3,5],[4])
 *         3 < 4 → 3->next = Merge([5],[4])
 *           5 > 4 → 4->next = Merge([5],[]) → [5]
 *           → 4->[5]
 *         → 3->[4,5]
 *       → 2->[3,4,5]
 *     → 1->[2,3,4,5]
 *-----------------------------------------------------------------------------*/
list Merge(list l, list m) {
    if (l == NULL) return m;   /* lista l esaurita: il risultato è tutto m */
    if (m == NULL) return l;   /* lista m esaurita: il risultato è tutto l */

    if (l->info <= m->info) {
        /* la testa di l è il minimo: tienila, e il suo "next" è il merge del resto */
        l->next = Merge(l->next, m);
        return l;
    }
    else {
        /* la testa di m è il minimo: tienila, e il suo "next" è il merge del resto */
        m->next = Merge(l, m->next);
        return m;
    }
}

/*-----------------------------------------------------------------------------
 * MAXLEN (lunghezza massima di un sottosegmento strettamente decrescente)
 *
 * SCOPO: trovare la lunghezza del più lungo sottosegmento contiguo
 *        strettamente decrescente nell'array v[0..n-1].
 *        Esempio: [1,5,25,17,6,4,12,10] → 4 (segmento 25,17,6,4)
 *
 * COME FUNZIONA:
 *   Scorre l'array confrontando ogni coppia adiacente v[i-1] e v[i].
 *   Se v[i-1] > v[i]: il segmento decrescente continua → count++
 *   Altrimenti: il segmento si è interrotto → aggiorna maxLen, reset count=1
 *   FONDAMENTALE: dopo il ciclo, controlla ancora count > maxLen
 *   perché l'ultimo segmento potrebbe non essere mai stato confrontato
 *   con maxLen (se il ciclo finisce mentre count > maxLen).
 *
 * RIGA PER RIGA:
 *   count = 1     → il primo elemento da solo è già un segmento di lunghezza 1
 *   maxLen = 1    → massimo iniziale = 1 (anche un solo elemento conta)
 *   i da 1 a n-1  → confronta v[i-1] con v[i] (coppie adiacenti)
 *   v[i-1]>v[i]   → segmento decrescente continua: count++
 *   else          → segmento finito: aggiorna max, reset count=1
 *   dopo il ciclo → OBBLIGATORIO: l'ultimo segmento potrebbe essere il più lungo!
 *
 * BUG CLASSICO: dimenticare il controllo count > maxLen DOPO il ciclo.
 *   Esempio: [5,4,3,2,1] → tutto l'array è decrescente, ma il ciclo finisce
 *   senza mai entrare nell'else, quindi maxLen rimarrebbe 1 senza il check finale.
 *-----------------------------------------------------------------------------*/
int MaxLen(int v[], int n) {
    int count = 1;    /* lunghezza del segmento decrescente corrente (parte da 1: il primo elemento da solo) */
    int maxLen = 1;   /* massima lunghezza trovata finora */

    for (int i = 1; i < n; i++) {
        if (v[i - 1] > v[i]) {
            count++;   /* il segmento decrescente continua: incrementa il contatore */
        }
        else {
            /* il segmento si è interrotto: aggiorna il massimo e riparte */
            if (count > maxLen) maxLen = count;
            count = 1;   /* il nuovo segmento inizia da v[i] (lunghezza 1) */
        }
    }

    /* FONDAMENTALE: controlla l'ultimo segmento che potrebbe non essere stato confrontato */
    if (count > maxLen) maxLen = count;

    return maxLen;
}

/*-----------------------------------------------------------------------------
 * ESPONENZIALE VELOCE (Fast Exponentiation / Repeated Squaring)
 *
 * SCOPO: calcolare x^n in O(log n) moltiplicazioni invece di O(n).
 *        Algoritmo fondamentale in crittografia e algoritmi numerici.
 *
 * IDEA:
 *   x^n = (x^(n/2))^2            se n è pari
 *   x^n = (x^(n/2))^2 * x        se n è dispari
 *   x^0 = 1                       caso base
 *
 * VERSIONE RICORSIVA (ExpRec):
 *   Calcola y = x^(n/2) UNA SOLA VOLTA (non due volte!),
 *   poi restituisce y*y o y*y*x.
 *   ATTENZIONE: NON scrivere return ExpRec(x,n/2)*ExpRec(x,n/2)
 *   perché chiamerebbe la funzione due volte → O(n) invece di O(log n).
 *
 * VERSIONE ITERATIVA (ExpIter):
 *   y = x (base), k = n (esponente residuo), z = 1 (accumulatore risultato)
 *   Ad ogni iterazione:
 *     se k è dispari: moltiplica z per y (consuma il bit meno significativo)
 *     quadra y (raddoppia l'esponente)
 *     dimezza k
 *   Alla fine z = x^n.
 *-----------------------------------------------------------------------------*/
int ExpRec(int x, int n) {
    if (n == 0) return 1;             /* caso base: x^0 = 1 */
    int y = ExpRec(x, n / 2);         /* calcola x^(n/2) UNA sola volta */
    if (n % 2 == 0) return y * y;     /* n pari:   x^n = (x^(n/2))^2 */
    else             return y * y * x; /* n dispari: x^n = (x^(n/2))^2 * x */
}
int ExpIter(int x, int n) {
    int y = x;    /* base corrente (verrà quadrata ad ogni passo) */
    int k = n;    /* esponente residuo da consumare */
    int z = 1;    /* accumulatore del risultato */
    while (k > 0) {
        if (k % 2 == 1) z = z * y;   /* se k è dispari: consuma questo bit */
        y = y * y;                    /* quadra la base per il prossimo bit */
        k = k / 2;                    /* dimezza l'esponente */
    }
    return z;
}

/*-----------------------------------------------------------------------------
 * RICERCA LINEARE (ricorsiva e iterativa)
 *
 * SCOPO: trovare la prima occorrenza di a in A[i..n], restituisce l'indice
 *        oppure -1 se non trovato.
 *
 * RICORSIVA: se fuori range → -1; se trovato → i; altrimenti ricorri su i+1
 * ITERATIVA: avanza k finché non trova a o esce dal range
 *
 * NOTA: il parametro i è l'indice di partenza (di solito 0),
 *       n è l'indice finale incluso (di solito lunghezza-1).
 *-----------------------------------------------------------------------------*/
int SearchLinRec(int A[], int i, int n, int a) {
    if (i > n)       return -1;              /* fuori range: non trovato */
    if (A[i] == a)   return i;              /* trovato in posizione i */
    return SearchLinRec(A, i + 1, n, a);    /* cerca nel resto */
}
int SearchLinIter(int A[], int i, int n, int a) {
    int k = i;                               /* k = cursore corrente */
    while (k <= n && A[k] != a) k++;         /* avanza finché non trovi a o esci */
    if (k > n) return -1;                    /* uscito senza trovare: non c'è */
    return k;                                /* trovato in posizione k */
}

/*-----------------------------------------------------------------------------
 * RICERCA BINARIA su array ORDINATO (ricorsiva e iterativa)
 *
 * SCOPO: trovare a in A[i..j] (che deve essere ordinato) in O(log n).
 *        Restituisce l'indice di a, oppure -1 se non trovato.
 *
 * COME FUNZIONA:
 *   Calcola il punto medio m = (i+j)/2.
 *   Se A[m] == a → trovato.
 *   Se A[m] < a  → a è nella metà destra: cerca in [m+1..j]
 *   Se A[m] > a  → a è nella metà sinistra: cerca in [i..m-1]
 *   Se i > j     → range vuoto, non trovato → -1
 *
 * RICORSIVA (SearchBinRec): usa i e j come indici correnti di ricerca
 * ITERATIVA (SearchBinIter): usa l e h che si avvicinano finché l > h
 *-----------------------------------------------------------------------------*/
int SearchBinRec(int A[], int i, int j, int a) {
    if (i > j) return -1;                        /* range vuoto: non trovato */
    int m = (i + j) / 2;                         /* punto medio */
    if (A[m] == a) return m;                     /* trovato! */
    if (A[m] < a)  return SearchBinRec(A, m + 1, j, a);   /* cerca a destra */
    else           return SearchBinRec(A, i, m - 1, a);   /* cerca a sinistra */
}
int SearchBinIter(int A[], int i, int j, int a) {
    int l = i, h = j;                            /* l=basso, h=alto */
    while (l <= h) {
        int m = (l + h) / 2;                     /* punto medio */
        if (A[m] == a) return m;                 /* trovato! */
        if (A[m] < a)  l = m + 1;               /* a è a destra: alza il basso */
        else           h = m - 1;               /* a è a sinistra: abbassa l'alto */
    }
    return -1;                                   /* l > h: range vuoto, non trovato */
}


/*=============================================================================
 * 2. LISTE SEMPLICI
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * DELETEALL (cancella tutte le occorrenze di x dalla lista)
 *
 * SCOPO: rimuovere ogni nodo con info == x, restituire la lista modificata.
 *
 * COME FUNZIONA (ricorsione):
 *   Se testa è x → salta il nodo: ritorna DeleteAll sul resto (non su se stesso!)
 *   Se testa non è x → tienila, ma fai ricorrere sul suo next
 *
 * BUG CLASSICO:
 *   return l invece di return DeleteAll(x, l->next) quando l->info == x
 *   → il nodo non viene eliminato!
 *-----------------------------------------------------------------------------*/
list DeleteAll(int x, list l) {
    if (l == NULL) return NULL;                        /* lista vuota: fine */
    if (l->info == x) return DeleteAll(x, l->next);   /* salta questo nodo */
    l->next = DeleteAll(x, l->next);                   /* tieni il nodo, ricorri sul resto */
    return l;
}

/*-----------------------------------------------------------------------------
 * ODDFIRST (porta i dispari prima dei pari, mantenendo l'ordine interno)
 *
 * SCOPO: riorganizzare la lista in modo che tutti i dispari vengano prima
 *        dei pari, mantenendo l'ordine relativo di ciascun gruppo.
 *        Esempio: [1,2,3,4,5] → [1,3,5,2,4]
 *
 * STRATEGIA:
 *   Crea due liste separate: dispari e pari.
 *   Poi le concatena con Concat.
 *   Odd_aux: filtra solo i dispari (salta i pari)
 *   Even_aux: filtra solo i pari (salta i dispari)
 *-----------------------------------------------------------------------------*/
list OddFirst(list l) {
    return Concat(Odd_aux(l), Even_aux(l));   /* dispari + pari */
}
list Odd_aux(list l) {
    if (l == NULL) return NULL;
    if (l->info % 2 == 0) return Odd_aux(l->next);        /* pari: salta */
    return Cons(l->info, Odd_aux(l->next));                /* dispari: includi */
}
list Even_aux(list l) {
    if (l == NULL) return NULL;
    if (l->info % 2 != 0) return Even_aux(l->next);       /* dispari: salta */
    return Cons(l->info, Even_aux(l->next));               /* pari: includi */
}

/*-----------------------------------------------------------------------------
 * SYMDIFF (differenza simmetrica di due liste ORDINATE)
 *
 * SCOPO: restituire gli elementi che stanno in una lista ma non nell'altra.
 *        Equivalente a (xs ∪ ys) \ (xs ∩ ys).
 *        Esempio: [1,2,3,4], [2,4,5] → [1,3,5]
 *
 * COME FUNZIONA (su liste ordinate):
 *   xs == NULL → tutto ys è nella differenza (copyList)
 *   ys == NULL → tutto xs è nella differenza
 *   xs->info == ys->info → elemento comune: SCARTA entrambi, ricorri
 *   xs->info < ys->info  → xs->info è solo in xs: INCLUDI, avanza xs
 *   xs->info > ys->info  → ys->info è solo in ys: INCLUDI, avanza ys
 *
 * NOTA: si usa copyList perché le liste originali non devono essere modificate.
 *-----------------------------------------------------------------------------*/
list SymDiff(list xs, list ys) {
    if (xs == NULL) return copyList(ys);                              /* tutto ys */
    if (ys == NULL) return copyList(xs);                              /* tutto xs */
    if (xs->info == ys->info) return SymDiff(xs->next, ys->next);    /* comune: scarta */
    if (xs->info < ys->info)  return Cons(xs->info, SymDiff(xs->next, ys));  /* solo in xs */
    else                      return Cons(ys->info, SymDiff(ys->next, xs));  /* solo in ys */
}

/*-----------------------------------------------------------------------------
 * INSERT (inserisce lista 'as' dentro 'bs' dopo n elementi di bs)
 *
 * SCOPO: inserire la lista as dentro bs in posizione n.
 *        Esempio: Insert([7,8], [1,2,3,4], 2) → [1,2,7,8,3,4]
 *
 * COME FUNZIONA:
 *   n == 0 → siamo nella posizione di inserimento: Concat(as, bs) e fine
 *   n > 0  → non ancora arrivati: bs->next = Insert(as, bs->next, n-1)
 *             tenendo bs come testa
 *
 * NOTA: funziona solo se bs ha almeno n elementi (non controlla bounds).
 *-----------------------------------------------------------------------------*/
list Insert(list as, list bs, int n) {
    if (n == 0) return Concat(as, bs);          /* posizione raggiunta: inserisci */
    bs->next = Insert(as, bs->next, n - 1);     /* avanza in bs */
    return bs;                                  /* mantieni bs come testa */
}

/*-----------------------------------------------------------------------------
 * REVERSE (inverte una lista — versione ricorsiva)
 *
 * SCOPO: restituire la lista con i nodi in ordine inverso.
 *        Esempio: [1,2,3,4] → [4,3,2,1]
 *
 * COME FUNZIONA:
 *   Caso base: lista vuota o un solo nodo → già invertita.
 *   Ricorsione: inverti il resto (l->next in poi), poi "aggancia" l alla fine.
 *     r = Reverse(l->next)    → inverte [2,3,4] in [4,3,2]
 *     l->next->next = l       → il nodo che era dopo l (ora in fondo) punta a l
 *     l->next = NULL          → l diventa l'ultimo nodo
 *     return r                → r è la nuova testa (il 4)
 *
 * TRUCCO: l->next->next = l funziona perché dopo Reverse(l->next),
 *   l->next punta ancora al nodo che era subito dopo l (il 2),
 *   che ora è in fondo alla lista invertita.
 *   Quindi l->next->next = l aggiunge l come ultimo nodo.
 *-----------------------------------------------------------------------------*/
list Reverse(list l) {
    if (l == NULL || l->next == NULL) return l;   /* vuota o un nodo: già invertita */
    list r = Reverse(l->next);                     /* inverti il resto, r = nuova testa */
    l->next->next = l;    /* il vecchio secondo nodo (ora in fondo) punta indietro a l */
    l->next = NULL;       /* l diventa il nuovo ultimo nodo */
    return r;             /* restituisce la nuova testa */
}


/*=============================================================================
 * 3. PILE E CODE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * STACK CON ARRAY
 *
 * SCOPO: implementare una pila (LIFO) usando un array.
 *   top = -1: pila vuota
 *   top = 0: un elemento (in data[0])
 *   top = size-1: pila piena
 *
 * PUSH: incrementa top PRIMA di inserire (S.top++ poi S.data[S.top] = x)
 * POP:  decrementa top DOPO aver letto (leggi data[top], poi top--)
 *
 * ATTENZIONE:
 *   - StackPush e StackPop ricevono Stack per VALORE (non puntatore).
 *     Le modifiche a S.top non si propagano fuori. In un vero programma
 *     si userebbe Stack* S oppure si restituirebbe lo Stack modificato.
 *-----------------------------------------------------------------------------*/
void StackPush(int x, Stack S) {
    if (S.top == S.size) { print("overflow"); return; }  /* pila piena */
    S.top++;                 /* PRIMA incrementa top */
    S.data[S.top] = x;       /* POI inserisce il dato */
}
int StackPop(Stack S) {
    if (S.top == -1) { print("underflow"); return -1; }  /* pila vuota */
    S.top--;                          /* PRIMA decrementa top */
    return S.data[S.top + 1];         /* restituisce il vecchio top */
}

/*-----------------------------------------------------------------------------
 * STACK CON LISTA
 *
 * SCOPO: implementare una pila (LIFO) usando una lista collegata.
 *   La testa della lista è il top della pila.
 *   Push = aggiungi in testa con Cons.
 *   Pop  = leggi la testa e avanza al nodo successivo.
 *
 * NOTA: S è list* (puntatore a lista) per poter modificare la testa.
 *-----------------------------------------------------------------------------*/
void StackPushList(int x, list* S) {
    *S = Cons(x, *S);    /* aggiunge in testa: nuovo nodo diventa il top */
}
int StackPopList(list* S) {
    if (*S == NULL) { print("underflow"); return -1; }  /* pila vuota */
    int x = (*S)->info;        /* legge il valore in cima */
    *S = (*S)->next;           /* avanza: il nuovo top è il secondo nodo */
    return x;
}

/*-----------------------------------------------------------------------------
 * CODA CON ARRAY CIRCOLARE
 *
 * SCOPO: implementare una coda (FIFO) usando un array circolare.
 *   front = indice del primo elemento (da dequeue-are)
 *   rear  = indice della prossima posizione libera (dove fare enqueue)
 *
 * INVARIANTE:
 *   Coda vuota:  front == rear
 *   Coda piena:  (rear + 1) % size == front  (una cella sacrificata!)
 *
 * AVANZARE UN INDICE: idx = (idx + 1) % size  (wrap-around circolare)
 *
 * ENQUEUE:
 *   Calcola sRear = (rear+1) % size
 *   Se sRear == front → overflow (piena)
 *   Altrimenti: metti x in data[rear], poi rear = sRear
 *
 * DEQUEUE:
 *   Se front == rear → underflow (vuota)
 *   Altrimenti: leggi data[front], avanza front
 *
 * ATTENZIONE: in Enqueue si scrive in data[rear] (non data[sRear])!
 *   sRear è solo usato per il controllo di overflow e per aggiornare rear.
 *-----------------------------------------------------------------------------*/
void Enqueue(int x, Queue Q) {
    int sRear = (Q.rear + 1) % Q.size;              /* prossima posizione dopo rear */
    if (sRear == Q.front) { print("overflow"); return; }  /* coda piena */
    Q.data[Q.rear] = x;    /* inserisce in rear (posizione libera corrente) */
    Q.rear = sRear;        /* aggiorna rear alla prossima posizione libera */
}
int Dequeue(Queue Q) {
    if (Q.front == Q.rear) { print("underflow"); return -1; }  /* coda vuota */
    int x = Q.data[Q.front];                    /* legge il primo elemento */
    Q.front = (Q.front + 1) % Q.size;           /* avanza front (circolare) */
    return x;
}

/*-----------------------------------------------------------------------------
 * CODA CON LISTA (QueueList)
 *
 * SCOPO: implementare una coda (FIFO) con due puntatori alla lista:
 *   front = testa della lista = primo elemento da dequeue-are
 *   rear  = ultimo nodo della lista = dove si aggiunge
 *
 * ENQUEUE:
 *   Se front == NULL (coda vuota): front = rear = nuovo nodo
 *   Altrimenti: aggiunge il nuovo nodo in fondo (rear->next) e aggiorna rear
 *
 * DEQUEUE:
 *   Leggi front->info, avanza front.
 *   Se front diventa NULL (l'ultimo elemento è stato rimosso): rear = NULL anche.
 *
 * NOTA: Q è passato per VALORE — le modifiche a Q.front e Q.rear non si
 *   propagano fuori. In un programma reale si usa QueueList* Q.
 *-----------------------------------------------------------------------------*/
void EnqueueList(int x, QueueList Q) {
    if (Q.front == NULL) {
        Q.front = Q.rear = Cons(x, NULL);   /* primo elemento: front e rear coincidono */
    }
    else {
        Q.rear->next = Cons(x, NULL);        /* aggiunge in fondo */
        Q.rear = Q.rear->next;               /* aggiorna rear al nuovo ultimo nodo */
    }
}
int DequeueList(QueueList Q) {
    if (Q.front == NULL) { print("underflow"); return -1; }  /* coda vuota */
    int x = Q.front->info;          /* legge il primo elemento */
    Q.front = Q.front->next;        /* avanza front */
    if (Q.front == NULL) Q.rear = NULL;   /* coda diventata vuota: anche rear a NULL */
    return x;
}


/*=============================================================================
 * 4. HASH TABLE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * HASH TABLE AD INDIRIZZAMENTO APERTO (open addressing)
 *
 * SCOPO: tabella hash dove le collisioni si risolvono cercando un'altra cella
 *        nello stesso array (sondaggio lineare, quadratico o doppio hashing).
 *        hashFun(k, i) restituisce la posizione da provare all'i-esimo tentativo.
 *
 * STRUTTURA: array di interi (0 = NULL = cella libera)
 *
 * HASHINSERTOPEN:
 *   Cerca la prima cella NULL con sondaggio (i=0,1,2,...).
 *   Se la trova, inserisce k lì e restituisce la posizione.
 *   Se non trova spazio (i == dim): tabella piena, restituisce -1.
 *
 * HASHSEARCHOPEN:
 *   Sonda le stesse posizioni di insert.
 *   NULL → la chiave non c'è (una cella vuota "blocca" la catena)
 *   k   → trovato, restituisce la posizione
 *   altro → collisione, continua a sondare
 *
 * BUG CLASSICO: dimenticare i++ nel while → loop infinito!
 *-----------------------------------------------------------------------------*/
int HashInsertOpen(HashTable map, int k) {
    int i = 0;
    while (i < map->dim) {
        int j = hashFun(k, i);           /* posizione da provare all'i-esimo tentativo */
        if (map->array[j] == NULL) {     /* cella libera trovata */
            map->array[j] = k;           /* inserisci k */
            return j;                    /* restituisce la posizione */
        }
        i++;                             /* prova la prossima posizione */
    }
    return -1;   /* tabella piena */
}
int HashSearchOpen(HashTable map, int k) {
    int i = 0;
    while (i < map->dim) {
        int j = hashFun(k, i);           /* posizione da controllare */
        if (map->array[j] == NULL) return -1;   /* cella vuota: k non c'è */
        if (map->array[j] == k)    return j;    /* trovato! */
        i++;                             /* collisione: prova la prossima */
    }
    return -1;   /* esaurite tutte le posizioni */
}

/*-----------------------------------------------------------------------------
 * HASH TABLE A CONCATENAMENTO (chaining)
 *
 * SCOPO: tabella hash dove le collisioni si risolvono con liste collegate.
 *        T è un array di liste: T[j] è la testa della lista per il bucket j.
 *        h(k) calcola l'indice del bucket per la chiave k.
 *
 * HASHINSERTCHAIN:
 *   Calcola j = h(k) e antepone k alla lista T[j] con Cons.
 *   T[j] = Cons(k, T[j]) → k diventa la nuova testa del bucket j.
 *   Non controlla i duplicati (se servisse, andrebbero cercati prima).
 *
 * HASHSEARCHCHAIN:
 *   Calcola j = h(k) e scorre la lista T[j] cercando k.
 *   Restituisce k se trovato, -1 altrimenti.
 *-----------------------------------------------------------------------------*/
void HashInsertChain(list* T, int k) {
    int j = h(k);             /* calcola il bucket di k */
    T[j] = Cons(k, T[j]);    /* antepone k alla lista del bucket (inserimento in testa) */
}
int HashSearchChain(list* T, int k) {
    int j = h(k);             /* calcola il bucket di k */
    list l = T[j];            /* punta alla lista del bucket */
    while (l != NULL) {
        if (l->info == k) return k;   /* trovato: restituisce k */
        l = l->next;                   /* avanza nella lista */
    }
    return -1;   /* non trovato */
}


/*=============================================================================
 * 5. HEAP (MAX-heap e MIN-heap)
 *
 * STRUTTURA: array 0-based
 *   parent(i) = i/2      left(i) = 2*i+1      right(i) = 2*i+2
 *   (se 1-based: parent(i)=i/2, left=2i, right=2i+1)
 *
 * PROPRIETA' MAX-HEAP: ogni nodo >= suoi figli
 * PROPRIETA' MIN-HEAP: ogni nodo <= suoi figli
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * HEAPIFY (ripristina la proprietà MAX-heap dal nodo i verso il basso)
 *
 * SCOPO: assumendo che i sottoalberi di i siano già max-heap,
 *        ma i potrebbe essere più piccolo di un figlio,
 *        Heapify "fa scendere" i fino alla posizione corretta.
 *
 * ATTENZIONE FONDAMENTALE:
 *   max(v[i], max(v[2*i+1], v[2*i+2])) restituisce l'INDICE del massimo,
 *   NON il valore! (Questo è il comportamento della max() preimplementata.)
 *
 * COME FUNZIONA:
 *   biggest = indice del massimo tra il nodo i e i suoi due figli
 *   Se biggest != i → il nodo i non è il massimo: swap e ricorri sull'indice spostato
 *   Se biggest == i → il nodo i è già il massimo: stop
 *
 * NOTA: non controlla che 2*i+1 e 2*i+2 siano < n (il CodeRunner gestisce questo).
 *-----------------------------------------------------------------------------*/
void Heapify(int v[], int i, int n) {
    /* biggest = INDICE (non valore) del massimo tra v[i], v[figlio sx], v[figlio dx] */
    int biggest = max(v[i], max(v[2*i+1], v[2*i+2]));
    if (biggest != i) {          /* i non è il massimo: deve scendere */
        swap(v, i, biggest);     /* scambia i con il figlio maggiore */
        Heapify(v, biggest, n);  /* ricorri sul nodo spostato (ora in posizione biggest) */
    }
    /* se biggest == i: il nodo è già nella posizione corretta, stop */
}

/*-----------------------------------------------------------------------------
 * BUILDHEAP (costruisce un MAX-heap da un array non ordinato)
 *
 * SCOPO: trasformare un array qualsiasi in un max-heap in O(n).
 *        (Inserire n elementi uno per uno sarebbe O(n log n).)
 *
 * COME FUNZIONA:
 *   Le foglie (indici n/2 .. n-1) sono già max-heap da sole (un nodo solo).
 *   Si applica Heapify partendo dall'ultimo nodo INTERNO (indice n/2 - 1)
 *   e si risale verso la radice (indice 0).
 *
 * ATTENZIONE BUG CLASSICO:
 *   Il ciclo deve essere: i >= 0  (non i > 0!)
 *   Con i > 0 si salta la radice (indice 0) → heap non corretto!
 *
 * NOTA: indice n/2-1 è corretto per array 0-based.
 *   Se l'array fosse 1-based, l'ultimo nodo interno sarebbe n/2.
 *-----------------------------------------------------------------------------*/
void BuildHeap(int v[], int n) {
    /* parte dall'ultimo nodo interno (le foglie sono già heap) */
    for (int i = n/2 - 1; i >= 0; i--) {  /* ATTENZIONE: >= 0, non > 0 ! */
        Heapify(v, i, n);
    }
}

/*-----------------------------------------------------------------------------
 * HEAPSORT
 *
 * SCOPO: ordinare l'array in ordine crescente usando il max-heap.
 *        Complessità O(n log n) nel caso peggiore (meglio di QuickSort worst case).
 *
 * COME FUNZIONA:
 *   1) BuildHeap: trasforma l'array in max-heap → il massimo è in v[0]
 *   2) Ad ogni passo:
 *      - swap(v, 0, i): porta il massimo corrente (v[0]) in fondo (posizione i)
 *      - Heapify(v, 0, i): ripristina la proprietà heap sulla parte rimanente [0..i-1]
 *        (i = dimensione ridotta della parte heap)
 *   Il risultato è ordinato in crescente perché i massimi vengono messi da destra.
 *-----------------------------------------------------------------------------*/
void HeapSort(int v[], int n) {
    BuildHeap(v, n);                       /* step 1: costruisci il max-heap */
    for (int i = n - 1; i >= 0; i--) {
        swap(v, 0, i);          /* porta il massimo in posizione i (ultima posizione della heap) */
        Heapify(v, 0, i);       /* i = nuova dimensione ridotta: la heap ora è [0..i-1] */
    }
}

/*-----------------------------------------------------------------------------
 * HEAPINSERTMIN (inserimento in MIN-heap)
 *
 * SCOPO: aggiungere un elemento x mantenendo la proprietà min-heap.
 *
 * COME FUNZIONA:
 *   1) Incrementa size e inserisce x in fondo (posizione size-1).
 *   2) "Sift up": finché x è minore del suo padre (h[i/2] > h[i]),
 *      scambia x con il padre e sale verso la radice.
 *   Condizione di stop: i == 0 (arrivato alla radice) o padre <= figlio.
 *
 * RIGA PER RIGA:
 *   (*size)++      → incrementa la dimensione (dereferenziato perché è int*)
 *   i = *size - 1  → indice dell'ultima posizione (0-based)
 *   h[i] = x       → inserisce x in fondo
 *   while i>0 && h[i/2]>h[i]: → risali finché padre > figlio
 *     swap(h, i, i/2)  → scambia con il padre
 *     i = i/2          → sale al padre
 *-----------------------------------------------------------------------------*/
void HeapInsertMin(int h[], int* size, int x) {
    (*size)++;                       /* incrementa la dimensione */
    int i = *size - 1;               /* indice dell'ultimo elemento (0-based) */
    h[i] = x;                        /* inserisce x in fondo */
    while (i > 0 && h[i/2] > h[i]) { /* risali: padre > figlio (viola min-heap) */
        swap(h, i, i/2);             /* scambia con il padre */
        i = i / 2;                   /* sali al padre */
    }
}

/*-----------------------------------------------------------------------------
 * EXTRACTMIN (estrae il minimo da un MIN-heap)
 *
 * SCOPO: rimuovere e restituire l'elemento minimo (la radice).
 *
 * COME FUNZIONA:
 *   1) Salva il minimo (h[0]).
 *   2) Porta l'ultimo elemento alla radice (h[0] = h[size-1]).
 *   3) Riduce size.
 *   4) Heapify dalla radice per ripristinare la proprietà min-heap.
 *      (Heapify va adattata: usa min invece di max per trovare il figlio minore.)
 *   Restituisce il valore salvato.
 *-----------------------------------------------------------------------------*/
int ExtractMin(int h[], int* size) {
    int res = h[0];              /* salva il minimo (radice) */
    h[0] = h[*size - 1];        /* porta l'ultimo elemento alla radice */
    (*size)--;                   /* riduce la dimensione */
    Heapify(h, 0, *size);        /* ripristina la proprietà min-heap (usa min al posto di max) */
    return res;                  /* restituisce il minimo estratto */
}


/*=============================================================================
 * 6. ALBERI K-ARI (rappresentazione figlio-fratello)
 *
 * STRUTTURA: { int info; kTree child; kTree sibling; }
 *   child   = puntatore al PRIMO figlio
 *   sibling = puntatore al fratello SUCCESSIVO (stesso padre, stesso livello)
 *
 * PATTERN FONDAMENTALE per scorrere tutti i figli di t:
 *   kTree c = t->child;
 *   while (c != NULL) {
 *       ... usa c ...
 *       c = c->sibling;
 *   }
 *
 * FOGLIA: t->child == NULL  (nessun figlio)
 * NODO INTERNO: t->child != NULL
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * HEIGHT (altezza dell'albero)
 *
 * SCOPO: calcolare la distanza massima dalla radice a una foglia.
 *        Foglia → altezza 0. Nodo interno → 1 + max altezze dei figli.
 *
 * COME FUNZIONA:
 *   Foglia (child == NULL) → restituisce 0
 *   Nodo interno → calcola l'altezza di ogni figlio, prende il massimo, aggiunge 1
 *
 * RIGA PER RIGA:
 *   ht = 0            → altezza minima tra i figli finora trovata
 *   c = t->child      → inizia dal primo figlio
 *   ht = max(ht, Height(c)) → aggiorna il massimo
 *   c = c->sibling    → passa al fratello successivo
 *   return ht + 1     → aggiunge 1 per il livello del nodo corrente
 *-----------------------------------------------------------------------------*/
int Height(kTree t) {
    if (t->child == NULL) return 0;    /* foglia: altezza 0 */
    int ht = 0;
    kTree c = t->child;               /* inizia dal primo figlio */
    while (c != NULL) {
        ht = max(ht, Height(c));       /* prendi il massimo tra tutti i figli */
        c = c->sibling;               /* passa al fratello (prossimo figlio) */
    }
    return ht + 1;                    /* aggiungi 1 per questo livello */
}

/*-----------------------------------------------------------------------------
 * SUMLEAF (somma delle etichette delle foglie)
 *
 * SCOPO: calcolare la somma dei valori info di tutte le foglie.
 *
 * COME FUNZIONA:
 *   t == NULL  → 0 (albero vuoto, sicurezza)
 *   Foglia     → restituisce t->info (questo nodo conta)
 *   Interno    → somma le SumLeaf di tutti i figli
 *
 * NOTA: a differenza di Height, qui il controllo t == NULL è necessario
 *   per gestire il caso di albero vuoto passato dall'esterno.
 *-----------------------------------------------------------------------------*/
int SumLeaf(kTree t) {
    if (t == NULL)          return 0;          /* albero vuoto */
    if (t->child == NULL)   return t->info;    /* foglia: conta questo valore */
    int res = 0;
    kTree c = t->child;
    while (c != NULL) {
        res += SumLeaf(c);    /* somma le foglie di ogni sottoalbero */
        c = c->sibling;
    }
    return res;
}

/*-----------------------------------------------------------------------------
 * KTREEBFS (visita in ampiezza = Breadth First Search)
 *
 * SCOPO: restituire la lista delle etichette nell'ordine di visita BFS
 *        (livello per livello, da sinistra a destra).
 *
 * COME FUNZIONA:
 *   Usa una coda (FIFO) per tenere traccia dei nodi da visitare.
 *   Ogni volta che visita un nodo, aggiunge tutti i suoi figli alla coda.
 *   Costruisce la lista con Cons → lista inversa → Reverse finale.
 *
 * RIGA PER RIGA:
 *   EnQueue(t, q)      → inserisce la radice nella coda
 *   DeQueue(q)         → estrae il prossimo da visitare
 *   l = Cons(n->info, l) → aggiunge in TESTA alla lista (quindi al contrario)
 *   EnQueue(c, q)      → aggiunge ogni figlio alla coda
 *   return Reverse(l)  → inverte per avere l'ordine corretto BFS
 *
 * BUG CLASSICO: return l invece di return Reverse(l) → lista inversa!
 *-----------------------------------------------------------------------------*/
list kTreeBFS(kTree t) {
    if (t == NULL) return NULL;
    list l = NULL;
    queue q = NewQueue();
    EnQueue(t, q);                            /* metti la radice in coda */
    while (!isEmpty(q)) {
        kTree n = DeQueue(q);                 /* estrai il prossimo nodo */
        l = Cons(n->info, l);                 /* aggiunge in testa (costruisce al contrario) */
        kTree c = n->child;
        while (c != NULL) {
            EnQueue(c, q);                    /* aggiungi tutti i figli alla coda */
            c = c->sibling;
        }
    }
    return Reverse(l);   /* OBBLIGATORIO: inverti per ottenere ordine BFS corretto */
}

/*-----------------------------------------------------------------------------
 * KTREEDFS (visita in profondità = Depth First Search)
 *
 * SCOPO: restituire la lista delle etichette nell'ordine DFS (pre-ordine).
 *
 * COME FUNZIONA:
 *   Identica a BFS ma usa una PILA (LIFO) invece della coda.
 *   Con la pila i figli vengono visitati in ordine inverso rispetto a BFS.
 *   Stessa costruzione al contrario con Cons → Reverse alla fine.
 *
 * DIFFERENZA BFS vs DFS:
 *   BFS usa Queue (FIFO): visita livello per livello
 *   DFS usa Stack (LIFO): scende in profondità prima di allargare
 *-----------------------------------------------------------------------------*/
list kTreeDFS(kTree t) {
    if (t == NULL) return NULL;
    list l = NULL;
    stack s = NewStack();
    Push(t, s);                               /* metti la radice nello stack */
    while (!isEmpty(s)) {
        kTree n = Pop(s);                     /* estrai il prossimo nodo */
        l = Cons(n->info, l);                 /* aggiunge in testa */
        kTree c = n->child;
        while (c != NULL) {
            Push(c, s);                       /* aggiungi tutti i figli allo stack */
            c = c->sibling;
        }
    }
    return Reverse(l);   /* inverti per ordine DFS corretto */
}

/*-----------------------------------------------------------------------------
 * SUM (verifica proprietà: ogni nodo = somma dei suoi figli)
 *
 * SCOPO: restituire true se per ogni nodo interno vale: info == somma dei figli.
 *        Le foglie soddisfano la proprietà vacuamente (true).
 *
 * COME FUNZIONA:
 *   Foglia → true (caso base, condizione vacuamente vera)
 *   Interno:
 *     - calcola la somma delle info di tutti i figli
 *     - verifica ricorsivamente Sum su ogni figlio
 *     - restituisce true sse info == somma E tutti i figli soddisfano Sum
 *
 * RIGA PER RIGA:
 *   res = 0, b = true  → inizializza somma e flag
 *   res += c->info      → accumula somma diretta dei figli (non ricorsiva)
 *   b = b && Sum(c)     → verifica ricorsiva su ogni sottoalbero
 *   return (t->info == res) && b
 *-----------------------------------------------------------------------------*/
bool Sum(kTree t) {
    if (t->child == NULL) return true;   /* foglia: proprietà soddisfatta */
    int res = 0;
    bool b = true;
    kTree c = t->child;
    while (c != NULL) {
        res += c->info;                  /* somma le info dirette dei figli */
        b = b && Sum(c);                 /* verifica ricorsiva nei sottoalberi */
        c = c->sibling;
    }
    return (t->info == res) && b;        /* questo nodo e tutti i sottoalberi OK */
}

/*-----------------------------------------------------------------------------
 * SOMMARAMO / SOMMACAMMINO
 *
 * SCOPO: per ogni foglia, aggiungere un nuovo figlio il cui valore è la
 *        somma delle etichette lungo il cammino dalla radice a quella foglia.
 *        Esempio: radice=1, figlio=2, foglia=3 → nuovo figlio della foglia = 1+2+3=6
 *
 * COME FUNZIONA:
 *   SommaRamo chiama SommaCammino(t, 0) passando accumulatore iniziale 0.
 *   SommaCammino riceve l'accumulatore s (somma finora):
 *     1) aggiorna s = s + t->info  (aggiunge il nodo corrente)
 *     2) se foglia → crea un nuovo nodo figlio con valore s
 *     3) se interno → ricorre sui figli con l'accumulatore aggiornato
 *
 * ATTENZIONE: s = s + t->info PRIMA della ricorsione/creazione del figlio.
 *   Se lo fai dopo, il nodo corrente non viene incluso nella somma.
 *-----------------------------------------------------------------------------*/
void SommaRamo(kTree t) {
    SommaCammino(t, 0);   /* avvia con accumulatore 0 */
}
void SommaCammino(kTree t, int s) {
    s = s + t->info;           /* aggiorna la somma con il nodo corrente */
    if (t->child == NULL) {    /* foglia: aggiungi un figlio con la somma del cammino */
        kTree n = malloc(sizeof(node));
        n->info = s;           /* il valore è la somma dell'intero cammino */
        n->child = NULL;
        n->sibling = NULL;
        t->child = n;          /* diventa l'unico figlio della foglia */
    }
    else {                     /* nodo interno: ricorre su tutti i figli */
        kTree c = t->child;
        while (c != NULL) {
            SommaCammino(c, s);    /* passa l'accumulatore aggiornato */
            c = c->sibling;
        }
    }
}

/*-----------------------------------------------------------------------------
 * NODIPROFONDI (conta i nodi a distanza <= h dalla radice)
 *
 * SCOPO: contare quanti nodi si trovano ai livelli 0, 1, ..., h.
 *        h=0 → solo la radice (1 nodo).
 *        h=1 → radice + tutti i figli diretti.
 *
 * COME FUNZIONA:
 *   h == 0 → conta solo la radice: return 1
 *   h > 0  → 1 (questo nodo) + somma dei NodiProfondi dei figli con h-1
 *             (ogni figlio può contribuire con i propri sottoalberi fino a h-1 livelli)
 *
 * RIGA PER RIGA:
 *   if h==0 return 1    → caso base: la radice stessa
 *   n = 1               → conta il nodo corrente
 *   n += NodiProfondi(c, h-1) → aggiungi i nodi nel sottoalbero di ogni figlio
 *                                 fino a profondità h-1
 *-----------------------------------------------------------------------------*/
int NodiProfondi(kTree t, int h) {
    if (h == 0) return 1;    /* profondità 0: solo la radice corrente */
    int n = 1;               /* conta il nodo corrente */
    kTree c = t->child;
    while (c != NULL) {
        n += NodiProfondi(c, h - 1);   /* nodi nel sottoalbero del figlio fino a h-1 */
        c = c->sibling;
    }
    return n;
}

/*-----------------------------------------------------------------------------
 * MAXSUMBRANCH (massima somma lungo un ramo dalla radice a una foglia)
 *
 * SCOPO: trovare il ramo (dalla radice a una foglia) con la somma massima
 *        delle etichette. Restituisce il valore di quella somma.
 *
 * COME FUNZIONA:
 *   Foglia → restituisce t->info (contributo di questa foglia)
 *   Interno → t->info + max delle MaxSumBranch di tutti i figli
 *             (scegli il ramo migliore tra i figli, aggiungi questo nodo)
 *
 * RIGA PER RIGA:
 *   if foglia return t->info    → caso base
 *   maxS = 0                    → massima somma dei figli finora
 *   maxS = max(maxS, MaxSumBranch(c)) → aggiorna con il figlio migliore
 *   return t->info + maxS       → somma questo nodo + il ramo migliore
 *-----------------------------------------------------------------------------*/
int MaxSumBranch(kTree t) {
    if (t->child == NULL) return t->info;    /* foglia: contributo solo suo */
    int maxS = 0;
    kTree c = t->child;
    while (c != NULL) {
        maxS = max(maxS, MaxSumBranch(c));   /* scegli il figlio con ramo più ricco */
        c = c->sibling;
    }
    return t->info + maxS;    /* aggiungi il valore di questo nodo al ramo migliore */
}

/*-----------------------------------------------------------------------------
 * SHORTEST (numero di nodi lungo il ramo più corto)
 *
 * SCOPO: trovare il ramo (dalla radice a una foglia) più corto in numero di nodi.
 *        Restituisce il conteggio di nodi lungo quel ramo (foglia conta 1).
 *
 * COME FUNZIONA:
 *   Foglia → return 1 (conta se stessa)
 *   Interno → 1 + min dei Shortest dei figli
 *
 * NOTA: usa INT_MAX come inizializzazione per il minimo (garantisce che
 *       il primo figlio sovrascriva subito il valore).
 *-----------------------------------------------------------------------------*/
int Shortest(kTree t) {
    if (t->child == NULL) return 1;    /* foglia: un solo nodo */
    int res = INT_MAX;                 /* inizia con infinito per trovare il minimo */
    kTree c = t->child;
    while (c != NULL) {
        res = min(res, Shortest(c));   /* scegli il ramo più corto tra i figli */
        c = c->sibling;
    }
    return res + 1;    /* aggiungi 1 per questo nodo */
}

/*-----------------------------------------------------------------------------
 * RANK (grado dell'albero = numero massimo di figli di un nodo)
 *
 * SCOPO: trovare il nodo con più figli e restituire quel numero.
 *        Il "grado" o "rank" di un albero k-ario è il k massimo effettivo.
 *
 * COME FUNZIONA (BFS):
 *   Usa una coda per visitare tutti i nodi.
 *   Per ogni nodo, conta quanti figli ha scorrendo child/sibling.
 *   Aggiorna maxFigli se il conteggio supera il massimo corrente.
 *
 * NOTA: usa la coda generica (queue) invece di ricorsione, per evitare
 *   overflow dello stack su alberi molto profondi.
 *-----------------------------------------------------------------------------*/
int Rank(kTree t) {
    if (t == NULL) return 0;
    int maxFigli = 0;
    queue q = NewQueue();
    EnQueue(t, q);
    while (!isEmpty(q)) {
        kTree n = DeQueue(q);
        int count = 0;
        kTree c = n->child;
        while (c != NULL) {
            count++;                   /* conta i figli di questo nodo */
            EnQueue(c, q);            /* aggiunge il figlio alla coda per visitarlo dopo */
            c = c->sibling;
        }
        if (count > maxFigli) maxFigli = count;   /* aggiorna il massimo */
    }
    return maxFigli;
}

/*-----------------------------------------------------------------------------
 * GENERATOR (genera un albero k-ario completo)
 *
 * SCOPO: creare un albero con altezza ht, grado rk, e tutti i nodi con valore key.
 *        Un albero "completo k-ario" ha esattamente rk figli per ogni nodo interno
 *        e altezza ht (le foglie sono al livello ht).
 *
 * COME FUNZIONA:
 *   1) Crea il nodo radice con valore key.
 *   2) Se ht==0 o rk==0: foglia → child = NULL.
 *   3) Altrimenti: crea rk figli, ognuno è Generator(ht-1, rk, key).
 *      I figli sono collegati tra loro con sibling:
 *        t->child = primo figlio
 *        primo->sibling = secondo figlio
 *        secondo->sibling = terzo figlio
 *        ...e così via
 *
 * RIGA PER RIGA:
 *   t->child = Generator(ht-1, rk, key)  → crea il primo figlio
 *   curr = t->child                       → punta al primo figlio
 *   for i da 1 a rk-1:
 *     curr->sibling = Generator(ht-1, rk, key)  → crea il fratello successivo
 *     curr = curr->sibling                        → avanza al nuovo fratello
 *
 * BUG CLASSICO: creare figli con altezza ht-0 invece di ht-1 → ricorsione infinita!
 *-----------------------------------------------------------------------------*/
kTree Generator(int ht, int rk, int key) {
    kTree t = malloc(sizeof(node));    /* alloca il nodo */
    t->info = key;                     /* imposta il valore */
    t->sibling = NULL;                 /* nessun fratello (lo imposterà il chiamante se necessario) */
    if (ht == 0 || rk == 0) {
        t->child = NULL;               /* foglia: nessun figlio */
        return t;
    }
    t->child = Generator(ht - 1, rk, key);   /* crea il primo figlio */
    kTree curr = t->child;
    for (int i = 1; i < rk; i++) {
        curr->sibling = Generator(ht - 1, rk, key);   /* crea il prossimo fratello */
        curr = curr->sibling;                          /* avanza all'ultimo fratello */
    }
    return t;
}


/*=============================================================================
 * 7. ALBERI BINARI DI RICERCA (BST)
 *
 * STRUTTURA: { int info; btree left, right, parent; }
 *
 * PROPRIETA' BST: per ogni nodo n:
 *   - tutti i nodi nel sottoalbero SINISTRO hanno chiave < n->info
 *   - tutti i nodi nel sottoalbero DESTRO hanno chiave > n->info
 *
 * CONSEGUENZE:
 *   - il MINIMO è il nodo più a sinistra (scendi sempre left)
 *   - il MASSIMO è il nodo più a destra (scendi sempre right)
 *   - la visita IN-ORDER (sx, radice, dx) dà le chiavi in ordine crescente
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * MININBTREE (trova il nodo con chiave minima)
 *
 * SCOPO: trovare il nodo con il valore più piccolo nel BST (o sottoalbero).
 *        Usato anche da Successor.
 *
 * COME FUNZIONA: scende sempre a sinistra finché non trova un nodo senza figlio sx.
 *   Il minimo è il nodo più a sinistra (proprietà BST).
 *-----------------------------------------------------------------------------*/
btree MinInBTree(btree bt) {
    if (bt->left == NULL) return bt;          /* nessun figlio sx: questo è il minimo */
    return MinInBTree(bt->left);              /* scendi ancora a sinistra */
}

/*-----------------------------------------------------------------------------
 * RIGHTANCESTOR (trova il primo antenato destro = primo antenato
 *                di cui questo nodo è nel sottoalbero SINISTRO)
 *
 * SCOPO: trovare il più piccolo antenato con chiave maggiore del nodo corrente.
 *        Usato da Successor quando il nodo non ha figlio destro.
 *
 * COME FUNZIONA:
 *   Risale verso la radice tramite parent.
 *   Se il nodo corrente è il figlio SINISTRO del padre → il padre è l'antenato destro.
 *   Altrimenti continua a salire.
 *   Se si arriva alla radice senza trovarlo (parent == NULL) → non esiste: return NULL.
 *
 * BUG CLASSICO: usare = invece di == nel confronto dei puntatori:
 *   Sbagliato: if (bt->parent->left = bt)    → assegnamento, sempre true!
 *   Corretto:  if (bt->parent->left == bt)   → confronto
 *-----------------------------------------------------------------------------*/
btree RightAncestor(btree bt) {
    if (bt->parent == NULL) return NULL;             /* radice: nessun antenato dx */
    if (bt->parent->left == bt) return bt->parent;   /* sono figlio sx: il padre è l'antenato dx */
    return RightAncestor(bt->parent);                /* sono figlio dx: continua a salire */
}

/*-----------------------------------------------------------------------------
 * SUCCESSOR (trova il successore in un BST = il minimo dei maggiori)
 *
 * SCOPO: trovare il nodo con la chiave immediatamente successiva (più piccola
 *        tra quelle maggiori di bt->info).
 *
 * DUE CASI:
 *   1) bt ha un figlio destro → il successore è il MINIMO del sottoalbero destro
 *      (il nodo più piccolo tra quelli maggiori di bt)
 *   2) bt NON ha figlio destro → bisogna risalire: il successore è il primo
 *      antenato di cui bt è nel sottoalbero SINISTRO (RightAncestor)
 *
 * Restituisce NULL se bt è il massimo (nessun successore).
 *-----------------------------------------------------------------------------*/
btree Successor(btree bt) {
    if (bt->right != NULL) return MinInBTree(bt->right);   /* caso 1: minimo del sottoalbero dx */
    return RightAncestor(bt);                               /* caso 2: primo antenato destro */
}

/*-----------------------------------------------------------------------------
 * DECLIST (lista chiavi in ordine DECRESCENTE)
 *
 * SCOPO: restituire la lista di tutte le chiavi del BST in ordine decrescente.
 *        Esempio: BST con chiavi 1,2,3,4,5 → lista [5,4,3,2,1]
 *
 * COME FUNZIONA (visita in-order invertita: dx, radice, sx):
 *   La visita normale in-order (sx, radice, dx) dà ordine crescente.
 *   Invertendo (prima scendi a sx, poi preponi la radice, poi scendi a dx),
 *   grazie all'accumulatore, si ottiene l'ordine decrescente.
 *
 * RIGA PER RIGA di DecList_aux(bt, l):
 *   if bt==NULL return l    → albero vuoto: restituisce lista accumulata finora
 *   l = DecList_aux(bt->left, l)   → prima scendi a sinistra (valori piccoli)
 *   l = Cons(bt->info, l)          → poi preponi la radice (va in testa)
 *   return DecList_aux(bt->right, l) → poi scendi a destra (valori grandi)
 *
 * Perché questo dà ordine decrescente?
 *   I valori piccoli (sinistra) vengono processati per primi e finiscono
 *   in testa (Cons). I valori grandi (destra) vengono processati dopo
 *   e sovrascrivono le teste. Il risultato è lista decrescente.
 *-----------------------------------------------------------------------------*/
list DecList(btree bt) {
    return DecList_aux(bt, NULL);    /* avvia con lista vuota come accumulatore */
}
list DecList_aux(btree bt, list l) {
    if (bt == NULL) return l;                        /* caso base: restituisce l'accumulatore */
    l = DecList_aux(bt->left, l);                    /* prima processa il sottoalbero sx */
    l = Cons(bt->info, l);                           /* preponi la radice alla lista */
    return DecList_aux(bt->right, l);                /* poi processa il sottoalbero dx */
}

/*-----------------------------------------------------------------------------
 * ISORDERED (verifica se un albero binario è un BST)
 *
 * SCOPO: controllare se l'albero soddisfa la proprietà BST.
 *        Non basta confrontare ogni nodo con i figli diretti:
 *        bisogna garantire che TUTTI i nodi del sottoalbero sinistro siano
 *        minori e TUTTI quelli del destro siano maggiori.
 *
 * SOLUZIONE: usare IsOrdered_aux che restituisce Triple {isOrdered, min, max}
 *   così si propagano il minimo e massimo di ogni sottoalbero per verificare
 *   la condizione globale.
 *
 * ATTENZIONE: IsOrdered_aux restituisce Triple, non bool!
 *   Il tipo di ritorno è fondamentale.
 *
 * QUATTRO CASI in IsOrdered_aux:
 *   1) Foglia: isOrdered=true, min=max=info
 *   2) Solo figlio sx: verifica info > tl.max (la radice deve essere > tutti a sx)
 *      min=tl.min, max=info
 *   3) Solo figlio dx: verifica info < tr.min (la radice deve essere < tutti a dx)
 *      min=info, max=tr.max
 *   4) Entrambi i figli: verifica entrambe le condizioni e che entrambi siano BST
 *      min=tl.min, max=tr.max
 *-----------------------------------------------------------------------------*/
bool IsOrdered(btree bt) {
    if (bt == NULL) return true;                 /* albero vuoto: è BST per definizione */
    return IsOrdered_aux(bt).isOrdered;
}
Triple IsOrdered_aux(btree bt) {
    Triple t;
    if (bt->left == NULL && bt->right == NULL) {
        /* Caso 1: foglia */
        t.isOrdered = true;
        t.min = bt->info;
        t.max = bt->info;
    }
    else if (bt->right == NULL) {
        /* Caso 2: solo figlio sinistro */
        Triple tl = IsOrdered_aux(bt->left);
        t.isOrdered = tl.isOrdered && (bt->info > tl.max);  /* radice > massimo sx */
        t.min = tl.min;                                       /* minimo = minimo del sottoalbero sx */
        t.max = bt->info;                                     /* massimo = radice (è la più grande) */
    }
    else if (bt->left == NULL) {
        /* Caso 3: solo figlio destro */
        Triple tr = IsOrdered_aux(bt->right);
        t.isOrdered = tr.isOrdered && (bt->info < tr.min);  /* radice < minimo dx */
        t.min = bt->info;                                     /* minimo = radice (è la più piccola) */
        t.max = tr.max;                                       /* massimo = massimo del sottoalbero dx */
    }
    else {
        /* Caso 4: entrambi i figli */
        Triple tl = IsOrdered_aux(bt->left);
        Triple tr = IsOrdered_aux(bt->right);
        t.isOrdered = tl.isOrdered && tr.isOrdered
                      && (bt->info > tl.max)    /* radice > massimo sx */
                      && (bt->info < tr.min);   /* radice < minimo dx */
        t.min = tl.min;    /* minimo = minimo del sottoalbero sx */
        t.max = tr.max;    /* massimo = massimo del sottoalbero dx */
    }
    return t;
}

/*-----------------------------------------------------------------------------
 * ANTENATO COMUNE (Lowest Common Ancestor in un BST)
 *
 * SCOPO: trovare il nodo più profondo che è antenato comune di due nodi
 *        con chiavi a e b (con a <= b).
 *
 * COME FUNZIONA (sfrutta la proprietà BST):
 *   Se a <= radice <= b → la radice è l'LCA (a è a sx o uguale, b è a dx o uguale)
 *   Se b < radice       → entrambi a e b sono nel sottoalbero sx → scendi a sx
 *   Se a > radice       → entrambi a e b sono nel sottoalbero dx → scendi a dx
 *
 * NOTA: funziona solo su BST, non su alberi binari generici.
 *   Su un albero generico servirebbero strategie diverse (es. risalire con parent).
 *
 * ATTENZIONE: il pre-requisito è a <= b (a < b per nodi distinti).
 *-----------------------------------------------------------------------------*/
btree AntenatoComune(btree bt, int a, int b) {
    if (a <= bt->info && bt->info <= b) return bt;     /* a <= radice <= b: questo è l'LCA */
    if (b < bt->info)  return AntenatoComune(bt->left, a, b);   /* entrambi a sx */
    return AntenatoComune(bt->right, a, b);            /* entrambi a dx */
}

/*-----------------------------------------------------------------------------
 * INSERTBST (inserimento in un BST)
 *
 * SCOPO: aggiungere la chiave k nel BST mantenendo la proprietà BST.
 *        Restituisce la radice aggiornata (necessario perché l'albero
 *        potrebbe essere inizialmente vuoto).
 *
 * COME FUNZIONA:
 *   bt == NULL → crea un nuovo nodo foglia con valore k
 *   k == info  → già presente, non inserire (no duplicati in BST)
 *   k < info   → k appartiene al sottoalbero sx: inserisci lì e aggiorna left
 *   k > info   → k appartiene al sottoalbero dx: inserisci lì e aggiorna right
 *
 * RIGA PER RIGA:
 *   if bt==NULL → alloca nuovo nodo, imposta info=k, left=right=NULL
 *   if k==info → duplicato: restituisci bt senza modifiche
 *   if k < info → bt->left = InsertBST(k, bt->left)  (può creare nuovo nodo)
 *   else        → bt->right = InsertBST(k, bt->right)
 *   return bt   → restituisce la radice (invariata tranne per un figlio aggiornato)
 *
 * NOTA: NewNode() è la funzione preimplementata che alloca un btree node.
 *-----------------------------------------------------------------------------*/
btree InsertBST(int k, btree bt) {
    if (bt == NULL) {
        btree n = NewNode();     /* alloca nuovo nodo */
        n->info = k;             /* imposta il valore */
        n->left = NULL;
        n->right = NULL;
        return n;                /* il nuovo nodo è una foglia */
    }
    if (k == bt->info) return bt;            /* già presente: non inserire */
    if (k < bt->info) {
        bt->left = InsertBST(k, bt->left);   /* inserisci nel sottoalbero sx */
    }
    else {
        bt->right = InsertBST(k, bt->right); /* inserisci nel sottoalbero dx */
    }
    return bt;    /* restituisce la radice (modificata solo in un sottoalbero) */
}


/*=============================================================================
 * RIEPILOGO BUG CLASSICI DA EVITARE
 *=============================================================================
 *
 * [1]  Liste/alberi:   if (l == NULL) o if (t == NULL) SEMPRE come primo controllo
 *                      → segfault altrimenti quando dereferenzi NULL
 *
 * [2]  Partition:      i++ PRIMA di swap(A,i,j), NON dopo!
 *                      Sbagliato: swap(A,i,j); i++;
 *                      Corretto:  i++; swap(A,i,j);
 *
 * [3]  CountBubble:    flag deve essere bool, si resetta a false all'inizio di ogni passata
 *
 * [4]  MaxLen:         controlla count > maxLen DOPO il ciclo for!
 *                      (l'ultimo segmento non entra mai nell'else)
 *
 * [5]  BuildHeap:      il ciclo deve essere i >= 0, NON i > 0
 *                      (altrimenti si salta la radice che è l'indice 0)
 *
 * [6]  BFS/DFS:        return Reverse(l) alla fine, NON return l
 *                      (Cons costruisce la lista al contrario)
 *
 * [7]  HashOpen:       i++ alla fine del while, altrimenti loop infinito
 *
 * [8]  RightAncestor:  usa == per confrontare puntatori, NON =
 *                      (= è assegnamento e dà sempre true)
 *
 * [9]  IsOrdered_aux:  il tipo di ritorno è Triple, NON bool
 *                      (deve propagare min e max, non solo isOrdered)
 *
 * [10] Generator:      ogni figlio ha altezza ht-1, NON ht
 *                      (altrimenti ricorsione infinita)
 *
 * [11] ExpRec:         calcola y=ExpRec(x,n/2) UNA SOLA VOLTA, non due!
 *                      Sbagliato: return ExpRec(x,n/2)*ExpRec(x,n/2);  → O(n)
 *                      Corretto:  int y=ExpRec(x,n/2); return y*y;     → O(log n)
 *
 * [12] Heapify:        max() e min() restituiscono INDICE, non valore!
 *                      biggest = max(v[i], max(v[2i+1], v[2i+2])) → indice
 *
 * [13] list è già un puntatore! Non scrivere list* l per una lista,
 *                      ma list l (equivale a struct listNode* l)
 *
 *=============================================================================*/
