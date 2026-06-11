#ifndef PILE_H
#define PILE_H

// Struttura del singolo nodo della lista
struct Node {
    int data;
    struct Node* next;
};

typedef struct Node node;

// Struttura della Pila (Descrittore)
struct Stack {
    struct Node* top; // Punta all'ultimo elemento inserito
    int size;         // Variabile per tener conto del numero di elementi
};

typedef struct Stack stack;

stack *createStack();
//tutte operazioni complessità temporale O(1)
//complessità temporale O(n)
void push(stack* s, int value);
int sizeStack(stack* s);
int emptyStack(stack* s);
node* top(stack* s);
int pop(stack* s);


#endif