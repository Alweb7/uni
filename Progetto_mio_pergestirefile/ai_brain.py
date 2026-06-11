import json
import os
import re
from typing import Optional

import requests
from pydantic import BaseModel


class AIAction(BaseModel):
    azione: str
    nome: Optional[str] = None
    contenuto: Optional[str] = None
    percorso: Optional[str] = None
    testo_risposta: str


class GeminiBrain:
    """Compatibilita' con la UI esistente, ma backend locale Ollama."""

    def __init__(self, api_key=None, ollama_url=None, model=None):
        self.ollama_url = (ollama_url or os.getenv("OLLAMA_URL") or "http://localhost:11434").rstrip("/")
        configured_model = model or os.getenv("OLLAMA_MODEL")
        self.model_id = configured_model or self._first_available_model() or "llama2"
        self.timeout = int(os.getenv("OLLAMA_TIMEOUT", "90"))
        self.use_json_format = not self.model_id.lower().startswith("llama2")

    def _first_available_model(self):
        try:
            response = requests.get(f"{self.ollama_url}/api/tags", timeout=2)
            response.raise_for_status()
            models = response.json().get("models", [])
            preferred = (
                "llama3.2:1b",
                "qwen2.5:0.5b",
                "qwen2.5:1.5b",
                "phi3:mini",
                "llama2:latest",
                "llama2",
            )
            names = [model.get("name") for model in models if model.get("name")]
            for model_name in preferred:
                if model_name in names:
                    return model_name
            if models:
                return models[0].get("name")
        except Exception:
            pass
        return None

    def elabora_comando(self, comando_utente, percorso_attuale, file_context=None):
        azione_locale = self._azione_locale(comando_utente, file_context=file_context)
        if azione_locale:
            return azione_locale

        if not self._sembra_comando_file(comando_utente):
            return self._risposta_chat(comando_utente, file_context=file_context)

        contesto_file = f"\n\nContesto file caricato:\n{file_context}" if file_context else ""
        istruzioni = f"""Sei TommyAI, un assistente locale per la gestione file.
Percorso attuale: {percorso_attuale}
{contesto_file}

Rispondi SEMPRE e SOLO con un oggetto JSON valido, senza markdown.
Schema:
{{
  "azione": "vai_a|crea_file|crea_cartella|elimina|analizza_cartella|suggerisci_organizzazione|organizza_cartella|trova_doppioni|pulizia_temp|messaggio",
  "nome": "nome file o cartella, se serve",
  "contenuto": "contenuto del file, solo per crea_file",
  "percorso": "percorso assoluto o relativo, solo per vai_a",
  "testo_risposta": "risposta breve in italiano"
}}

Regole:
- Usa "messaggio" se l'utente chiede solo informazioni o se il comando non e' chiaro.
- Per creare o eliminare file/cartelle usa solo nomi semplici quando possibile.
- Per "vai_a", se l'utente dice una cartella relativa, mettila in "percorso".
- Usa "analizza_cartella" per report/statistiche della cartella corrente.
- Usa "suggerisci_organizzazione" per mostrare come riorganizzare senza spostare file.
- Usa "organizza_cartella" solo se l'utente chiede chiaramente di applicare/spostare/organizzare davvero.
- Usa "trova_doppioni" per cercare duplicati.
- Usa "pulizia_temp" per stimare file temporanei/cache rimovibili.
- Se e' presente un file caricato, puoi rispondere a domande sul suo contenuto usando "messaggio".
- Non dire di aver visto immagini se hai solo metadati o anteprima testuale.
- Non proporre eliminazioni di file di sistema o cartelle applicative.
- Non inventare azioni diverse da quelle elencate."""

        prompt = f"{istruzioni}\n\nComando utente: {comando_utente}"

        try:
            raw_text = self._generate_ollama(prompt)
            data = self._parse_json_or_message(raw_text)
            return self._coerenza_azione_modello(self._build_action(data), comando_utente)

        except requests.exceptions.ConnectionError:
            return AIAction(
                azione="messaggio",
                testo_risposta=(
                    "Ollama non sembra avviato. Apri PowerShell e lancia: "
                    "ollama serve"
                ),
            )
        except requests.exceptions.Timeout:
            return AIAction(
                azione="messaggio",
                testo_risposta=(
                    f"Ollama sta rispondendo troppo lentamente con '{self.model_id}'. "
                    "I comandi file rapidi funzionano, ma per la chat libera conviene usare un modello piu' leggero "
                    "oppure aumentare OLLAMA_TIMEOUT."
                ),
            )
        except requests.exceptions.HTTPError as e:
            dettagli = ""
            try:
                dettagli = e.response.text[:160]
            except Exception:
                pass
            return AIAction(
                azione="messaggio",
                testo_risposta=(
                    f"Errore Ollama: {e.response.status_code}. "
                    f"Controlla che il modello '{self.model_id}' sia installato con: "
                    f"ollama pull {self.model_id}. {dettagli}"
                ),
            )
        except Exception as e:
            return AIAction(
                azione="messaggio",
                testo_risposta=f"Errore durante la risposta locale: {str(e)[:120]}",
            )

    def _risposta_chat(self, comando_utente, file_context=None):
        contesto = f"\n\nFile caricato:\n{file_context}" if file_context else ""
        prompt = (
            "Sei TommyAI, assistente locale in italiano. "
            "Rispondi in modo breve, pratico e chiaro. "
            "Se c'e' un file caricato, usa solo il contesto fornito senza inventare."
            f"{contesto}\n\nUtente: {comando_utente}\nTommyAI:"
        )
        try:
            text = self._generate_ollama(prompt, json_mode=False, num_predict=180).strip()
            return AIAction(azione="messaggio", testo_risposta=text or "Non ho ricevuto una risposta utile.")
        except requests.exceptions.Timeout:
            return AIAction(
                azione="messaggio",
                testo_risposta=f"Ollama e' lento con '{self.model_id}'. Riprova: ora il modello dovrebbe restare caldo.",
            )
        except Exception as e:
            return AIAction(azione="messaggio", testo_risposta=f"Errore chat locale: {str(e)[:120]}")

    def _generate_ollama(self, prompt, json_mode=True, num_predict=220):
        payload = {
            "model": self.model_id,
            "prompt": prompt,
            "stream": False,
            "keep_alive": "10m",
            "options": {
                "temperature": 0.1,
                "top_p": 0.9,
                "num_predict": num_predict,
                "num_ctx": 4096,
                "repeat_penalty": 1.1,
            },
        }
        if json_mode and self.use_json_format:
            payload["format"] = "json"

        response = requests.post(
            f"{self.ollama_url}/api/generate",
            json=payload,
            timeout=self.timeout,
        )

        # Alcuni modelli/runner Ollama vecchi non supportano bene format=json:
        # ritentiamo senza formato rigido, ma tenendo il prompt JSON-only.
        if response.status_code >= 500 and "format" in payload:
            payload.pop("format", None)
            response = requests.post(
                f"{self.ollama_url}/api/generate",
                json=payload,
                timeout=self.timeout,
            )

        response.raise_for_status()
        return response.json().get("response", "")

    def _parse_json_or_message(self, raw_text):
        try:
            return self._parse_json(raw_text)
        except Exception:
            text = raw_text.strip()
            if not text:
                text = "Non ho ricevuto una risposta utile dal modello locale."
            return {
                "azione": "messaggio",
                "testo_risposta": text[:1200],
            }

    def _parse_json(self, raw_text):
        text = raw_text.strip()
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            match = re.search(r"\{.*\}", text, re.DOTALL)
            if not match:
                raise ValueError("Ollama non ha restituito JSON valido")
            return json.loads(match.group(0))

    def _build_action(self, data):
        if not isinstance(data, dict):
            raise ValueError("Risposta JSON non valida")

        allowed_actions = {
            "vai_a",
            "crea_file",
            "crea_cartella",
            "elimina",
            "analizza_cartella",
            "suggerisci_organizzazione",
            "organizza_cartella",
            "trova_doppioni",
            "pulizia_temp",
            "messaggio",
        }
        data.setdefault("azione", "messaggio")
        data.setdefault("testo_risposta", "")

        if data["azione"] not in allowed_actions:
            data["azione"] = "messaggio"
            data["testo_risposta"] = data["testo_risposta"] or "Non ho capito il comando."

        if hasattr(AIAction, "model_validate"):
            return AIAction.model_validate(data)
        return AIAction.parse_obj(data)

    def _coerenza_azione_modello(self, action, comando_utente):
        testo = comando_utente.lower()
        action_keywords = {
            "vai_a": ("vai", "spostati", "entra", "apri", "cartella"),
            "crea_file": ("crea", "nuovo", "file"),
            "crea_cartella": ("crea", "nuova", "cartella"),
            "elimina": ("elimina", "cancella", "rimuovi", "butta"),
            "analizza_cartella": ("analizza", "scansiona", "report", "statistiche"),
            "suggerisci_organizzazione": ("organizza", "riorganizza", "ordina", "sistema"),
            "organizza_cartella": ("organizza", "riorganizza", "ordina", "sistema", "sposta"),
            "trova_doppioni": ("doppion", "duplicat"),
            "pulizia_temp": ("pulisci", "pulizia", "cache", "temporanei", "temp"),
        }

        keywords = action_keywords.get(action.azione)
        if keywords and not any(keyword in testo for keyword in keywords):
            action.azione = "messaggio"
            action.nome = None
            action.contenuto = None
            action.percorso = None

        return action

    def _azione_locale(self, comando_utente, file_context=None):
        testo = comando_utente.lower().strip()

        if testo in {"ciao", "hey", "hei", "salve", "buongiorno", "buonasera"}:
            return AIAction(
                azione="messaggio",
                testo_risposta=(
                    "Ciao! Posso analizzare cartelle, organizzare file, cercare doppioni, "
                    "caricare file nella chat e rispondere alle tue domande."
                ),
            )

        if any(frase in testo for frase in ("cosa sai fare", "aiuto", "help", "comandi")):
            return AIAction(
                azione="messaggio",
                testo_risposta=(
                    "Comandi veloci: 'vai in Downloads', 'analizza questa cartella', "
                    "'organizza questa cartella', 'organizza davvero questa cartella', "
                    "'trova doppioni', 'stima pulizia temp', 'crea cartella nome', "
                    "'crea file nome.txt', 'elimina nome'. Puoi anche caricare un file e chiedermi di spiegarlo."
                ),
            )

        match_vai = re.search(
            r"\b(?:vai|spostati|entra|apri)\s+(?:a|in|nella|nel|su)?\s*([a-zA-Z0-9_ .:\\/~-]+)",
            testo,
        )
        if match_vai:
            percorso = match_vai.group(1).strip()
            percorso = re.sub(r"\b(?:cartella|folder)$", "", percorso).strip()
            if percorso:
                return AIAction(
                    azione="vai_a",
                    percorso=percorso,
                    testo_risposta=f"Mi sposto in: {percorso}",
                )

        match_crea_cartella = re.search(r"\b(?:crea|nuova|genera)\s+(?:una\s+)?cartella\s+(.+)$", testo)
        if match_crea_cartella:
            nome = self._pulisci_nome(match_crea_cartella.group(1))
            if nome:
                return AIAction(
                    azione="crea_cartella",
                    nome=nome,
                    testo_risposta=f"Creo la cartella '{nome}'.",
                )

        match_crea_file = re.search(r"\b(?:crea|nuovo|genera)\s+(?:un\s+)?file\s+(.+)$", testo)
        if match_crea_file:
            nome = self._pulisci_nome(match_crea_file.group(1))
            if nome:
                return AIAction(
                    azione="crea_file",
                    nome=nome,
                    contenuto="",
                    testo_risposta=f"Creo il file '{nome}'.",
                )

        match_elimina = re.search(r"\b(?:elimina|cancella|rimuovi|butta)\s+(.+)$", testo)
        if match_elimina and not any(parola in testo for parola in ("doppion", "duplicat")):
            nome = self._pulisci_nome(match_elimina.group(1))
            nome = re.sub(r"^(?:il|lo|la|un|una|file|cartella)\s+", "", nome).strip()
            if nome:
                return AIAction(
                    azione="elimina",
                    nome=nome,
                    testo_risposta=f"Sposto '{nome}' nel cestino sicuro.",
                )

        if file_context and any(
            frase in testo
            for frase in (
                "questo file",
                "file caricato",
                "documento caricato",
                "riassumi",
                "spiega",
                "cosa contiene",
                "fammi domande",
            )
        ):
            return None

        if any(parola in testo for parola in ("doppion", "duplicat")):
            return AIAction(
                azione="trova_doppioni",
                testo_risposta="Cerco eventuali file duplicati nella cartella corrente.",
            )

        if any(parola in testo for parola in ("analizza", "scansiona", "report", "statistiche")):
            return AIAction(
                azione="analizza_cartella",
                testo_risposta="Analizzo la cartella corrente.",
            )

        if any(parola in testo for parola in ("organizza", "riorganizza", "ordina", "sistema")):
            applica = any(
                parola in testo
                for parola in ("davvero", "applica", "esegui", "sposta", "fallo", "procedi")
            )
            return AIAction(
                azione="organizza_cartella" if applica else "suggerisci_organizzazione",
                testo_risposta=(
                    "Organizzo i file nella cartella corrente."
                    if applica
                    else "Preparo un'anteprima dell'organizzazione, senza spostare file."
                ),
            )

        if any(parola in testo for parola in ("pulisci", "pulizia", "cache", "temporanei", "temp")):
            return AIAction(
                azione="pulizia_temp",
                testo_risposta="Stimo i file temporanei e la cache rimovibile.",
            )

        return None

    def _sembra_comando_file(self, comando_utente):
        testo = comando_utente.lower()
        keywords = (
            "vai",
            "spostati",
            "entra",
            "apri",
            "crea",
            "nuovo",
            "nuova",
            "elimina",
            "cancella",
            "rimuovi",
            "butta",
            "analizza",
            "scansiona",
            "report",
            "statistiche",
            "organizza",
            "riorganizza",
            "ordina",
            "sistema",
            "doppion",
            "duplicat",
            "pulisci",
            "pulizia",
            "cache",
            "temporanei",
            "temp",
        )
        return any(keyword in testo for keyword in keywords)

    def _pulisci_nome(self, testo):
        nome = testo.strip().strip("\"'")
        nome = re.sub(r"\s+(?:qui|nella cartella corrente|in questa cartella)$", "", nome).strip()
        return nome
