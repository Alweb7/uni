#include <stdio.h>
#include <stdlib.h>



// FUNZIONE INCREMENTO VARIABILI CON ARRAY
/*
int incrementa(int *x){
    (*x)++;

}
*/
int moltiplica(int a,int b){
    int prodotto;
    prodotto=a*b;
    return prodotto;
}



int main(void){
    //PUNTATORI 

    /*
    int a = 5;    
    //Aggiungiamo '&' per prendere l'indirizzo di 'a', non il suo valore
    int *p = &a;   
    int *x= &a;
    // Aggiungiamo '*' per stampare il VALORE a cui 'p' punta (cioè 5)
    printf("stampo %d \n", *p); 
    //incrementa(&a);
    printf("stampo incremento %d", a);
    // Se volevi stampare proprio L'INDIRIZZO di memoria contenuto in p, useresti %p senza asterisco:
    printf("indirizzo: %p \n", p); 
    */

    // PROVA USO MALLOC per un array

    /* 
    int *v = malloc(10 * sizeof(int)); // va usato quando un array non basta a livello di memoria perchè anche se c'è l'array a lunghezza dinamica se l'utente sbaglia ad inserire occupa tutta la memoria dello stack

    if (v == NULL) {
        printf("Errore: memoria esaurita!\n");
        return 1; // Esci con un errore
    }
    v[1]=42;
    printf("stampo %d\n", v[1]);
    printf("stanpo %p", v);
    free(v);
    */


    //PROVA ARRAY E PUNTATORI

    /*
    int v[4] = {10, 20, 30, 40};
    
    // Assegnamo il puntatore all'inizio dell'array
    int *p = v; // Equivalente a scrivere: int *p = &v[0];

    printf("--- METODO 1: L'aritmetica *(p + i) ---\n");
    for (int i = 0; i < 4; i++) {
        // Al posto di v[i], diciamo: "parti dall'indirizzo p, 
        // fai 'i' passi in avanti, e leggi il valore"
        printf("Posizione %d: %d\n", i, *(p + i));
    }

    printf("\n--- METODO 2: Far camminare il puntatore ---\n");
    // Questo è il metodo usato dai programmatori C più esperti
    int *p2 = v;
    int *fine= v+4;
    for (int i = 0; i < 4; i++) {
        printf("Valore attuale: %d\n", *p2);
        p2++; // Sposta fisicamente il puntatore alla cella successiva!
    }
    // "Finché non raggiungi il confine, stampa e avanza"
    while (p < fine) {
        printf("%d\n", *p);
        p++; 
    }
    */
   
    /*
    //   PUNTATORE ALLA FUNZIONE
    // 2. Dichiaro il puntatore a funzione.
    // Deve avere lo stesso tipo di ritorno (int) e gli stessi parametri (int, int)
    int (*operazione)(int, int);

    // 3. Assegno l'indirizzo della funzione al puntatore.
    // Basta usare il nome della funzione senza le parentesi ()
    operazione = moltiplica; 
    
    // (Nota: scrivere operazione = &moltiplica; è identico, il C lo capisce lo stesso)

    // 4. Chiamo la funzione USANDO IL PUNTATORE
        
    printf("Il risultato e': %d\n", operazione(5,4));
    */

    return 0;
}


