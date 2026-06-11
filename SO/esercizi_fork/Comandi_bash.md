
# Riassunto Scripting Bash - Laboratorio di Sistemi Operativi

Questo documento raccoglie in un unico file tutti i programmi, costrutti, operatori e comandi principali analizzati nella dispensa introduttiva a Bash.

---

## 1. Redirezioni e Pipe
Bash permette di combinare programmi e deviare i flussi di input/output.
* **Redirezione output (`>`):** Scrive l'output di un comando in un file (sovrascrivendolo).
    ```bash
    $ cmd > outfile
    ```
* **Redirezione in append (`>>`):** Aggiunge l'output in fondo a un file esistente.
    ```bash
    $ cmd >> outfile
    ```
* **Esecuzione sequenziale e redirezione mista:** Combina input da file ed esegue più comandi sulla stessa riga, separandoli con `;`.
    ```bash
    $ cmd1 < dati; cmd2; cmd3 >> outfile
    ```
* **Pipe (`|`):** Passa l'output del primo comando come input diretto del secondo.
    ```bash
    $ cmd1 | cmd2 | cmd3
    ```

---

## 2. Il Primo Script e l'Uso delle Variabili
Ogni script dovrebbe iniziare con lo *shebang* `#!/bin/bash` per indicare al sistema quale interprete usare. Prima di eseguirlo, ricorda di dargli i permessi con `chmod +x nome_script`.

### Hello World e Variabili Base
```bash
#!/bin/bash
# questo è un commento!!!
VAR=prova
VAR1=23

# Stampa di variabili base (si usa il $)
echo $VAR $VAR1

# Stampa dei parametri posizionali passati da riga di comando (es. come argv in C)
# $0 è il nome dello script stesso, $1 è il primo argomento, $2 il secondo, ecc.
echo $0 $1

```

### Le Regole del Quoting (Gli Apici)

Il modo in cui si usano gli apici cambia radicalmente l'interpretazione delle variabili:

```bash
#!/bin/bash
VAR=prova

# APICI DOPPI: Effettua la sostituzione (stampa "il valore è prova")
echo "il valore è $VAR" 

# APICI SINGOLI INVERSI (Backtick): Esegue il comando contenuto all'interno
echo `ls -l $VAR` 

# APICI SINGOLI: Stampa letteralmente la stringa senza interpretare il $ (stampa "$VAR")
echo '$VAR' 

```

---

## 3. Operazioni Matematiche con `let`

Per fare calcoli aritmetici e assegnare i risultati alle variabili si usa il comando `let`.

**Esempio pratico: Calcolo dei file nascosti**

```bash
#!/bin/bash
# Conta tutti i file (inclusi i nascosti)
tmp=`ls -la | wc -l`
# Conta solo i file normali
tmp1=`ls -l | wc -l`

echo "numero di elementi nella directory: "$tmp
echo "di questi, non hidden: "$tmp1

# Esegue la sottrazione matematica
let "diff=$tmp-$tmp1"

echo "hidden: $diff"

```

---

## 4. Strutture di Controllo e Cicli

### Tabelle degli Operatori (Costrutto `[ ]`)

Negli script Bash, si usano spazi obbligatori attorno alle parentesi quadre `[ ]`.

**Operatori Logici:**

* **AND (`&&`):** `[ cond1 ] && [ cond2 ]` (Vero se entrambe vere)
* **OR (`||`):** `[ cond1 ] || [ cond2 ]` (Vero se almeno una vera)

**Operatori di Confronto Numerico:**

* `-eq` : Uguale a
* `!=` (o `-ne`) : Diverso da
* `-gt` : Maggiore di
* `-ge` : Maggiore o uguale
* `-lt` : Minore di
* `-le` : Minore o uguale

**Operatori di Test sui File (Es. `[ -r file.txt ]`):**

* `-f` : Esiste ed è un file normale
* `-s` : Esiste e NON è vuoto (dimensione > 0)
* `-r` : Esiste ed è leggibile (Read)
* `-w` : Esiste ed è scrivibile (Write)
* `-x` : Esiste ed è eseguibile (eXecute)
* `-d` : Esiste ed è una directory
* `-h` : Esiste ed è un link simbolico

---

### Ciclo `while`

Esegue un blocco di codice finché la condizione è vera.

```bash
#!/bin/bash
let "ripeti = 1"

# -lt significa "less than" (minore di 3)
while [ $ripeti -lt 3 ]
do
    echo "hip hip"
    let "ripeti = $ripeti + 1"
done
echo "hurra!!"

```

### Ciclo `while` con input da tastiera (`read`)

Si usa `read` per acquisire input dall'utente (stdin) e salvarlo in una variabile.

```bash
#!/bin/bash
finito=go

while [ $finito != quit ]
do
    echo "un altro giro? [go/quit]"
    read finito
done

```

### Ciclo `for ... in`

Itera su una lista di valori automaticamente.

```bash
#!/bin/bash
# Cerca tutti i file con estensione .c e li stampa
for myf in `ls *.c`
do
    echo $myf
done

```

### Script avanzato: Pulizia IPC (Semafori)

Unisce un ciclo `for` all'output filtrato con `grep` e `awk` per eliminare array di semafori.

```bash
#!/bin/bash
# ipcs -s mostra i semafori, ipcrm li rimuove
for i in `ipcs -s | grep radicion | awk '{print $2}'`; do
    ipcrm -s $i;
done

```

### Costrutto `if then else`

Valuta condizioni logiche usando gli operatori visti in precedenza.

```bash
#!/bin/bash
DEFDIR=/path/di/default

if [ $1 ] # Se il primo parametro esiste
then
    if [ $1 = "-d" ] # Se il parametro è esattamente "-d"
    then
        mia_var=`wc -l $2`
        echo $mia_var
    else
        echo "opzione sconosciuta"
    fi
else
    # Caso in cui non è stato passato alcun parametro
    mia_var=`wc -l $DEFDIR`
    echo $mia_var
fi

```

---

## 5. Esportazione Variabili e Configurazione

Le variabili di uno script non sono visibili alle "sotto-shell" a meno che non vengano esplicitamente esportate tramite il comando `export`.

**Esempio di Scope (script1 chiama script2):**

```bash
# ----- File: mio_script.sh -----
# Esportiamo la variabile per renderla globale alle sotto-shell
export dato=`cat mioinput`
./saluta.sh

# ----- File: saluta.sh -----
# Questo script ora può leggere la variabile 'dato'
echo "ciao $dato"

```

### File di inizializzazione `.bashrc`

Contiene comandi eseguiti automaticamente all'avvio della shell. Utile per definire alias o aggiornare i percorsi di sistema (PATH).

```bash
# Aggiunge la directory corrente ('.') e "mybin" ai percorsi di ricerca degli eseguibili
export PATH=.:mybin:$PATH 

# Creazione di alias per risparmiare tempo digitando a terminale
alias a=alias
a hi=history
a m=more
a c=clear
a rm="rm -i"
a x=exit
a so=source
a d=date

```

---

## 6. Ricerca Avanzata: `grep` e Regex

La sintassi generale per cercare testo nei file è: `grep [opzioni] [pattern] [file]`. I pattern utilizzano le **Espressioni Regolari (Regex)**.

### Caratteri speciali Regex:

* `^` : Ancora di inizio linea (es. `^Mario` trova Mario solo a inizio riga).
* `$` : Ancora di fine linea (es. `Mario$` trova Mario solo a fine riga).
* `.` : Sostituisce **un** qualsiasi singolo carattere.
* `\` : Carattere di escape. Tratta il carattere successivo come testo normale (es. `\$`).
* `[ ]` : Range. Cerca uno qualsiasi dei caratteri indicati (es. `[a-z]`).
* `*` : L'elemento precedente è ripetuto zero o più volte.

### Opzioni fondamentali di `grep`:

* `-i` : Ignora le maiuscole/minuscole.
* `-v` : Invert match (Trova tutte le linee che **NON** contengono il pattern).
* `-c` : Restituisce solo il numero (count) di linee trovate.
* `-n` : Mostra anche il numero della riga del match.
* `-w` : Cerca la parola intera esatta.
* `-E` : Usa le Espressioni Regolari Estese.
* `-A num` : Mostra `num` righe *dopo* (After) il match.
* `-B num` : Mostra `num` righe *prima* (Before) il match.
* `-C num` : Mostra `num` righe prima E dopo il match (Context).

---

## 7. Utilizzo di AWK

`awk` è un potente strumento per il processamento riga per riga di file strutturati in colonne/campi. La sintassi base è: `awk <pattern> {<azioni>}`.

**1. Stampa linee che contengono una parola:**

```bash
$ awk '/Mario/' anagrafica.txt

```

**2. Cambiare il separatore e stampare campi specifici:**
Imposta la virgola come separatore (`FS=","`) e stampa la colonna 1 e 2 (`$1`, `$2`).

```bash
$ awk 'BEGIN {FS=","} /Mario/ {print $1, $2}' anagrafica.txt

```

**3. Operazioni multiple e calcoli sui campi:**
Cerca "Mario", somma le loro età (campo 4), stampa nome ed età, e infine stampa il totale usando il blocco `END`.

```bash
$ awk 'BEGIN {FS=","} \
    /Mario/ {eta += $4} \
    /Mario/ {print $1, $3, $4} \
    END {print "eta tot dei Mario = " eta}' anagrafica.txt

```

**4. Elaborazione file separati da Tabulazione (Gestione NF):**
Utilizza la variabile integrata `NF` (Number of Fields) per contare quanti campi ha ogni riga.

```bash
$ cat infile.txt | awk -F $'\t' '{ print $1 "\t" NF-1 }' newfile

```

```

```