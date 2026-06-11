/*Esercizi_Alberi_K-ari.c*/

/*1°:
 *Scrivere una funzione che calcoli l'altezza di un albero k-ario, ovvero il 
 *massimo delle lunghezze dei rami (se solo radice non ho rami quindi e' 0).
 *L'albero e' non vuoto.
*/
    int Height(kTree t) {
        if(t->child == NULL) {
            return 0;
        }
        else {
            kTree c = t->child;
            int ht = 0;
            while(c != NULL) {
                ht = max(ht, Height(c));
                c = c->sibling;
            }
            return ht + 1;
        }
    }

/*2°:
 *Scrivere una funzione che calcoli la somma delle etichette sulle foglie.
 *Le etichette sono intere e l'albero puo' essere vuoto.
*/
    int SumLeaf(kTree t) {
        if(t == NULL) {
            return 0;
        }
        else if(t->child == NULL) {
            return t->info;
        }
        else{
            ktree c = t->child;
            int res = 0;
            while(c != NULL) {
                res = res + SumLeaf(c);
                c = c->sibling;
            }
            return res;
        }
    }

/*3°:
 *Scrivere una funzione che ritorna la lista delle etichette in ordine di visita 
 *BFS dell' albero.
 *Albero puo' essere vuoto.
 *Suggerimento: aggiungere progressivamente etichette con Cons() e poi restituire 
 *inversa della lista con Reverse().
 *(DFS si fa uguale ma con stack al posto della queue, la lista rimane uguale).
*/
    list kTreeBFS(ktree t) {
        if(t == NULL) {
            return NULL;
        }
        else {
            queue q = NewQueue();
            list l = NULL;
            EnQueue(t, q);
            while(!isEmptyQueue(q)) {
                kTree n = DeQueue(q);
                l = Cons(n->info, l);
                ktree c = n->child;
                while(c != NULL) {
                    EnQueue(c, q);
                    c = c->sibling;
                }
            }
            return Reverse(l);
        }
    }

/*4°:
 *Scrivere una funzione che ritorni true se l'etichetta di ogni nodo (con almeno 
 *un figlio) e' uguale alla somma delle etichette dei suoi figli, false altrimenti.
 *Albero non vuoto.
*/
    bool Sum(ktree t) {
        if(t->child == NULL) {
            return true;
        }
        else{
            kTree c = t->child;
            res = 0;
            bool b = true;
            while(c != NULL) {
                res = res + c->info;
                b = b && Sum(c);
                c = c->sibling;
            }
            return t->info == res && b;
        }
    }

/*5°:
 *Scrivere una funzione che aggiunga ad ogni foglia un nuovo ramo contenente la 
 *somma di tutte le etichette presenti lungo il cammino.
 *Albero non vuoto.
 *Suggerimento: Creare funzione ausiliaria che accumuli in una variabile la 
 *              somma fino a quel momento.
*/
    void SommaRamo(ktree t) {
        SommaCammino(t, 0);
    }
    void SommaCammino(kTree t, int s) {
        if(t->child == NULL) {
            t->child = NewNode(n);
            n->info = s + t->info;
            n->child = NULL;
            n->sibling = NULL;
        }
        else {
            s = s + t->info;
            c = t->child;
            while(c != NULL) {
                SommaCammino(c, s);
                c = c->sibling;
            }
        }
    }

/*6°:
 *Scrivere una funzione 'NodiProfondi(kTree t, int h)' che restituisca il numero 
 *di nodi dell'albero che si trovano a livello <=h.
 *albero non vuoto.
 *Non si possono usare funzioni ausiliarie. 
*/
    int NodiProfondi(ktree t, int h) {
        if(h == 0) {
            return 1;
        }
        else {
            int n = 1;
            ktree c = t->child;
            while(c != NULL) {
                n = n + NodiProfondi(c, h - 1);
                c = c->sibling;
            }
            return n;
        }
    }

/*7°:
 *Scrivere una funzione che ritorni il massimo delle somme delle etichette sullo 
 *stesso ramo.
 *Ovvero prima devo fare la somma di tutte le etichette di ogni ramo e poi devo 
 *ritornare il max di tali somme.
*/
    int MaxSumBranch(kTree t) {
        if(t == NULL) {
            return 0;
        }
        else if(t->child == 0) {
            return t->info;
        }
        else {
            int maxSum = MaxSumBranch(t->child);
            kTree c = t->child->sibling;
            while(c != NULL) {
                maxSum = max(maxSum, MaxSumBranch(c));
                c = c->sibling;
            }
            return t->info + maxSum;
        }
    }

/*8°:
 *Scrivere una funzione che ritorni il numero di nodi presenti lungo il 
 *ramo più corto.
 *Ramo è cammino da radice a foglia.
 *Albero k-ario non vuoto.
 *(Una DFS).
*/
    int Shortest(kTree t) {
        if(t->child == NULL) {
            return 1;
        }
        else{
            int res = Shortest(t->child);
            kTree c = t->child;
            while(c != NULL) {
                res = min(res, Shortest(c));
                c = c->sibling;
            } 
            return res + 1;
        }
    }

/*9°:
 *Scrivere una funzione che calcoli il grado di un albero 
 *potenzialmente anche vuoto.
 *Con 'grado' si intende il massimo tra il numero di figli tra tutti i nodi.
 *(BFS modificata per tenere conto del max num di figli).
*/
    int Rank(ktree t) {
        if(t == NULL) {
            return 0;
        }
        else if(t->child == NULL) {
            return 1;
        }
        else {
            int count = 0;
            int max = 0;
            queue q = NewQueue();
            EnQueue(q, t);
            while(!isEmptyQueue(q)) {
                ktree n = DeQueue(q);
                kTree c = n->child;
                count = 0;
                while(c != NULL) {
                    count++;
                    EnQueue(q, c);
                    c = c->sibling;
                }
                if(count > max) {
                    max = count;
                }
            }
            return max;
        }
    }

/*10°:
 *Scrivere una funzione che generi e ritorni un albero avente i seguenti dati:
 *altezza il valore 'ht', grado il valore 'rk' e chiave il valore 'key'.
 *Ogni nodo ha chiave identica a ogni altro nodo e vale 'key'.
 *Ogni nodo viene rappresentato con una struct 'node' composta dai campi usuali.
*/
    kTree Generator(int ht, int rk, int key) {
        kTree t = malloc(sizeof(node));
        t->info = key;
        t->sibling = NULL;
        if(ht == 0) {
            t->child = NULL;
            return t;
        }
        else {
            t->child = Generator(ht - 1, rk, key);
            while(rk > 1) {
                t->child->sibling = Generator(0, rk, key);
                rk--;
            }
            return t;
        }
    }