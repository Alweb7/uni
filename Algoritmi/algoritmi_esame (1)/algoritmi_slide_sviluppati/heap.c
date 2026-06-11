#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *array;      // Puntatore ai dati
    int size;       // Elementi attuali
    int capacity;   // Spazio totale allocato
} Heap;

typedef struct {
    int *array;      // Puntatore ai dati
    int size;       // M
    int capacity;   // Spazio totale allocato
    int el_da_ord;  // N
} Heap_per_sort;

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

//restituisce la posizione del genitore se esiste, -1 altrimenti
int parent(Heap *H, int i){
    if(i > 0) return (i - 1) / 2;
    else return -1;
}

//restituisce la posizione del figlio sinistro se esiste, -1 altrimenti
int left(Heap *H, int i){
    int l = 2 * i + 1;
    if(l < H->size) return l;
    else return -1;
}

//restituisce la posizione del figlio destro se esiste, -1 altrimenti
int right(Heap *H, int i){
    int r = 2 * i + 2;
    if(r < H->size) return r;
    else return -1;
}

//complessità O(log n) (n numero di nodi dell'albero)
void heapInsert(Heap *H, int x){
    if(H->size == H->capacity){
        H->capacity = H->capacity * 2;
        H->array = (int *)realloc(H->array, H->capacity * sizeof(int));
    }
    
    int posiz = H->size;
    H->array[posiz] = x;
    H->size++;
    
    int index_padre = parent(H, posiz);
    while(posiz > 0 && index_padre >= 0 && H->array[posiz] > H->array[index_padre]){
        swap(&H->array[posiz], &H->array[index_padre]);
        posiz = index_padre;
        index_padre = parent(H, posiz);
    }
}

//assicura che l'albero con radice in i è heap massimo
void heapify_down(Heap* H, int i){
    int l = left(H, i);
    int r = right(H, i);
    int massimo = i;
    
    if(l != -1 && H->array[l] > H->array[massimo]){
        massimo = l;
    }
    if(r != -1 && H->array[r] > H->array[massimo]){
        massimo = r;
    }
    
    if(massimo != i){
        swap(&H->array[massimo], &H->array[i]);
        heapify_down(H, massimo);
    }
}

//O(log n)
void heap_extract(Heap* H){
    H->array[0] = H->array[H->size - 1];
    H->size--;

    heapify_down(H, 0);
}

void heapify_downSort(Heap_per_sort* H, int i){
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int massimo = i;
    
    if(l < H->el_da_ord && H->array[l] > H->array[massimo]){
        massimo = l;
    }
    if(r < H->el_da_ord && H->array[r] > H->array[massimo]){
        massimo = r;
    }
    
    if(massimo != i){
        swap(&H->array[massimo], &H->array[i]);
        heapify_downSort(H, massimo);
    }
}

//O(n logn)
void buildHeapPerSort(Heap_per_sort* V){
    V->el_da_ord = V->size;
    for(int i = V->size / 2; i >= 0; i--){
        heapify_downSort(V, i);
    }
}

//O(n logn)
void heapSort(Heap_per_sort* V){
    buildHeapPerSort(V);
    
    for(int i = V->el_da_ord - 1; i > 0; i--){
        swap(&V->array[i], &V->array[0]);
        V->el_da_ord--;
        heapify_downSort(V, 0);
    }
}

// pre:  hd < dim A[]
// post: A[0..hd] e' uno heap massimo
void buildMaxHeap(int A[], int dim)
{
    for (int i = dim/2; i >= 0; i--) 
        maxHeapify(A, i, dim);
}

// pre:  0 < dim <= dim A[]
// post: A[0..dim-1] ordinato in modo non decrescente
void heapSort(int A[], int dim)
{
    buildMaxHeap(A, dim-1); 
    
    for (int i = dim-1; i > 0; i--) { 
        int temp = A[i]; 
        A[i] = A[0]; 
        A[0] = temp; 
        
        maxHeapify(A, 0, i-1); 
    }
}


int main(){
    return 0;
}