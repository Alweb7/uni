#include <stdio.h>
#include <stdlib.h>
#include "pile.h"

stack *createStack() {
    stack* s = malloc(sizeof(struct Stack));
    s->top = NULL;
    s->size = 0; // Inizialmente la pila è vuota
    return s;
}

//tutte operazioni complessità temporale O(1)
//complessità temporale O(n)
void push(stack* s, int value) {
    node* newNode = malloc(sizeof(struct Node));
    if (newNode == NULL) return; // Errore di allocazione
    newNode->data = value;
    
    s->size++; // Incrementiamo il contatore degli elementi

    newNode->next = s->top; // Il nuovo nodo punta al vecchio top
    s->top = newNode;       // Il top diventa il nuovo nodo
}

int sizeStack(stack* s){
    return s->size;
}

int emptyStack(stack* s){
    if(s->size == 0) return 1;
    else return 0;
}

node* top(stack* s){
    if(s->size == 0) return NULL;
    else return s->top;
}

int pop(stack* s) {
    if (s->top == NULL) return -1; // Pila vuota

    node* temp = s->top;
    int poppedValue = temp->data;
    
    s->top = s->top->next; // Spostiamo il top al nodo successivo
    free(temp);            // Liberiamo la memoria
    
    s->size--; // Decrementiamo il contatore
    return poppedValue;
}
