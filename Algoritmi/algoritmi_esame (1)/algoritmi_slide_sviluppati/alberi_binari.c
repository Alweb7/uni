/************************************
*  Struttura per gli alberi binari
*  (senza puntatore al padre)
************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define SEPARATOR "#<ab@17943918#@>#"
#include "code.h"
#include "pile.h"

struct BtreeNd {
    int             key;
    struct BtreeNd* left;
    struct BtreeNd* right;
    struct BtreeNd* parent;
    
};

typedef struct BtreeNd* btree;

btree ConsTree(int k, btree l, btree r, btree p) {
    btree rootnode = malloc(sizeof(struct BtreeNd));
    rootnode->key = k;
    rootnode->left = l;
    rootnode->right = r;
    rootnode->parent = p;
    return rootnode;
}

// post: stampa indentata dell'albero bt in preordine sinistro, con margine
//       iniziale di n tab, senza visualizzare i puntatori a nil
void printtree(btree bt, int n) {
    if (bt != NULL) {
        for (int i = 0; i < n; i++) 
            printf("   ");
        printf("%d\n", bt->key);
        printtree(bt->left, n + 1);
        printtree(bt->right, n + 1);
    }
}

// post: stampa indentata dell'albero bt in preordine sinistro, con margine
//       iniziale di n tab, visualizzando i puntatori a nil
void printtree2(btree bt, int n) {
    for (int i = 0; i < n; i++) 
            printf("   ");
    if (bt == NULL) 
        printf("nil\n");
    else {
        printf("%d\n", bt->key);
        printtree2(bt->left, n + 1);
        printtree2(bt->right, n + 1);
    }
}

int cardinalità_bin(btree t){
    if(!t) return 0;
    int l = cardinalità_bin(t->left);
    int r = cardinalità_bin(t->right);
    return l+r+1;
}

int altezza_bin(btree t){
    if(!t->left && !t->right) return 0;

    int hl = 0;
    int hr = 0;
    if(t->left) hl = altezza_bin(t->left);
    if(t->right) hr = altezza_bin(t->right);

    return 1 + (hl > hr ? hl : hr);

}

void bfs_bin(btree T){
    if(!T) return;

    queue q = NewQueue();
    EnQueue(T, q);
    while(!isEmptyQueue(q)){
        btree curr = DeQueue(q);
        
        printf("%d ", curr->key);

        if(curr->left) EnQueue(curr->left, q);
        if(curr->right) EnQueue(curr->right, q);
    }
}

void dfs_bin(btree T){
    if(!T) return;

    dfs_bin(T->left);
    printf("%d ", T->key);
    dfs_bin(T->right);
}

//O(h) dove h è l'altezza dell'albero
btree ricerca_ric(btree T, int k){
    if(!T) return NULL;

    if(T->key == k) return T;
    else if( k < T->key) return ricerca_ric(T->left, k);
    else return ricerca_ric(T->right, k);
}

btree ricerca_it(btree T, int k){
    btree curr = T;
    while(curr && curr->key != k){
        if(k < curr->key) curr = curr->left;
        else curr = curr->right;
    }

    return curr;
}

btree min(btree T){
    btree curr = T;
    while(curr->left){
        curr = curr->left;
    }
    return curr;
}

btree tree_succ(btree T){
    if(T->right) return min(T->right);
    else{
        btree P = T->parent;
        while(T == P->right && P){
            T = P;
            P = T->parent;
        }
        return P;
    }
}

void tree_insert(btree N, btree *T){
    btree P = NULL;
    btree curr = *T;
    while(curr){
        P = curr;
        if(N->key == curr->key) return; //esiste già
        else if(N->key < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    N->parent = P;
    if(!P) *T = N;
    else if(N->key < P->key) P->left = N;
    else P->right = N;
}

void delete_uno(btree Z, btree *T){
    if(Z == *T){
        if(Z->left) *T = Z->left;
        else *T = Z->right;

        Z->parent = NULL;
    }
    else{
        btree S;
        if(Z->key){
            Z->left->parent = Z->parent;
            S = Z->left;
        }
        else{
            Z->right->parent = Z->parent;
            S = Z->right;
        }
        if(Z->parent->right == Z) Z->parent->right = S;
        else Z->parent->left = S;
    }
}

void tree_delete(btree Z, btree *T){
    if(!Z->left && !Z->right){ //Z è una foglia
        if(Z == *T) *T = NULL;
        else{
            if(Z->parent->left == Z) Z->parent->left = NULL;
            else Z->parent->right = NULL;
        }
    }
    else{
        if(!Z->left || !Z->right) delete_uno(Z, T);
        else{
            btree Y = min(Z->right);
            Z->key = Y->key;
            tree_delete(Y, T);
        }
    }
}

void left_rotate(btree* T, btree x){
    btree y = x->right;
    x->right = y->left;
    if(y->left) y->left->parent = x;

    y->parent = x->parent;
    if(!x->parent) *T = y;
    else{
        if(x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
    }
    
    y->left = x;
    x->parent = y;
}

/*1°:
 *Scrivere le seguenti funzioni: 
 *      (1)Ritornare puntatore al nodo con chiave minima.
 *      (2)Ritornare l'avo destro piu' prossimo a nodo corrente.
 *      (3)Ritornare puntatore al successore se esiste.
 *(Con 'successore' si intende il minimo dei maggioranti del nodo in questione).
*/
/*(1):*/
btree MinInBtree(btree bt) {
    if(!bt) return NULL;
    if(!bt->left) return bt;

    return MinInBtree(bt->left);
}
/*(2):*/
btree RightAncestor(btree bt) {
    if(!bt) return NULL;
    if(!bt->parent) return bt;

    if(bt->parent->left == bt) return bt->parent;
    else return RightAncestor(bt->parent);
}

/*(3):*/
btree Successor(btree bt) {
    if(!bt) return NULL;

    if(bt->right) return MinInBtree(bt->right);
    else return RightAncestor(bt);
}

/*2°:
 *Scrivere una funzione che ritorna la lista delle chiavi di bt in ordine 
 *decrescente.
 *Albero binario può essere vuoto.
 *Suggerimento: scrivere funzione ausiliaria che ritorna la lista delle chiavi
 *concatenata con l.
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

list Concat(list l, list m){
    if(!l) return m;
    l->next = Concat(l->next, m);
    return l;
}

// Inserisce x in ordine decrescente nella lista l
list InsertDec(int x, list l) {
    if(!l || x > l->info) {
        return Cons(x, l);
    }
    l->next = InsertDec(x, l->next);
    return l;
}

list DecListAux(btree bt, list l) {
    if(!bt) return l;
    
    l = DecListAux(bt->left, l);
    l = InsertDec(bt->key, l);
    l = DecListAux(bt->right, l);
    
    return l;
}

list DecList(btree bt) {
    return DecListAux(bt, NULL);
}

/*3°:
 *Scrivere una funzione che ritorna TRUE se bt e' di ricerca.
 *Suggerimento: usare tipo 'Triple' preimplementato e scrivere funzione ausiliaria 
 *che ritorni la tripla (min, isOrdered, max) dove isOrdered e' un boolean, 
 *max e min si capisce.
*/
btree MaxInBtree(btree bt) {
    if(!bt) return NULL;
    if(!bt->right) return bt;

    return MaxInBtree(bt->right);
}

bool IsOrdered(btree bt) {
    if(!bt) return true;

    // Controlla che il sottoalbero sinistro sia ordinato
    if(bt->left) {
        btree maxl = MaxInBtree(bt->left);
        if(maxl->key >= bt->key) return false;
        if(!IsOrdered(bt->left)) return false;
    }
    
    // Controlla che il sottoalbero destro sia ordinato
    if(bt->right) {
        btree minr = MinInBtree(bt->right);
        if(minr->key <= bt->key) return false;
        if(!IsOrdered(bt->right)) return false;
    }

    return true;
}

/*4°:
 *Scrivere una funzione che ritorni l'antenato comune ad 'a' e 'b' più prossimo.
 *'a' e 'b' sono chiavi intere, con a < b.
*/
btree AntenatoComune(btree bt, int a, int b) {
    if(!bt) return NULL;
    
    if(a < bt->key && b < bt->key) {
        return AntenatoComune(bt->left, a, b);
    }
    else if(a > bt->key && b > bt->key) {
        return AntenatoComune(bt->right, a, b);
    }
    
    return bt;
}

/*5°:
*Scrivere funzione Insert(int k, btree bt) che ritorna l'albero ordinato dopo aver 
*inserito chiave k.
*bt puo' essere vuoto.
*/
btree Insert(int k, btree bt) {
    if(!bt){
        btree newN = malloc(sizeof(struct BtreeNd));
        newN->key = k;
        newN->left = newN->right = newN->parent = NULL;
        return newN;
    }

    if(k == bt->key) return bt;
    if(k < bt->key) {
        bt->left = Insert(k, bt->left);
        if(bt->left) bt->left->parent = bt;
    }
    else {
        bt->right = Insert(k, bt->right);
        if(bt->right) bt->right->parent = bt;
    }
    
    return bt;
}

int main()
{

    btree bt_left_left = ConsTree(9, NULL, NULL, NULL);
    btree bt_left_right = ConsTree(1, NULL, NULL, NULL);
    btree bt_left = ConsTree(35, bt_left_left, bt_left_right, NULL);
    bt_left_left->parent = bt_left;
    bt_left_right->parent = bt_left;
    
    btree bt_right_right = ConsTree(3, NULL, NULL, NULL);
    btree bt_right = ConsTree(5, NULL, bt_right_right, NULL);
    bt_right_right->parent = bt_right;
    
    btree bt = ConsTree(20, bt_left, bt_right, NULL);
    bt_left->parent = bt;
    bt_right->parent = bt;

    /*printf("Albero dato (senza nil):\n");
    printtree(bt, 0);
    printf("Albero dato (incluso nil):\n");
    printtree2(bt, 0);*/

    //list ris = DecList(bt);
    //printlist(ris);

    //printf("ordinato: %d\n\n", IsOrdered(bt));

    // Creazione di un albero ordinato per test
    //printf("=== Albero ordinato ===\n");
    btree ord_left_left = ConsTree(3, NULL, NULL, NULL);
    btree ord_left_right = ConsTree(7, NULL, NULL, NULL);
    btree ord_left = ConsTree(5, ord_left_left, ord_left_right, NULL);
    ord_left_left->parent = ord_left;
    ord_left_right->parent = ord_left;
    
    btree ord_right_left = ConsTree(12, NULL, NULL, NULL);
    btree ord_right_right = ConsTree(20, NULL, NULL, NULL);
    btree ord_right = ConsTree(15, ord_right_left, ord_right_right, NULL);
    ord_right_left->parent = ord_right;
    ord_right_right->parent = ord_right;
    
    btree bt_ord = ConsTree(10, ord_left, ord_right, NULL);
    ord_left->parent = bt_ord;
    ord_right->parent = bt_ord;

    /*printf("Albero ordinato (senza nil):\n");
    printtree(bt_ord, 0);
    printf("\nAlbero ordinato: %d\n", IsOrdered(bt_ord));*/

    /*btree ant = AntenatoComune(bt_ord, 12, 20);
    printf("ant com: %d \n", ant->key);
    btree ant1 = AntenatoComune(bt_ord, 12, 5);
    printf("ant com: %d", ant1->key);*/

    btree con4 = Insert(4, bt_ord);
    printtree(bt_ord, 0);
    btree con5 = Insert(5, bt_ord);
    printtree(bt_ord, 0);

    return 0;
}