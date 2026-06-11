import hashlib
import mimetypes
import os
from pathlib import Path

from PIL import Image


class FileInspector:
    TEXT_EXTENSIONS = {
        ".txt",
        ".md",
        ".py",
        ".json",
        ".csv",
        ".log",
        ".xml",
        ".html",
        ".css",
        ".js",
        ".yml",
        ".yaml",
        ".ini",
        ".toml",
    }

    MAX_PREVIEW_BYTES = 80_000
    MAX_CONTEXT_CHARS = 12_000

    @staticmethod
    def inspect(path):
        file_path = Path(path).resolve()
        if not file_path.exists():
            raise FileNotFoundError(f"File non trovato: {file_path}")
        if not file_path.is_file():
            raise ValueError("Puoi caricare solo file, non cartelle")

        stat = file_path.stat()
        mime, _ = mimetypes.guess_type(str(file_path))
        data = {
            "path": str(file_path),
            "name": file_path.name,
            "extension": file_path.suffix.lower(),
            "size_bytes": stat.st_size,
            "size_mb": stat.st_size / (1024 * 1024),
            "mime": mime or "unknown",
            "sha256": FileInspector._hash_file(file_path),
            "preview": "",
            "kind": "binary",
            "notes": [],
        }

        if FileInspector._is_image(file_path, data["mime"]):
            FileInspector._inspect_image(file_path, data)
        elif FileInspector._is_text_like(file_path, data["mime"]):
            FileInspector._inspect_text(file_path, data)
        else:
            data["notes"].append("File binario: posso ragionare sui metadati, ma non leggerne il contenuto senza un parser dedicato.")

        data["summary"] = FileInspector.summary(data)
        data["context"] = FileInspector.context_for_ai(data)
        return data

    @staticmethod
    def _hash_file(file_path):
        sha = hashlib.sha256()
        with open(file_path, "rb") as handle:
            for chunk in iter(lambda: handle.read(1024 * 1024), b""):
                sha.update(chunk)
        return sha.hexdigest()

    @staticmethod
    def _is_text_like(file_path, mime):
        if file_path.suffix.lower() in FileInspector.TEXT_EXTENSIONS:
            return True
        return bool(mime and (mime.startswith("text/") or mime in {"application/json", "application/xml"}))

    @staticmethod
    def _is_image(file_path, mime):
        return bool(mime and mime.startswith("image/")) or file_path.suffix.lower() in {".png", ".jpg", ".jpeg", ".webp", ".gif", ".bmp", ".ico"}

    @staticmethod
    def _inspect_text(file_path, data):
        raw = file_path.read_bytes()[: FileInspector.MAX_PREVIEW_BYTES]
        for encoding in ("utf-8", "utf-16", "latin-1"):
            try:
                text = raw.decode(encoding)
                data["kind"] = "text"
                data["encoding"] = encoding
                data["preview"] = text[: FileInspector.MAX_CONTEXT_CHARS]
                data["line_count"] = text.count("\n") + (1 if text else 0)
                if data["size_bytes"] > FileInspector.MAX_PREVIEW_BYTES:
                    data["notes"].append("Anteprima limitata: il file e' grande, ho letto solo la parte iniziale.")
                return
            except UnicodeDecodeError:
                continue

        data["notes"].append("Non sono riuscito a decodificare il testo del file.")

    @staticmethod
    def _inspect_image(file_path, data):
        data["kind"] = "image"
        try:
            with Image.open(file_path) as image:
                data["width"] = image.width
                data["height"] = image.height
                data["mode"] = image.mode
                data["format"] = image.format
        except Exception as exc:
            data["notes"].append(f"Immagine rilevata, ma non leggibile da PIL: {exc}")

    @staticmethod
    def summary(data):
        lines = [
            f"File caricato: {data['name']}",
            f"Tipo: {data['kind']} ({data['mime']})",
            f"Dimensione: {data['size_mb']:.2f} MB",
            f"SHA256: {data['sha256'][:16]}...",
        ]

        if data["kind"] == "text":
            lines.append(f"Righe stimate: {data.get('line_count', 0)}")
            if data.get("preview"):
                preview = data["preview"][:900].strip()
                lines.append("")
                lines.append("Anteprima:")
                lines.append(preview)
        elif data["kind"] == "image":
            size = f"{data.get('width', '?')}x{data.get('height', '?')}"
            lines.append(f"Risoluzione: {size}")
            lines.append(f"Formato: {data.get('format', 'unknown')}")

        if data["notes"]:
            lines.append("")
            lines.extend(data["notes"])

        return "\n".join(lines)

    @staticmethod
    def context_for_ai(data):
        parts = [
            "FILE CARICATO NELLA CHAT",
            f"Nome: {data['name']}",
            f"Percorso: {data['path']}",
            f"Tipo: {data['kind']} / {data['mime']}",
            f"Dimensione MB: {data['size_mb']:.2f}",
            f"SHA256: {data['sha256']}",
        ]

        if data["kind"] == "text":
            parts.append("Contenuto/anteprima:")
            parts.append(data.get("preview", ""))
        elif data["kind"] == "image":
            parts.append(f"Risoluzione: {data.get('width', '?')}x{data.get('height', '?')}")
            parts.append("Nota: posso usare i metadati dell'immagine; per una vera descrizione visiva servirebbe un modello vision.")
        else:
            parts.append("Nota: file binario, disponibile solo analisi metadati.")

        return "\n".join(parts)
