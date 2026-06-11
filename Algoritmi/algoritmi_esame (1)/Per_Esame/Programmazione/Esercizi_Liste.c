/*Esercizi_Liste.c*/

/*1°:
 *Scrivere funzione che ritorni il rango di ogni elemento di una lista, ovvero ogni
 *elemento della lista viene modificato in modo che sia la somma dell'elemento 
 *originario con ogni elemento che segue.
 *Esempio: [1, 2, 3] diventa [6, 5, 3]
*/
    int Rank(list l) {
        if(l == NULL) {
            return 0;
        }
        else{
            l->info = l->info + Rank(l->next);
            return l->info;
        }
    }

/*2°:
 *Scrivere la funzione Reverse che data una lista ne restituisca la lista inversa.
 *Esempio: [1, 2, 3] diventa [3, 2, 1]
*/
    list Reverse(list l) {
        if(l == NULL || l->next == NULL) {
            return l;
        }
        else{
            list r = Reverse(l->next);
            l->next->next = l;
            l->next = NULL;
            return r;
        }
    }

/*3°:
 *Scrivere la funzione Palindrome che data una lista stabilisca se questa e' 
 *palindroma o meno.
 *Suggerimento: usare Reverse dell'esercizio precedente. 
 *Si possono usare funzioni ausiliarie.
 *Esempio: [1, 2, 25, 36, 25, 2, 1] e' palindroma 
 *         [1, 2, 25, 40, 36, 2, 1] non lo e'.
*/
    bool Palindrome(list l) {
        if(l == NULL) {
            return true;
        }
        else{
            list r = Reverse(l);
            return Equqal(l, r);
        }
    }
    bool Equal(list l, list r) {
        if(l == NULL){
            return r == NULL;
        }
        else if(r == NULL) {
            return l == NULL;
        }
        else {
            return l->info == r->info && Equal(l->next, r->next);
        }
    }

/*4°:
 *Scrivere una funzione che, data una lista, la riordini in modo da avere tutti
 *i dispari prima di tutti i pari.
 *L'ordine dei dispari e dei pari tra loro deve rimanere invariato.
 *La funzione deve ritornare una nuova lista.
 *preimplementate le funzioni Cons(int x, list l) e Concat(list l, list m).
 *Esempio: [1, 4, 5, 6, 12, 25, 3, 7, 10] diventa [1, 5, 25, 17, 4, 6, 12, 10].
*/
    list OddFirst(list l) {
        return Concat(Odd_aux(l), Even_aux(l));
    }
    list Odd_aux(list l) {
        if(l == NULL) {
            return l;
        }
        else{
            if(l->info % 2 == 0) {
                return Odd_aux(l->next);
            }
            else{
                return Cons(l->info, Odd_aux(l->next));
            }
        }
    }
    list Even_aux(list l) {
        if(l == NULL) {
            return l;
        }
        else{
            if(l->info % 2 != 0) {
                return Odd_aux(l->next);
            }
            else{
                return Cons(l->info, Even_aux(l->next));
            }
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
        if(l == NULL || r == NULL) {
            return NULL;
        }
        else {
            if(l->info = r->info) {
                return Cons(l->info, Intersection(l->next, r->next))
            }
            else {
                if(l->info < r->info) {
                    return Intersection(l->next, r);
                }
                else {
                    return Intersection(l, r->next);
                }
            }
        }
    }
    list Union(list l, list r) {
        if(l == NULL) {
            return r;
        }
        else if(r == NULL) {
            return l;
        }
        else {
            if(l->info == r->info) {
                return Cons(l->info, Union(l->next, r->next));
            }
            else {
                if(l->info < r->info) {
                    return Cons(l->info, Union(l->next, r);
                }
                else {
                    return Cons(r->info, Union(l, r->next);
                }
            }
        }
    }
    list Difference(list l, list r) {
        if(l == NULL) {
            return NULL;
        }
        else if(r == NULL) {
            return l;
        }
        else {
            if(l->info == r->info) {
                return Difference(l->next, r->next);
            }
            else {
                if(l->info < r->info) {
                    return Cons(l->info, Difference(l->next, r));
                }
                else {
                    return Difference(l, r->next);
                }
            }
        }
    }
    list SymDifference(list l, list r) {
        if(l == NULL) {
            return r;
        }
        else if(r == NULL) {
            return l;
        }
        else {
            if(l->info == r->info) {
                return SymDifference(l->next, r->next);
            }
            else {
                if(l->info < r->info) {
                    return(Cons(l->info, SymDifference(l->next, r)));
                }
                else {
                    return Cons(r->info, SymDifference(l, r->next));
                }
            }
        }
    }

/*6°:
 *Scrivere funzione che dati due parametri, una lista e un intero, cancella tutte
 *le occorrenze di quell'intero e ritorna la lista.
*/
    list DeleteAll(int x, list l) {
        if(l == NULL) {
            return NULL;
        }
        else {
            if(l->info == x) {
                return DeleteAll(n, l->next);
            }
            else{
                l->next = DeleteAll(n, l->next);
                return l;
            }
        }
    }

/*7°:
 *Scrivere funzione che inserisce lista l dentro r dopo n elementi di r seguita
 *dai rimanenti elementi di r. 
 *Usare funzione preimplementata Concat.
*/
    list Insert(list l, list r, int n) {
        if(n == 0) {
            return Concat(l, r);
        }
        else {
            r->next = Insert(l, r-Lnext, n - 1);
            return r;
        }
    }