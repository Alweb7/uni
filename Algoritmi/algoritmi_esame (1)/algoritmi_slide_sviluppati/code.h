#ifndef CODE_H
#define CODE_H

// nelle applicazioni il tipo int può essere sostituito con qualunque altro

typedef void* Type;

// coda di el. di tipo T, ossia di puntatori a kTreeVertex

struct queueEl {
    Type            info;
    struct queueEl* next;
};

struct queueFrame {
    struct queueEl* front; // primo el. della coda
    struct queueEl* rear;  // ultimo el. della coda

    int size;
};

typedef struct queueFrame* queue;

// post: alloca e ritorna una coda vuota
queue NewQueue();
//tutte complessità temporale O(1)
//tutte complessità spaziale O(n)
// post: ritorna true se la coda è vuota, false altrimenti
bool isEmptyQueue(queue q);
// post: accoda t come ultimo elemento di q
void EnQueue (Type t, queue q);
// pre:  q non è vuota
// post: ritorna il primo el. di q SENZA rimuoverlo da q
Type First(queue q);
// pre:  q non è vuota
// post: ritorna il primo el. di q RIMUOVENDOLO da q
Type DeQueue(queue q);
int sizeQueue(queue q);
int emptyQueue(queue q);


#endif