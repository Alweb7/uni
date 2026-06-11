/*Esercizi_Vari.c*/

/*1°:
 *Realizzare funzione che implementi algoritmo della bandiera:
 *Prima verdi, poi bianchi infine rossi.
 *Usare il tipo enumerativo 'Color'. 'n' è dimensione array.
 *Funzione swap preimplementata.
*/
    void Bandiera(Color B[], int n) {
        int i = 0, j = 0;
        for(int k = 0; k < n; k++) {
            if(B[k] == bianco) {
                swap(B, j, k);
                j++
            }
            else if(B[k] == verde){
                swap(B, j, k);
                swap(B, i, j);
                i++;
                j++;
            }
            /*se rosso rimane dov'è.*/
        }
    }

/*2°:
 *Scrivere una funzione che conta il numero di confronti effettuati mentre viene
 *ordinato un array con Bubble Sort.
 *'n' e' dimensione dell'array. 
*/
    int CountBubble(int A[], int n) {
        int count = 0;
        bool flag = true;
        for(int i = n - 1; i > 0 && flag; i--) {
            flag = false;
            for(int j = 0; j < i; j++) {
                count++;
                if(A[j] > A[j + 1]) {
                    swap(A, j, j + 1);
                    sw = true;
                }
            }
        }
        return count;
    }

/*3°:
 *Scrivere la funzione 'Merge' dell'algoritmo Merge Sort.
 *La struttura dati su cui basarsi per scriverlo e' la lista. 
*/
    list Merge(list l, list m) {
        if(l == NULL) {
            return m;
        }
        else if(m == NULL) {
            return l;
        }
        else {
            if(l->info <= m->info) {
                l->next = Merge(l->next, m);
                return l;
            }
            else {
                m->next = Merge(l, m->next);
                return m;
            }
        }
    }

/*4°:
 *Scrivere la funzione 'Partition' dell'algoritmo Quick Sort.
 *La struttura dati su cui basarsi per scriverlo è l'array di interi. 
*/
    int Partition(int A[], int low, int high) {
        int i, j, tmp;
        int pivot = A[high];
        i = low - 1;
        for(j = low; j < high; j++) {
            if(A[j] < pivot) {
                tmp = A[i];
                A[i] = A[j];
                A[j] = tmp;
            }
        }
        tmp = A[i + 1];
        A[i + 1] = A[high];
        A[high] = tmp;
        return i + 1;
    }

/*5°:
 *Scrivere una funzione che implementi la insert in una tabella hash ad 
 *indirizzamento aperto. 'Cons' mette k dopo array[j - 1] e prima di array[j + 1].
 *'HashFun(int k, int i)' è funzione hash preimplementata.
*/
    int Insert(HasTable map, int k) {
        int i = 0;
        while(i < map->dim) {
            int j = HashFun(k, i);
            if(map->array[j] == NULL) {
                map->array[j] = Cons(k, map->array[j - 1], map->array[j + 1]);
                return j;
            }
            i++;
        }
        return NULL;
    }

/*6°:
 *Scrivere una funzione che implementi la search in una tabella hash ad 
 *indirizzamento aperto.
 *'HashFun(int k, int i)' è funzione hash preimplementata.
*/
    int Search(HashTable map, int k) {
        int i = 0;
        while(i < map->dim) {
            int j = HashFun(k, i);
            if(map->array[j] == NULL) {
                return -1;
            }
            else if(map->array[j]->info == k) {
                return map->array[j]->info;
            }
        }
    }

/*7°:
 *Scrivere una funzione che implementi l'estrazione del minimo da un MinHeap 'h'
 *avente 'h.size' > 0.
 *(Tolgo elemento iniziale, al suo posto ci metto quello finale e chiamo heapify).
 *(Per semplicita' gli elementi dello heap sono interi).
*/
    int ExctractMin(Heap h) {
        int res = h[0];
        h[0] = h[h.size - 1];
        h.size--;
        Heapify(h, 0);
    }
    void Heapify(Heap v, int i) {
        int tmp;
        int smallest = min(v[i], min(v[2 * i + 1], v[2 * i + 2])); 
                    /*'min' ritorna indice non valore*/
        if(smallest != i) {
            tmp = v[i];
            v[i] = v[smallest];
            v[smallest] = tmp;
            Heapify(v, smallest);
        }
    }

/*8°:
 *Scrivere una funzione per realizzare la struttura dati MaxHeap dato un array
 *non ordinato.
 *(Itera heapify da penultimo livello a radice. Questo perchè il penultimo livello
 * ha ancora figli, l'ultimo no, l'ultimo sono foglie e le foglie sono in 
 * A[length/2..length]).
 *Implementare inoltre l'algoritmo di ordinamento HeapSort.
*/
    void Heapify(int v[], int i) {
        int tmp;
        int biggest = max(v[i], max(v[2 * i + 1], v[2 * i + 2]));
                    /*'max' ritorna indice non valore*/
        if(biggest != i) {
            tmp = v[i];
            v[i] = v[biggest];
            v[biggest] = tmp;
            Heapify(v, biggest);
        }
    }
    void BuildHeap(int v[]) {
        /*se array parte da 0 serve '-1', se parte da 1 solo 'length/2'*/
        for(int i = (lenght(v) / 2 - 1); i > 0; i--) {
            Heapify(v, i);
        }
    }
    void HeapSort(int v[]) {
        int tmp;
        BuildHeap(v);
        for(int i = (lenght(v) - 1); i >= 0; i--) {
            tmp = v[0];
            v[0] = v[i];
            v[i] = tmp;
            Heapify(v, 0);
        }
    }

/*9°:
 *Scrivere una funzione che dato un array di interi restituisca la lunghezza
 *maggiore di un sottosegmento dell'array ordinato in senso decrescente.
 *Esempio: 1-5-25-17-6-4-12-10 lunghezza = 4 (sottosegmento = 25-17-6-4).
*/
    int MaxLen(int v[]) {
        int size = lenght(v), count = 1, max = 1;
        for(int i = 1; i < size; i++) {
            if(v[i - 1] > v[i]) {
                count++;
            }
            else {
                if(count > max) {
                    max = count;
                }
                count = 1;
            }
        }
        if(count > max) {
            max = count;
        }
        return max;
    }

/*10°:
 *Scrivere una funzione che esegua l'operazione insert in un MinHeap.
 *(inserimento sempre in ultima posizione e poi risalgo confrontando con parent e 
 * se minore viene scambiato).
 *Heap rappresentato come array.
*/
    void Insert(int h[], int x) {
        lenght(h)++;
        int i = lenght(h), tmp;
        h[i] = x;
        while(i > 0 && h[i / 2] > h[i]) {
            tmp = h[i];
            h[i] = h[i / 2];
            h[i / 2] = tmp;
            i = i / 2;
        }
    }

/*11°:
 *Scrivere due funzioni (ricorsiva e iterativa) che calcolino l'esponenziale veloce
 *basandosi su:  x^2k = x^k*x^k e x^2k+1 = x^k*x^k*x.
*/
    int ExpRec(int x, int n) {
        if(n == 0) {
            return 1;
        }
        else {
            int y = ExpRec(x, n/2);
            if(n % 2 == 0) {
                return y * y;
            }
            else {
                return y * y * x;
            }
        }
    }
    int ExpIter(int x, int n) {
        int y = x, k = n, z = 1;
        while(k > 0) {
            if(k % 2 == 1) {
                z = z * y;
            }
            y = y * y;
            k = k/2;
        }
        return z;
    }

/*12°:
 *Scrivere due funzioni (ricorsiva e iterativa) che eseguano la ricerca lineare
 *in un array di interi. Parametro 'i' e' indice elemento iniziale.
*/
    int SearchRec(int A[], int i, int a) {
        int j = lenght(A);
        if(i > j) {
            return -1;
        }
        else {
            if(A[i] = a) {
                return i;
            }
            else {
                return SearchRec(A, i + 1, a);
            }
        }
    }
    int SearchIter(int A[], int i, int a) {
        int j = lenght(A), k = i;
        while(k < j && A[k] != a) {
            k++;
        }
        if(k > j) {
            return -1;
        }
        else {
            return k;
        }
    }

/*13°:
 *Scrivere due funzioni (ricorsiva e iterativa) che eseguano la ricerca dicotomica
 *in un array ordinato non decrescente di interi.
 *Parametro 'i' e' indice elemento iniziale mentre 'j' è indice elemento finale.
*/
    int SearchRec(int A[], int i, int j, int a) {
        if(i > j) {
            return -1;
        }
        else {
            int m = (i + j) / 2;
            if(A[m] == a) {
                return m;
            } 
            else {
                if(A[m] < a) {
                    return SearchRec(A, m + 1, j, a);
                }
                else {
                    return SearchRec(A, i, m - 1, a);
                }
            }
        }
    }
    int SearchRec(int A[], int i, int j, int a) {
        int l = i, h = j;
        while(l <= h) {
            m = (i + j) / 2;
            if(A[m] == a) {
                return m;
            }
            else {
                if(A[m] < a) {
                    l = m + 1;
                }
                else {
                    h = m - 1;
                }
            }
        }
        return -1;
    }