# 🚀 Setup TommyAI v2 - Backend Ibrido

## Prerequisiti

### 1️⃣ Ollama (Veloce, Gratis, Offline)

#### Windows
```powershell
# 1. Scarica da https://ollama.ai/download
# 2. Installa l'installer
# 3. Apri PowerShell e installa il modello:
ollama pull llama2

# 4. Avvia Ollama (lancia una volta, rimane in background)
ollama serve
```

Ollama sarà disponibile su `http://localhost:11434`

#### macOS
```bash
brew install ollama
ollama pull llama2
ollama serve
```

#### Linux
```bash
curl -fsSL https://ollama.ai/install.sh | sh
ollama pull llama2
ollama serve
```

**Nota**: La prima volta impiega 5-10 min (è 4GB), poi è istantaneo!

---

### 2️⃣ Dipendenze Python

```bash
pip install requests anthropic customtkinter pillow
```

---

### 3️⃣ Configurazione (Opzionale)

#### Se vuoi usare Claude per insights avanzati:

Crea file `.env` nella root del progetto:
```
CLAUDE_API_KEY=sk-ant-xxxxxxxxxxxxx
OLLAMA_URL=http://localhost:11434
```

**Come ottenere API key Claude gratis**:
1. Vai su https://console.anthropic.com
2. Sign up
3. Prendi i primi $5 crediti gratis
4. Copia la key in `.env`

---

## 📊 Struttura Moduli

```
TommyAI v2
├── core/
│   ├── file_analyzer.py         # Scansione e stats
│   ├── duplicate_handler.py     # Rilevamento doppioni
│   ├── smart_organizer.py       # Ordinamento intelligente
│   └── cache_manager.py         # Pulizia cache/temp
│
├── ai/
│   ├── hybrid_brain.py          # Backend Ollama + Claude
│   └── gemini_brain.py          # (legacy - ancora usato)
│
├── main.py                      # UI principale
├── test_demo.py                 # Demo/test tutti moduli
└── .env                         # Config (opzionale)
```

---

## ✅ Verifiche Setup

### 1️⃣ Ollama Online?
```python
import requests
response = requests.get("http://localhost:11434/api/tags")
print("Ollama OK!" if response.status_code == 200 else "Ollama offline")
```

### 2️⃣ Esegui demo
```bash
python test_demo.py
```

Dovrebbe mostrare:
- ✅ File scanner
- ✅ Duplicate detection
- ✅ Smart organization
- ✅ Cache analysis
- ✅ Hybrid brain status

---

## 🎯 Uso nei Comandi

### Utente chiede: "Pulisci il PC dai doppioni"

1. **FileAnalyzer** → Scansiona directory
2. **DuplicateHandler** → Rileva hash duplicati
3. **HybridBrain.smart_analyze** → Claude decide quali tenere
4. **DuplicateHandler.safe_delete** → Elimina con backup

### Utente chiede: "Organizza i miei file"

1. **FileAnalyzer** → Categorizza
2. **SmartOrganizer** → Suggerisce cartelle
3. **HybridBrain.quick_classify** → Ollama veloce
4. **SmartOrganizer.auto_sort** → Sposta file

### Utente chiede: "Report del PC"

1. **FileAnalyzer.scan_directory** → Scansione
2. **CacheManager.estimate_cleanup** → Analizza cache
3. **HybridBrain.generate_insights** → Claude insights
4. **FileAnalyzer.generate_report** → Report completo

---

## 🔧 Troubleshooting

### "Ollama offline"
```
Solluzione:
1. Ollama aperto? Esegui: ollama serve
2. Porta corretta? Check http://localhost:11434
3. Modello scaricato? Esegui: ollama pull llama2
```

### "Claude API key non valida"
```
Soluzione:
1. Vai su https://console.anthropic.com
2. Copia key corretta
3. Incolla in .env CLAUDE_API_KEY=sk-ant-...
```

### "Permission denied" su file delete
```
Soluzione:
- Usa safe_delete() che crea backup
- Check permissions folder
- Prova cartella diversa
```

---

## 📈 Performance Esperati

| Operazione | Tempo | Tool |
|-----------|-------|------|
| Scan 1000 file | 1-2s | FileAnalyzer |
| Find duplicates | <1s | DuplicateHandler |
| Smart analyze | 2-5s | Claude (se attivo) |
| Quick classify | <100ms | Ollama |
| Generate report | 2-3s | Combined |

---

## 🎬 Next Steps

1. ✅ Installa Ollama + modello
2. ✅ Installa dipendenze Python
3. ✅ Esegui `python test_demo.py`
4. ✅ Verifica tutto funziona
5. ⏳ Integrazione UI (prossimo step)

---

Ready? Dammi ok quando Ollama è up! 🚀
