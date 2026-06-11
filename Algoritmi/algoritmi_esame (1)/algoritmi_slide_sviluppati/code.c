/**************************************
*  Code semplici di el. di tipo T = int
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define SEPARATOR "#<ab@17943918#@>#"
#include "code.h"

// post: alloca e ritorna una coda vuota
queue NewQueue() {
    queue q = malloc(sizeof(struct queueFrame));
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

//tutte complessità temporale O(1)
//tutte complessità spaziale O(n)
// post: ritorna true se la coda è vuota, false altrimenti
bool isEmptyQueue(queue q) {
    return q->front == NULL;
}

// post: accoda t come ultimo elemento di q
void EnQueue (Type t, queue q) {
    struct queueEl* newEl = malloc(sizeof(struct queueEl));
    newEl->info = t;
    newEl->next = NULL;
    if (q->front == NULL)
        q->front = q->rear = newEl;
    else{ // q->front != NULL implica q->rear != NULL
        q->rear->next = newEl;
        q->rear = newEl;
    }

    q->size ++;
}

// pre:  q non è vuota
// post: ritorna il primo el. di q SENZA rimuoverlo da q
Type First(queue q) {
    return q->front->info;
}

// pre:  q non è vuota
// post: ritorna il primo el. di q RIMUOVENDOLO da q
Type DeQueue(queue q) {
    if(sizeQueue(q) == 0) return 0;
    Type t = q->front->info;
    struct queueEl* oldFirst = q->front;

    if (q->front == q->rear) // la coda ha un solo el.
        q->front = q->rear = NULL;
    else
        q->front = q->front->next;

    free(oldFirst);
    q->size --;

    return t;
}

int sizeQueue(queue q){
    return q->size;
}

int emptyQueue(queue q){
    if(q->front == q->rear) return 1;
    else return 0;
}