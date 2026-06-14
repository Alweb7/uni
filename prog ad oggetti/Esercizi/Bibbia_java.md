# Bibbia Java

---

## Array monodimensionali

### Dichiarazione e inizializzazione
```java
int[] a = {1, 2, 3, 4, 5};          // diretto
int[] b = new int[10];               // con dimensione
int[] c = new int[]{1, 2, 3};       // new con valori
```

> Non puoi assegnare `{...}` a una variabile già dichiarata:
> ```java
> int[] a = new int[5];
> a = {1,2,3,4,5}; // ERRORE
> a = new int[]{1,2,3,4,5}; // OK
> ```

### Metodi utili
```java
a.length          // numero di elementi
```

### Scorrere un array
```java
// for classico
for (int i = 0; i < a.length; i++) {
    System.out.println(a[i]);
}

// for-each (senza indice)
for (int n : a) {
    System.out.println(n);
}
```

### Trovare il massimo (anche con negativi)
```java
int max = a[0]; // inizializza col primo elemento, NON con 0
for (int i = 1; i < a.length; i++) {
    if (a[i] > max) max = a[i];
}
```

---

## Array bidimensionali

### Dichiarazione e inizializzazione
```java
int[][] a = {{1,2,3},{4,5,6},{7,8,9}}; // 3 righe, 3 colonne
int[][] b = new int[3][3];
```

### Dimensioni
```java
a.length       // numero di righe
a[0].length    // numero di colonne della prima riga
a[i].length    // numero di colonne della riga i (utile per jagged array)
```

### Jagged array (righe di lunghezza diversa)
```java
int[][] a = {{1,2,3}, {4,5}, {6,7,8,9}};
// non esiste UN numero di colonne, dipende dalla riga
// usa sempre a[i].length nel loop
```

### Scorrere un array 2D
```java
// for classico
for (int i = 0; i < a.length; i++) {
    for (int j = 0; j < a[i].length; j++) {
        System.out.print(a[i][j]);
    }
    System.out.println();
}

// for-each doppio (senza indici)
for (int[] riga : a) {
    for (int n : riga) {
        System.out.print(n);
    }
    System.out.println();
}
```

### Stampa allineata (tipo tabella)
```java
System.out.printf("%4d", a[i][j]); // riserva 4 spazi per ogni numero
```

---

## Attributi di una classe

Gli attributi sono le variabili che descrivono lo stato di un oggetto, dichiarate dentro la classe ma fuori dai metodi.

```java
public class Matita {
    private String colore;
    private int lunghezza;
    private boolean temperata;
}
```

### Modificatori di accesso
| Modificatore | Accessibile da |
|---|---|
| `public` | tutte le classi |
| `private` | solo la stessa classe |
| `protected` | stessa classe + sottoclassi |
| (niente) | stesso package |

> In genere gli attributi si dichiarano `private` e si accede tramite getter/setter.

### Altri modificatori
| Modificatore | Significato |
|---|---|
| `static` | appartiene alla classe, condiviso tra tutti gli oggetti |
| `final` | costante, non modificabile dopo l'inizializzazione |
| `static final` | costante globale di classe |

```java
public class Esempio {
    private String nome;                  // normale
    private static int contatore;         // condiviso tra tutti gli oggetti
    private final String MARCA;           // non cambia dopo la creazione
    public static final int MAX = 10;     // costante globale
}
```

---

## Costanti (equivalente di #define)

In Java non esiste `#define`. Si usa `static final`:

```java
static final int MAX = 10;
static final double PI = 3.14;
```

> Per convenzione i nomi delle costanti si scrivono in **MAIUSCOLO** con underscore.
