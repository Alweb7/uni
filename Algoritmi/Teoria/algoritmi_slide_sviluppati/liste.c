/*****************************************
*  Struttura dati per le liste semplici
*  di interi
******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define SEPARATOR "#<ab@17943918#@>#"

struct listEl {
    int            info;
    struct listEl* next;
};

typedef struct listEl* list;

list Cons(int x, list xs) {
    list newlist = malloc(sizeof(struct listEl));
    newlist->info = x;
    newlist->next = xs;
    return newlist;
}

struct listEl2 {
    int            info;
    struct listEl2* pred; // puntatore al predecessore
    struct listEl2* next; // puntatore al successivo
};

typedef struct listEl2* list2;

// post: inserisce la chiave x in un frame
//       il cui predecessore è p e successore è n
list2 Cons2(int x, list2 p, list2 n) {
    list2 newlist = malloc(sizeof(struct listEl2));
    newlist->info = x;
    newlist->pred = p;
    newlist->next = n;
    return newlist;
}

void printlist (list l) {
    while (l != NULL) {
        printf("%d ", l->info);
        l = l->next;
    }
    printf("\n");
}

void printArray(int A[], int n){
    for(int i = 0; i<n; i++){
        printf("%d ", A[i]);
    }
    printf("\n");
}

//complessità: O(n)
list listSearch(list l, int k){
    if(!l) return NULL;
    
    list x = l;
    while(x != NULL && x->info != k){
        x = x->next;
    }
    return x;
}

//complessità O(1)
void listInsert2(list2* l, list2 x){
    x->next = *l;
    if(*l != NULL){
        (*l)->pred = x;
    }
    *l = x;
    x->pred = NULL;
}

//complessità O(1)
void listDelete2(list2* l, list2 x){
    if(x->pred != NULL){
        x->pred->next = x->next;
    }
    else{
        *l = x->next;
    }
    if(x->next){
        x->next->pred = x->pred;
    }
}

/*1°:
 *Scrivere funzione che ritorni il rango di ogni elemento di una lista, ovvero ogni
 *elemento della lista viene modificato in modo che sia la somma dell'elemento 
 *originario con ogni elemento che segue.
 *Esempio: [1, 2, 3] diventa [6, 5, 3]
*/
int Rank(list l) {
    if(!l) return 0;

    l->info += Rank(l->next);
    return l->info;
}

/*2°:
 *Scrivere la funzione Reverse che data una lista ne restituisca la lista inversa.
 *Esempio: [1, 2, 3] diventa [3, 2, 1]
*/
list Reverse(list l) {
    if (l == NULL) { return NULL; } 
    
    list rest = Reverse(l->next); 

    list tmp = rest; 
    if (tmp == NULL) { 
        return l; // l diventa la nuova testa 
    } 
    
    while (tmp->next != NULL) { 
        tmp = tmp->next; 
    } 
    tmp->next = l; 
    l->next = NULL; 
    return rest;
}

/*3°:
 *Scrivere la funzione Palindrome che data una lista stabilisca se questa e' 
 *palindroma o meno.
 *Suggerimento: usare Reverse dell'esercizio precedente. 
 *Si possono usare funzioni ausiliarie.
 *Esempio: [1, 2, 25, 36, 25, 2, 1] e' palindroma 
 *         [1, 2, 25, 40, 36, 2, 1] non lo e'.
*/
bool Equal(list l, list r) {
    if(!l && !r) return true;
    if(!l || !r) return false;

    if(l->info != r->info) return false;

    return Equal(l->next, r->next);
}

bool Palindrome(list l) {
    if(!l) return true;

    list rev = Reverse(l);
    return Equal(l, rev);
}

/*4°:
 *Scrivere una funzione che, data una lista, la riordini in modo da avere tutti
 *i dispari prima di tutti i pari.
 *L'ordine dei dispari e dei pari tra loro deve rimanere invariato.
 *La funzione deve ritornare una nuova lista.
 *preimplementate le funzioni Cons(int x, list l) e Concat(list l, list m).
 *Esempio: [1, 4, 5, 6, 12, 25, 3, 7, 10] diventa [1, 5, 25, 17, 4, 6, 12, 10].
*/
list Concat(list l, list m){ 
    if(!l) return m; 
    
    l->next = Concat(l->next, m); 
    return l; 
}

list OddFirst(list l) {
    if(!l) return NULL;
    
    list resto = OddFirst(l->next);
    
    if(l->info % 2 != 0) {  // È dispari - metto all'inizio
        return Cons(l->info, resto);
    } else {  // È pari - metto alla fine
        return Concat(resto, Cons(l->info, NULL));
    }
}

/*5°:
 *Supponendo di lavorare con liste di interi ordinate e senza ripetizioni scrivere
 *le seguenti funzioni:
 *Intersection(l, r) che calcola l'intersezione di due liste;
 *Union(l, r) che calcola l'uione di due liste;
 *Difference(l, r) che calcola la differenza (quelli di l ma non di r);
 *SymDifference(l, r) che calcola la differenza simmetrica 
 *                    (unione delle liste tranne loro intersezione).
*/
list Intersection(list l, list r) {
    if(!l || !r) return NULL;

    if(l->info == r->info){
        return Cons(l->info, Intersection(l->next, r->next));
    }
    if(l->info < r->info) return Intersection(l->next, r);
    else return Intersection(l, r->next);
}

list Union(list l, list r) {
    if(!l && !r) return NULL;
    if(!l) return r;
    if(!r) return l;

    list u = NULL;
    if(l->info == r->info){
        u->info = l->info;
        u->next = Intersection(l->next, r->next);
    }
    if(l->info < r->info){
        u->info = l->info;
        u->next = Intersection(l->next, r);
    } 
    else {
        u->info = r->info;
        u->next = Intersection(l, r->next);
    }
    return u;
}

list Difference(list l, list r) {
    if(!l && !r) return NULL;
    if(!l) return NULL;
    if(!r) return l;

    list d = NULL;
    if(l->info == r->info){
        d = Difference(l->next, r->next);
    }
    if(l->info < r->info){
        d->info = l->info;
        d->next = Difference(l->next, r);
    } 
    else {
        d = Difference(l, r->next);
    }
    return d;
}

list SymDifference(list l, list r) {
    if(!l && !r) return NULL;
    if(!l) return r;
    if(!r) return l;

    list s = NULL;
    if(l->info == r->info){
        s = SymDifference(l->next, r->next);
    }
    if(l->info < r->info){
        s->info = l->info;
        s->next = SymDifference(l->next, r);
    } 
    else {
        s->info = r->info;
        s->next = SymDifference(l, r->next);
    }
    return s;
}

/*6°:
 *Scrivere funzione che dati due parametri, una lista e un intero, cancella tutte
 *le occorrenze di quell'intero e ritorna la lista.
*/
list DeleteAll(int x, list l) {
    if(!l) return NULL;

    if(l->info == x){
        return DeleteAll(x, l->next);
    }
    else {
        l->next = DeleteAll(x, l->next);
        return l;
    }
}

/*7°:
 *Scrivere funzione che inserisce lista l dentro r dopo n elementi di r seguita
 *dai rimanenti elementi di r. 
 *Usare funzione preimplementata Concat.
*/
list Insert(list l, list r, int n) {
    if(!l && !r) return NULL;
    if(!l) return r;
    if(!r) return l;
    
    if(n == 0){
        list resto = r;
        return Concat(l, resto);
    }
    else{
        r->next = Insert(l, r->next, n-1);
        return r;
    }
}

int main() {
    int a[5] = {1, 2, 3, 4, 5};
    list l;
    list l1;
    list l2;

    l = Cons(7, Cons(8, Cons(9, NULL)));
    l1 = Cons(1, Cons(2, Cons(1, NULL)));
    l2 = Cons(1, Cons(2, Cons(3, Cons(4, Cons(5, NULL)))));

    //int rank = Rank(l1);
    //printlist(l1);

    /*list rev = Reverse(l);
    printlist(rev);
    rev = Reverse(l2);
    printlist(rev);*/

    //printf("%d \n", Palindrome(l));
    //printf("%d \n", Palindrome(l1));

    //list ris = OddFirst(l2);
    //printlist(ris);

    //list ris = DeleteAll(3, l2);
    //printlist(ris);

    list ris = Insert(l, l2, 2);
    printlist(ris);
    
}