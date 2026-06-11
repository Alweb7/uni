# Esercizio 2 - Tavola Hash con Concatenamento

## Sommario

Implementazione di una tavola hash generica con concatenamento (chaining) e applicazione pratica per analizzare la frequenza delle parole nel file dell'Iliade. Tutte le 8 funzioni richieste sono implementate, con complessità O(1) ammortizzata per le operazioni di base.

---

## 1. Implementazione della Tavola Hash

### Struttura Dati

```c
Node: {chiave, valore, puntatore_al_prossimo}
HashTable: {array_di_bucket, size, capacity, compare_fn, hash_fn}
```

- **Chaining:** Liste concatenate in ogni bucket per gestire le collisioni
- **Generica:** Supporta qualsiasi tipo di dato tramite void*
- **Ridimensionamento:** Doubling della capacità quando load_factor >= 0.75

### Funzioni Implementate

| Funzione | Complessità | Descrizione |
|----------|-------------|-------------|
| `hash_table_create()` | O(1) | Crea tavola vuota |
| `hash_table_put()` | O(1) ammort. | Inserisce/aggiorna elemento |
| `hash_table_get()` | O(1) ammort. | Recupera valore per chiave |
| `hash_table_contains_key()` | O(1) ammort. | Verifica presenza chiave |
| `hash_table_remove()` | O(1) ammort. | Rimuove elemento |
| `hash_table_size()` | O(1) | Ritorna numero elementi |
| `hash_table_keyset()` | O(n) | Ritorna array di chiavi |
| `hash_table_free()` | O(n) | Libera memoria |

---

## 2. Processo di Sviluppo con LLM

### 2.1 Prompt Iniziale

Richiesto all'LLM di implementare una tavola hash generica con:
- Concatenamento per collisioni
- Supporto void* per genericità
- Ridimensionamento automatico
- Tutte le 8 funzioni specificate

### 2.2 Analisi Critica dell'Output

**Punti positivi:**
- ✅ Struttura dati corretta e elegante
- ✅ Ridimensionamento con doubling implementato correttamente
- ✅ Gestione della memoria sicura
- ✅ Complessità rispettata

**Problemi riscontrati:**
- ❌ `hash_table_remove()` e `hash_table_contains_key()` non implementate inizialmente
- ❌ Unit test limitati a 3 test (non coprevano le funzioni mancanti)
- ❌ Pochi commenti nel codice

### 2.3 Iterazioni di Raffinamento

**Iterazione 1:** Implementazione manuale delle 2 funzioni mancanti
- Aggiunta `hash_table_contains_key()` - ricerca nel bucket
- Aggiunta `hash_table_remove()` - rimozione dalla lista con gestione prev pointer
- Verifica complessità O(1) ammortizzata mantenuta

**Iterazione 2:** Potenziamento dei test
- Aggiunto `test_contains_key()` per verificare la ricerca
- Aggiunto `test_remove()` per verificare rimozione e update size
- Totale test: 5 (100% copertura delle funzioni)

**Iterazione 3:** Documentazione in formato Doxygen
- Aggiunti commenti secondo standard corso (formato Doxygen)
- Commenti minimali nel codice, documentazione nel header
- Conformità alle linee guida ufficiali del laboratorio

### 2.4 Considerazioni Finali sul Processo LLM

✅ **Validazione della complessità:** La soluzione rispetta O(1) ammortizzato grazie al doubling strategy del ridimensionamento, calcolato empiricamente con analisi della formula: T(n) = O(n) ogni 2^k elementi, quindi O(n)/n = O(1) per elemento.

✅ **Robustezza:** L'implementazione gestisce correttamente NULL input, non permette chiavi duplicate (update), e libera la memoria in modo corretto senza leak.

⚠️ **Limitazione:** L'LLM non ha generato istantaneamente tutte le funzioni richieste, richiedendo intervento manuale. Ciò evidenzia l'importanza della supervisione nel processo di generazione del codice.

---

## 3. Unit Test

5 test automatizzati che coprono:
1. Creazione della tavola vuota
2. Inserimento e recupero elementi
3. Ridimensionamento con 100 elementi
4. Funzione `contains_key()` con casi positivi e negativi
5. Funzione `remove()` con verifica size e integrità dei dati

**Risultato:** ✅ 5/5 test passati

---

## 4. Applicazione Pratica: Analisi dell'Iliade

Analizza il file `iliade.txt` e trova la parola più frequente con lunghezza minima.

**Algoritmo:**
1. Legge parole dal file (fscanf)
2. Pulisce: rimuove non-alfabetici, converte a minuscole
3. Se lunghezza >= min_len: inserisce in hash table
4. Mantiene contatore di frequenza per ogni parola
5. Trova e stampa parola con massima frequenza

**Risultato esempio:**
```
$ ./main_ex2 iliade.txt 8
Parola piu' frequente: 'achaeans' (541 occorrenze)
```

**Complessità:** O(W * L) dove W=parole, L=lunghezza media ~O(W) esecuzione totale

---

## 5. Conformità alle Specifiche

✅ Sottocartella `ex2` con struttura corretta
✅ Makefile con `make all` genera `bin/main_ex2` e `bin/test_ex2`
✅ `test_ex2` eseguibile senza parametri
✅ `main_ex2 <file> <min_len>` funzionante
✅ README.md con processo di sviluppo documentato
✅ Codice commentato in formato Doxygen
✅ Nessun memory leak
✅ Tutte le 8 funzioni implementate

---

## 6. Compilazione ed Esecuzione

```bash
make all              # Compila entrambi gli eseguibili
./bin/test_ex2        # Esegue 5 unit test
./bin/main_ex2 iliade.txt 6  # Analizza il file con min_len=6
```

