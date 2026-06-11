/************************************
* Struttura dati degli alberi k-ari
************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define SEPARATOR "#<ab@17943918#@>#"

#include "pile.h"
#include "code.h"

struct kTreeVertex {
    int                  key;
    struct kTreeVertex*  child;
    struct kTreeVertex*  sibling;
};

typedef struct kTreeVertex* kTree;

kTree consTree(int k, kTree c, kTree s) {
    kTree t = malloc(sizeof(struct kTreeVertex));
    t->key = k;
    t->child = c;
    t->sibling = s;
    return t;
}

kTree leaf(int k, kTree s) {
    return consTree(k, NULL, s);
}

kTree root(int k, kTree c) {
    return consTree(k, c, NULL);
}

// post: stampa indentata dell'albero t con margine
//       iniziale di n tab
void printTree(kTree t, int d) {
    if (t == NULL) return;
    
    for (int i = 0; i < d; ++i)
        printf("   ");
    printf("%d\n", t->key);
    kTree cl = t->child;
    while (cl != NULL) {
        printTree(cl, d + 1);
        cl = cl->sibling;
    }
}

int cardinalita_k(kTree t){
    if(!t) return 0;

    int card=1;
    kTree C = t->child;
    while(C){
        card = card + cardinalita_k(C);
        C = C->sibling;
    }

    return card;
}

int cardinalità_k_equival(kTree t){
    if(!t) return 0;

    int c = cardinalità_k_equival(t->child);
    int s = cardinalità_k_equival(t->sibling);
    return c+s+1;
}

int altezza_k(kTree t){
    if(!t->child) return 0;

    int h=0;
    kTree C = t->child;
    while(C){
        int alt_C = altezza_k(C);
        h = (h > alt_C ? h : alt_C);
        C = C->sibling;
    }

    return h+1;
}

void dfs_k(kTree T){
    if(!T) return;

    printf("%d ", T->key);
    kTree currC = T->child;
    while(currC){
        dfs_k(currC);
        currC = currC->sibling;
    }
}

void bfs_k(kTree T){
    if(!T) return;

    queue q = NewQueue();
    EnQueue(T, q);
    while(!isEmptyQueue(q)){
        kTree curr = (kTree)DeQueue(q);

        printf("%d ", curr->key);

        kTree C = curr->child;
        while(C){
            EnQueue(C, q);
            C = C->sibling;
        }
    } 
}

/*1°:
 *Scrivere una funzione che calcoli l'altezza di un albero k-ario, ovvero il 
 *massimo delle lunghezze dei rami (se solo radice non ho rami quindi e' 0).
 *L'albero e' non vuoto.
*/
int Height(kTree t) {
    if(!t) return -1;
    if(!t->child) return 0;

    int h = 0;
    kTree c = t->child;
    while(c){
        int hc = Height(c);
        c = c->sibling;

        if(h < hc) h = hc;
    }
    return h + 1;
    
}

/*2°:
 *Scrivere una funzione che calcoli la somma delle etichette sulle foglie.
 *Le etichette sono intere e l'albero puo' essere vuoto.
*/
int SumLeaf(kTree t) {
    if(!t) return 0;
    if(!t->child) {return t->key;}

    int suml = 0;
    kTree c = t->child;
    while (c){
        suml += SumLeaf(c);
        c = c->sibling;
    }

    return suml;
}

/*3°:
 *Scrivere una funzione che ritorna la lista delle etichette in ordine di visita 
 *BFS dell' albero.
 *Albero puo' essere vuoto.
 *Suggerimento: aggiungere progressivamente etichette con Cons() e poi restituire 
 *inversa della lista con Reverse().
 *(DFS si fa uguale ma con stack al posto della queue, la lista rimane uguale).
*/
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

void printlist (list l) {
    while (l != NULL) {
        printf("%d ", l->info);
        l = l->next;
    }
    printf("\n");
}

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

list kTreeBFSRec(kTree t, list l){
    if(!t) return NULL;

    queue q = NewQueue();
    EnQueue(t, q);
    while(!isEmptyQueue(q)){
        kTree curr = (kTree)DeQueue(q);
        l = Cons(curr->key, l);

        kTree c = curr->child;
        while(c){
            EnQueue(c, q);
            c = c->sibling;
        }
    }
    
    return Reverse(l);
}

list kTreeBFS(kTree t) {
    if(!t) return NULL;
    
    return kTreeBFSRec(t, NULL);
}

/*4°:
 *Scrivere una funzione che ritorni true se l'etichetta di ogni nodo (con almeno 
 *un figlio) e' uguale alla somma delle etichette dei suoi figli, false altrimenti.
 *Albero non vuoto.
*/
bool Sum(kTree t) {
    if(!t || !t->child) return true;

    int s = 0;
    bool ris = true;
    kTree c = t->child;
    while (c){
        s += c->key;
        ris = ris && Sum(c);
        c = c->sibling;
    }

    return ris && s == t->key;
}

/*5°:
 *Scrivere una funzione che aggiunga ad ogni foglia un nuovo ramo contenente la 
 *somma di tutte le etichette presenti lungo il cammino.
 *Albero non vuoto.
 *Suggerimento: Creare funzione ausiliaria che accumuli in una variabile la 
 *              somma fino a quel momento.
*/
void SommaRamoAux(kTree t, int acc){
    if(!t->child){
        t->child = leaf(acc + t->key, NULL);
        return;
    }

    kTree c = t->child;
    while (c){
        SommaRamoAux(c, acc + t->key);
        c = c->sibling;
    }
    
}

void SommaRamo(kTree t) {
    if(!t) return;

    SommaRamoAux(t, 0);
}

/*6°:
 *Scrivere una funzione 'NodiProfondi(kTree t, int h)' che restituisca il numero 
 *di nodi dell'albero che si trovano a livello <=h.
 *albero non vuoto.
 *Non si possono usare funzioni ausiliarie. 
*/
int NodiProfondi(kTree t, int h) {
    if(!t || h < 0) return 0;
    
    // Conta il nodo corrente (è a profondità 0 rispetto a se stesso)
    int s = 1;
    
    // Se h > 0, conta i figli a profondità h-1
    if(h > 0) {
        kTree c = t->child;
        while(c) {
            s += NodiProfondi(c, h-1);
            c = c->sibling;
        }
    }
    return s;  
}

/*7°:
 *Scrivere una funzione che ritorni il massimo delle somme delle etichette sullo 
 *stesso ramo.
 *Ovvero prima devo fare la somma di tutte le etichette di ogni ramo e poi devo 
 *ritornare il max di tali somme.
*/
int MaxSumBranchAux(kTree t, int acc) {
    if(!t) return -1; // se albero vuoto
    
    // Se è una foglia, ritorna la somma del ramo
    if(!t->child) {
        return acc + t->key;
    }
    
    // Se non è foglia, trova il massimo tra i figli
    int max = -1;
    kTree c = t->child;
    while(c) {
        int sum = MaxSumBranchAux(c, acc + t->key);
        if(sum > max) max = sum;
        c = c->sibling;
    }
    
    return max;
}

/*8°:
 *Scrivere una funzione che ritorni il numero di nodi presenti lungo il 
 *ramo più corto.
 *Ramo è cammino da radice a foglia.
 *Albero k-ario non vuoto.
 *(Una DFS).
*/
int ShortestAux(kTree t, int acc) {
    if(!t) return 999999;
    if(!t->child) return acc + 1; // conta anche il nodo foglia

    int minLen = 999999;
    kTree c = t->child;
    while(c) {
        int len = ShortestAux(c, acc + 1);
        if(len < minLen) minLen = len;
        c = c->sibling;
    }

    return minLen;
}

int Shortest(kTree t) {
    if(!t) return 0;

    return ShortestAux(t, 0);
}

int MaxSumBranch(kTree t) {
    if(!t) return 0;
    return MaxSumBranchAux(t, 0);
}

/*9°:
 *Scrivere una funzione che calcoli il grado di un albero 
 *potenzialmente anche vuoto.
 *Con 'grado' si intende il massimo tra il numero di figli tra tutti i nodi.
 *(BFS modificata per tenere conto del max num di figli).
*/
int Rank(kTree t) {
    if(!t) return 0;

    int r = 0;
    queue q = NewQueue();
    EnQueue(t, q);
    while (!isEmptyQueue(q)){
        kTree curr = (kTree)DeQueue(q);
        
        kTree c = curr->child;
        int rc = 0;
        while (c){
            EnQueue(c, q);
            rc++;
            c = c->sibling;
        }
        if(r < rc) r = rc;
    }
    
    
    return r;
}

int main() {

// test 1 - albero originale
    kTree t =
       root(12, 
            consTree(22, 
                leaf(1,NULL), 
                leaf(2, 
                    root(32, 
                        leaf(3, 
                            leaf(4, NULL)
                        )
                    )
                )
            )
       );

    

/* t in forma indentata:
12
	22
		1
	2
	32
		3
		4
*/

    printf("Albero dato:\n");
    printTree(t, 0);

    /*printf("Altezza: %d\n", Height(t));
    printf("Somma foglie: %d\n", SumLeaf(t));

    list dfs = kTreeBFS(t);
    printlist(dfs);

    printf("Sum: %d\n\n", Sum(t));*/

    // test 2 - albero dove Sum ritorna true
    // Struttura:
    //      15
    //     /  \
    //   10    5  (siblings)
    //   / \
    //  6   4  (siblings)
    // Dove: 10 = 6 + 4 e 15 = 10 + 5
    kTree figli_10 = leaf(6, leaf(4, NULL));  // 6 e 4 come siblings
    kTree t_sum = root(15, consTree(10, figli_10, leaf(5, NULL)));  // 15 con 10 e 5 come figli
    
    //printf("=== Albero con Sum = true ===\n");
    printTree(t_sum, 0);
    /*printf("Sum: %d\n", Sum(t_sum));

    SommaRamo(t);
    printTree(t, 0);

    printf("Nodi prondi di %d: %d \n", 1, NodiProfondi(t, 1));
    printf("Nodi prondi di %d: %d \n", 0, NodiProfondi(t, 0));
    printf("Nodi prondi di %d: %d \n", 2, NodiProfondi(t, 2));
    
    printf("\nMax somma ramo: %d\n", MaxSumBranch(t));*/

    printf("Rank albero: %d\n", Rank(t));
    printf("Rank albero: %d\n", Rank(t_sum));

}
