/*Esercizi_Alberi_Binari.c*/

/*1°:
 *Scrivere le seguenti funzioni: 
 *      (1)Ritornare puntatore al nodo con chiave minima.
 *      (2)Ritornare l'avo destro piu' prossimo a nodo corrente.
 *      (3)Ritornare puntatore al successore se esiste.
 *(Con 'successore' si intende il minimo dei maggioranti del nodo in questione).
*/
/*(1):*/
    btree MinInBtree(btree bt) { 
        if(bt->left == NULL) {
            return bt;
        }
        else {
            return MinInBtree(bt->left);
        }
    }
/*(2):*/
    btree RightAncestor(btree bt) {
        if(bt->parent == NULL) {
            return NULL;
        }
        else if(bt = bt->parent->left) {
            return bt->parent;
        }
        else {
            return RightAncestor(bt->parent);
        }
    }
/*(3):*/
    btree Successor(btree bt) {
        if(bt->right == NULL) {
            return RightAncestor(bt);
        }
        else {
            return MinInBtree(bt->right);
        }
    }

/*2°:
 *Scrivere una funzione che ritorna la lista delle chiavi di bt in ordine 
 *decrescente.
 *Albero binario può essere vuoto.
 *Suggerimento: scrivere funzione ausiliaria che ritorna la lista delle chiavi
 *concatenata con l.
*/
    list DecList(btree bt) {
        return DecList_Aux(bt, NULL);
    }
    list DecList_Aux(btree bt, list l) {
        if(bt == NULL) {
            return l;
        }
        else {
            l = DecList_Aux(bt->left, l);
            l = Cons(bt->info, l);
            return DecList_Aux(bt->right, l);
        }
    }

/*3°:
 *Scrivere una funzione che ritorna TRUE se bt e' di ricerca.
 *Suggerimento: usare tipo 'Triple' preimplementato e scrivere funzione ausiliaria 
 *che ritorni la tripla (min, isOrdered, max) dove isOrdered e' un boolean, 
 *max e min si capisce.
*/
    bool IsOrdered(btree bt) {
        if(bt == NULL) {
            return true;
        }
        else {
            Triple t = IsOrdered_Aux(bt);
            return t.isOrdered;
        }
    }
    bool IsOrdered_Aux(btree bt) {
        Triple t;
        if(bt->left == NULL && bt->right == NULL) {
            t.isOrdered = true;
            t.min = bt->info;
            t.max = bt->info;
        }
        else if(bt->right == NULL) {
            Triple tleft = IsOrdered_Aux(bt->left);
            t.isOrdered = tleft.isOrdered && bt->info > tleft.max;
            t.min = tleft.min;
            t.max = bt->info;
        }
        else if(bt->left == NULL) {
            Triple tright = IsOrdered_Aux(bt->right);
            t.isOrdered = tright.isOrdered && bt->info < tright.min;
            t.min = bt->info;
            t.max = tright.max;
        }
        else {
            Triple tleft = IsOrdered_Aux(bt->left);
            Triple tright = IsOrdered_Aux(bt->right);
            t.isOrdered = tleft.isOrdered && tright.isOrdered &&
                        bt->info > tleft.max && bt->info < tright.min;
            t.min = tleft.min;
            t.max = tright.max;
        }
        return t;
    }

/*4°:
 *Scrivere una funzione che ritorni l'antenato comune ad 'a' e 'b' più prossimo.
 *'a' e 'b' sono chiavi intere, con a < b.
*/
    btree AntenatoComune(btree bt, int a, int b) {
        if(bt->info >= a && bt->info <= b) {
            return bt;
        }
        else if(bt->info < a) {
            return AntenatoComune(bt->right, a, b);
        }
        else if(bt->info > b) {
            return AntenatoComune(bt->left, a, b);
        }
    }

/*5°:
*Scrivere funzione Insert(int k, btree bt) che ritorna l'albero ordinato dopo aver 
*inserito chiave k.
*bt puo' essere vuoto.
*/
    btree Insert(int k, btree bt) {
        if(bt == NULL) {
            bt = NewNode();
            bt->info = k;
            bt->left = NULL;
            bt->right = NULL;
            return bt;
        }
        else if(bt->info == k) {
            return bt;
        }
        else {
            if(k < bt->info) {
                bt->left = Insert(k, bt->left);
                return bt;
            }
            else {
                bt->right = Insert(k, bt->right);
                return bt;
            }
        }
    }