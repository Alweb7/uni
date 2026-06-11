/*Esercizi_Ultimo_Esame.c*/

/*1°:
 *Data una lista di interi scrivere una funzione che ordini i dispari prima dei pari.
 *L'ordine dei dispari e dei pari tra loro deve rimanere invariato.
 *La funzione deve ritornare una nuova lista.
 *preimplementate le funzioni Cons(int x, list l) e Concat(list l, list m).
 *Esempio: 1-4-5-6-12-25-3-7-10 diventa 1-5-25-17-4-6-12-10.
*/
list OddFirst(list l) {
    return Concat(Odd_aux(l), Even_aux(l));
}
list Odd_aux(list l) {
    if(l == NULL) {
        return l;
    }
    else {
        if(l->info % 2 == 0) {
            return Odd_aux(l->next);
        }
        else {
            return Cons(l->info, Odd_aux(l->next));
        }
    }
}
list Even_aux(list l) {
    if(l == NULL) {
        return l;
    }
    else {
        if(l->info % 2 != 0) {
            return Even_aux(l->next);
        }
        else {
            return Cons(l->info, Even_aux(l->next));
        }
    }
}

/*2°:
 *Realizzare una funzione che conti l'altezza di un albero k-ario.
 *Altezza e' massima distanza tra radice e foglia lungo un ramo. Radice esclusa.
 *Una DFS in sostanza.
*/
int height(kTree t) {
    if(t->child == NULL) {
        return 0;
    }
    else {
        int ht = 0;
        kTree c = t->child;
        while(c != NULL) {
            ht = max(ht, height(c));
            c = c->sibling;
        }
        return ht + 1;
    }
}

/*3°:
 *Realizzare una funzione che implementi una visita in ampiezza e che stampi la lista contenente il valore 
 *degli elementi in ordine di lettura.
 *Una BFS.
*/
list BFS(kTree t) {
    if(t == NULL) {
        return NULL;
    }
    else {
        list l = NULL;
        queue q = NewQueue();
        Enqueue(q, t);
        while(!isEmpty(q)) {
            kTree n = DeQueue(q);
            l = Cons(n->info, l);
            kTree c = n->child;
            while(c != NULL) {
                EnQueue(q, c);
                c = c->sibling
            }
        }
        return reverse(l);
    }
}

/*4°:
 *Realizzare la struttura dati heap massimo dato un array non ordinato.
 *Implementare inoltre l'algoritmo di ordinamento HeapSort. 
*/
void Heapify(int[] v, int i) {
    int tmp;
    //max ritorna indice non valore.
    int biggest = max(v[i], max(v[2 * i + 1], v[2 * i + 2]));
    if(biggest != i) {
        tmp = v[i];
        v[i] = v[biggest];
        v[biggest] = tmp;
        Heapify(v, biggest);
    }
}
void BuildHeap(int[] v) {
    for(int i = (lenght(v))/2; i = 1; i--) {
        Heapify(v, i);
    }
}
void HeapSort(int[] v) {
    int tmp;
    BuildHeap(v);
    for(int i = (lenght(v) - 1); i >= 0; i--) {
        tmp = v[0];
        v[0] = v[i];
        v[i] = tmp;
        Heapify(v, 0);
    }
}

/*5°:
 *Scrivere una funzione che dato un array di interi restituisca la lunghezza maggiore di 
 *un sottosegmento dell'array dato ordinato in senso decrescente.
 *Esempio: 1-5-25-17-6-4-12-10 lunghezza = 4 (sottosegmento = 25-17-6-4)
*/
int MaxLen(int[] v) {
    int size = length(v), count = 1, max = 1;
    for(int i = 1; i < size; i++) {
        if(v[i-1] > v[i]) {
            count++;
        }
        else {
            if(count > max) {
                max = count;
            }
            count = 1;
        }
    }
    if(count > max) 
        max = count;
    return max;
}

/*6°:
 *Scrivere una funzione che calcoli il grado di un albero k-ario.
 *Con "grado" si intende il massimo tra il numero di figli di ogni nodo.
 *(BFS modificata in modo da tenere conto del max dei figli).
*/
int Rank(kTree t) {
    if(t == NULL) {
        return 0;
    }
    else {
        int count = 0;
        int max = 0;
        queue q = NewQueue();
        EnQueue(q, t);
        while(!isEmpty(q)) {
            kTree n = DeQueue(q);
            kTree c = n->child;
            count = 0;
            while(c != NULL) {
                count++;
                EnQueue(q, c);
                c = c->sibling;
            }
            if(count > max)
                max = count;
        }
        return max;
    }
}

/*7°:
 *Scrivere una funzione che generi e ritorni un albero k-ario avente i seguenti dati:
 *Come altezza il valore 'ht', come grado il valore 'rk' e come chiave il valore 'key'. 
 *Ogni nodo dell'albero ha chiave identica a ogni altro nodo.
 *Ogni nodo viene rappresentato con una struct 'node' contenente i soliti attributi.
*/
kTree Gen(int ht, int rk, int key) {
    kTree t = malloc(sizeof(node)), tmp;
    t->info = key;
    t->sibling = NULL;
    if(rk == 0 || ht == 0) {
        t->child == NULL;
        return t;
    }
    else {
        t->child = Gen(ht - 1, rk, key);
        tmp = t->child;
        tmp->sibling = Gen(0, rk - 1; key);
        return t;
    }
}

/*8°:
 *Realizzare una funzione che esegua l'operazione di insert in un min heap.
 *Inserimento sempre in ultima posizione e poi risalgo confrontandocon parent e se minore viene scambiato.
 *Heap rappresentato come array.
*/
void Insert(int[] h, int x) {
    lenght(h)++;
    int i = lenght(h), tmp;
    h[i] = x;
    while(i > 0 && h[i/2] > h[i]) {
        tmp = h[i];
        h[i] = h[i/2];
        h[i/2] = tmp;
        i = i/2;
    }
}

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
bool isOrdered(btree bt) {
    if(bt == NULL) {
        return true;
    }
    else {
        Triple t = isOrderedBTreeAux(bt);
        return t.isOrdered;
    }
}
Triple isOrderedBTreeAux(btree bt) {
    Triple t;
    if(bt->right == NULL && bt->left == NULL) {
        t.isOrdered = true;
        t.min = bt->key;
        t.max = bt->key;
    }
    else if(bt->right == NULL) {
        Triple tleft = isOrderedBTreeAux(bt->left);
        t.isOrdered = tleft.isOrdered && bt->key > tleft.max;
        t.min = tleft.min;
        t.max = bt->key;
    }
    else if(bt->left == NULL) {
        Triple tright = isOrderedBTreeAux(bt->right);
        t.isOrdered = tright.isOrdered && bt->key < tright.min;
        t.min = bt->key;
        t.max = tright.max;
    }
    else {
        Triple tleft = isOrderedBTreeAux(bt->left);
        Triple tright = isOrderedBTreeAux(bt->right);
        t.isOrdered = tleft.isOrdered && tright.isOrdered &&
                      bt->key > tleft.max && bt->key < tright.min;
        t.min = tleft.min;
        t.max = tright.max;
    }
    return t;
}