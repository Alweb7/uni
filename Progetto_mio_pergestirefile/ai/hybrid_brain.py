import requests
import json
from datetime import datetime

class HybridBrain:
    """Backend ibrido: Ollama (veloce) + Claude (accurato)"""

    def __init__(self, claude_api_key=None, ollama_url="http://localhost:11434"):
        self.ollama_url = ollama_url
        self.claude_api_key = claude_api_key
        self.ollama_available = self._check_ollama()

    def _check_ollama(self):
        """Verifica se Ollama è online"""
        try:
            response = requests.get(f"{self.ollama_url}/api/tags", timeout=2)
            return response.status_code == 200
        except:
            return False

    # ============ OLLAMA (VELOCE) ============

    def quick_classify(self, file_name, file_ext, file_size_mb):
        """Classificazione veloce file via Ollama"""
        if not self.ollama_available:
            return {"category": "unknown", "confidence": 0}

        prompt = f"""File: {file_name}{file_ext} ({file_size_mb:.1f}MB)
Classifica rapidamente in: images, documents, videos, audio, archives, code, other.
Rispondi solo con categoria."""

        try:
            response = requests.post(
                f"{self.ollama_url}/api/generate",
                json={"model": "llama2", "prompt": prompt, "stream": False},
                timeout=10
            )

            if response.status_code == 200:
                result = response.json()
                category = result.get("response", "").strip().lower().split("\n")[0]
                return {"category": category, "source": "ollama", "fast": True}
        except:
            pass

        return {"category": "unknown", "confidence": 0}

    def quick_sort_suggestion(self, files_list):
        """Raccomandazione ordinamento veloce da Ollama"""
        if not self.ollama_available or not files_list:
            return {"suggestions": [], "source": "offline"}

        file_summary = ", ".join([f"{f['name']}" for f in files_list[:5]])
        prompt = f"""Questi file: {file_summary}...
Suggeri cartelle di destinazione tipo: Pictures, Documents, Videos, etc.
Rispondi in JSON: {{"suggestions": [{{"file": "...", "folder": "..."}}]}}"""

        try:
            response = requests.post(
                f"{self.ollama_url}/api/generate",
                json={"model": "llama2", "prompt": prompt, "stream": False},
                timeout=10
            )

            if response.status_code == 200:
                result = response.json()
                try:
                    suggestions = json.loads(result.get("response", "{}"))
                    return {"suggestions": suggestions, "source": "ollama"}
                except:
                    pass
        except:
            pass

        return {"suggestions": [], "source": "error"}

    # ============ CLAUDE (ACCURATO) ============

    def smart_analyze(self, analysis_data):
        """Analisi accurata tramite Claude"""
        if not self.claude_api_key:
            return {"insights": "Claude non configurato", "source": "offline"}

        prompt = f"""Analizza questi dati di file management:
{json.dumps(analysis_data, indent=2)}

Fornisci insights e raccomandazioni intelligenti su:
- Organizzazione suggerita
- Spazio liberabile
- Rischi potenziali
Rispondi concisamente."""

        try:
            from anthropic import Anthropic
            client = Anthropic(api_key=self.claude_api_key)
            response = client.messages.create(
                model="claude-3-5-sonnet-20241022",
                max_tokens=500,
                messages=[{"role": "user", "content": prompt}]
            )
            return {
                "insights": response.content[0].text,
                "source": "claude"
            }
        except Exception as e:
            return {"insights": f"Errore Claude: {str(e)}", "source": "error"}

    def get_recommendation(self, file_data, use_claude=False):
        """Raccomandazione ibrida: Ollama per velocità, Claude per accuratezza"""
        if use_claude and self.claude_api_key:
            return self.smart_analyze(file_data)
        else:
            return self.quick_classify(
                file_data.get("name", ""),
                file_data.get("ext", ""),
                file_data.get("size", 0) / (1024 * 1024)
            )

    def generate_insights(self, report_data):
        """Genera insights dal report (usa Claude se disponibile)"""
        if self.claude_api_key:
            return self.smart_analyze(report_data)

        # Fallback: semplici insights locali
        insights = []
        if report_data.get("duplicate_count", 0) > 0:
            insights.append(f"⚠️ {report_data['duplicate_count']} file duplicati trovati!")
        if report_data.get("saveable_mb", 0) > 100:
            insights.append(f"💾 Puoi liberare {report_data['saveable_mb']:.0f}MB!")
        if report_data.get("old_files_count", 0) > 10:
            insights.append(f"🗂️ {report_data['old_files_count']} file > 1 anno - considera archivio")

        return {
            "insights": "\n".join(insights) if insights else "Nessun problema rilevato",
            "source": "local"
        }

    def health_check(self):
        """Verifica stato dei backend"""
        return {
            "ollama": self.ollama_available,
            "claude": bool(self.claude_api_key),
            "timestamp": datetime.now().isoformat()
        }
