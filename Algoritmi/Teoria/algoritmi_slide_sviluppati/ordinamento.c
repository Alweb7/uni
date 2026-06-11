#include <stdio.h>

//ricerca di un elemento nel vettore ordinato (se non ordinato al più n)
//al più log n
int BinSearchRic(int x, int A[], int i, int j){
    if(i>j) return 0;
    else{
        int m = (i+j)/2;
        if(x == A[m]) return 1;
        else{
            if(x < A[m]) return BinSearchRic(x, A, i, m-1);
            else return BinSearchRic(x, A, m+1, j);
        }
    }
}

//=====================================================================
//==== ORDINARE UN VETTORE ====
//trivial sort: genera tutte le permutazioni e sceglie quella con elementi ordinati
//il numero di permutazioni è n! -> cresce di più di 2^n
int sorted(int A[], int n){
    for(int i=1; i<=n; i++){
        if(A[i-1] > A[i]) return 0;
    }
    return 1;
}

int TrivialSort(int A[]){
    for(/*all permutazione di A */;;){
        if(sorted(/*permutazione di A*/ NULL, 0)==1){
            return /*permutazione di A*/ A;
        }
    }
}

//Insertion Sort: data un vettore con la parte sinistra, A[1..i−1], ordinata, è facile inserirci l'elemento A[i] in
//modo tale che il sottovettore A[1..i] risulti ordinata (aumentando così la parte ordinata).
//O(n)
//caso peggione esecuzione a*n^2
//caso migliore esecuzione a*n
void InsertionSort(int A[], int n){
    for(int i=1; i<=n; i++){ //eseguito quindi n volte
        int j=i;
        while(j>1 && A[j-1]>A[j]){
            swap(A[j-1], A[j]); //O(1)
            j--;
        }
    }
}

//Selection Sort: data un vettore in cui la parte sinistra, A[1..i − 1], è ordinata e contiene gli i − 1 numeri
//più piccoli del vettore, si cerca il minimo della parte A[i..n] e si mette nella posizione i (aumentando così la parte ordinata).
// caso migliore = caso peggiore = n^2
void SelectionSort(int A[], int n){
    for(int i=0; i<n-1; i++){
        int k=i;
        for(int j=i+1; j<n; j++){
            if(A[k] > A[j]){
                i = j;
            }
        }
        swap(A[i], A[k]);
    }
}

//Quick Sort: si seleziona un elemento del vettore, detto perno, attorno al quale si
//riorganizzano gli altri elementi in modo tale che gli elementi più piccoli o uguali al perno si trovino in
//posizioni precedenti a quella del perno e gli elementi più grandi in posizioni successive; poi si ripete lo stesso
//procedimento a sinistra e a destra del perno.
//caso peggiore: perno è sempre minimo o massimo del sottoarray organizzato da partition perchè viene effettuata solo
//               1 chiamata ricorsiva delle due -> Theta(n^2)  
//caso medio -> O(nlog n)               
void QuickSort(int A[], int left, int right){ //all'inizio left = 0; right = n -> A[left, ..., right]
    if(right - left >1){
        int p = partition(A, left, right);
        if(p>2) QuickSort(A, left, p-1);
        if(p < right - 1) QuickSort(A, p+1, right);
    }
}

//Partition utilizza A[1] come perno e considera una volta tutti gli elementi del sottovettore che deve riorganizzare.
//numero di operarazioni a*n (a è una costante, n num degli elementi di A)
int partition(int A[], int left, int right){
    int i = left+1;
    int j = right;
    while(i <= j){
        if(j > left && A[j] >= A[left]) j--;
        else if(i < right && A[i] <= A[left]){
            i = i+1;
        }
        else{
            swap(A[i], A[j]);
            i++;
            j--;
        }
    }
    swap(A[left], A[j]);
    return j;
}


//Merge Sort: vettore che contiene un elemento è ordinato;
//se il vettore contiene più di un elemento allora viene diviso in due parti bilanciate;
//ognuna di queste due parti viene ordinata applicando ricorsivamente l'algoritmo;
//le due parti vengono fuse.
//caso medio -> O(nlog n)               
void MergeSort(int A[], int left, int right){
    if(right - left == 1) return;
    else{
        int m = (right - left) / 2;
        MergeSort(A, left, m);
        MergeSort(A, m+1, right);
        Merge(A, left, m, right);
    }
}


void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1; // Lunghezza del primo sotto-array
    int n2 = right - mid;    // Lunghezza del secondo sotto-array

    // Crea array temporanei per copiare i dati
    int L[n1], R[n2];

    // Copia i dati nei sotto-array temporanei L[] e R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Unisci gli array temporanei L e R nell'array originale arr[left...right]
    i = 0; // Indice iniziale del primo sotto-array
    j = 0; // Indice iniziale del secondo sotto-array
    k = left; // Indice iniziale dell'array unito
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    // Copia gli elementi rimanenti di L[], se ci sono
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    // Copia gli elementi rimanenti di R[], se ci sono
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}