import json
import os
import shutil
import time
from pathlib import Path

from core import CacheManager, DuplicateHandler, FileAnalyzer, SmartOrganizer

class FileLogic:
    APP_ROOT = Path(__file__).resolve().parent

    @staticmethod
    def esegui_azione(res, percorso_attuale):
        try:
            nuovo_percorso = percorso_attuale

            if res.azione == "crea_cartella" and res.nome:
                path = FileLogic._target_sicuro(percorso_attuale, res.nome)
                os.makedirs(path, exist_ok=True)

            elif res.azione == "crea_file" and res.nome:
                path = FileLogic._target_sicuro(percorso_attuale, res.nome)
                if os.path.exists(path):
                    raise FileExistsError(f"Il file esiste gia': {path}")
                with open(path, "w", encoding="utf-8") as f:
                    f.write(res.contenuto if res.contenuto else "")

            elif res.azione == "vai_a" and res.percorso:
                p = FileLogic._risolvi_percorso(res.percorso, percorso_attuale)
                if not os.path.exists(p):
                    raise FileNotFoundError(f"Cartella non trovata: {p}")
                if not os.path.isdir(p):
                    raise NotADirectoryError(f"{p} non è una cartella")
                nuovo_percorso = p
                os.chdir(nuovo_percorso)

            elif res.azione == "elimina" and res.nome:
                target = FileLogic._target_sicuro(percorso_attuale, res.nome)
                if not os.path.exists(target):
                    raise FileNotFoundError(f"File/cartella non trovata: {target}")
                trash_path = FileLogic._sposta_nel_cestino_sicuro(target, percorso_attuale)
                res.testo_risposta = f"Ho spostato '{res.nome}' nel cestino sicuro: {trash_path}"

            elif res.azione == "analizza_cartella":
                res.testo_risposta = FileLogic._analizza_cartella(percorso_attuale)

            elif res.azione == "suggerisci_organizzazione":
                res.testo_risposta = FileLogic._organizzazione(percorso_attuale, dry_run=True)

            elif res.azione == "organizza_cartella":
                res.testo_risposta = FileLogic._organizzazione(percorso_attuale, dry_run=False)

            elif res.azione == "trova_doppioni":
                res.testo_risposta = FileLogic._trova_doppioni(percorso_attuale)

            elif res.azione == "pulizia_temp":
                res.testo_risposta = FileLogic._stima_pulizia(percorso_attuale)

            return nuovo_percorso
        except Exception as e:
            raise e

    @staticmethod
    def _scan(percorso_attuale, max_depth=1):
        analyzer = FileAnalyzer(percorso_attuale)
        files = analyzer.scan_directory(max_depth=max_depth)
        return analyzer, files

    @staticmethod
    def _risolvi_percorso(percorso, percorso_attuale):
        testo = percorso.strip().strip('"')
        aliases = {
            "desktop": Path.home() / "Desktop",
            "scrivania": Path.home() / "Desktop",
            "download": Path.home() / "Downloads",
            "downloads": Path.home() / "Downloads",
            "scaricati": Path.home() / "Downloads",
            "documenti": Path.home() / "Documents",
            "documents": Path.home() / "Documents",
            "immagini": Path.home() / "Pictures",
            "pictures": Path.home() / "Pictures",
            "musica": Path.home() / "Music",
            "music": Path.home() / "Music",
            "video": Path.home() / "Videos",
            "videos": Path.home() / "Videos",
            "home": Path.home(),
        }

        alias = aliases.get(testo.lower())
        if alias:
            return str(alias)

        expanded = Path(os.path.expanduser(testo))
        if expanded.is_absolute():
            return str(expanded)
        return str(Path(percorso_attuale) / expanded)

    @staticmethod
    def _target_sicuro(percorso_attuale, nome):
        if not nome or not nome.strip():
            raise ValueError("Nome file/cartella mancante")

        base = Path(percorso_attuale).resolve()
        target = (base / nome.strip().strip('"')).resolve()

        if not FileLogic._is_relative_to(target, base):
            raise PermissionError("Operazione bloccata: il percorso esce dalla cartella corrente")

        if FileLogic._percorso_protetto(target):
            raise PermissionError("Operazione bloccata: percorso di sistema o protetto")

        trash_dir = FileLogic.cestino_path()
        if FileLogic._is_relative_to(base, trash_dir) or FileLogic._is_relative_to(target, trash_dir):
            raise PermissionError("Operazione bloccata: gestisci il cestino manualmente dal collegamento Desktop")

        if target.name in {".tommyai_trash", ".git", "Windows", "System32"}:
            raise PermissionError("Operazione bloccata: elemento protetto")

        return str(target)

    @staticmethod
    def _is_relative_to(path, base):
        try:
            path.relative_to(base)
            return True
        except ValueError:
            return False

    @staticmethod
    def _percorso_protetto(path):
        target = Path(path).resolve()
        home = Path.home().resolve()

        protected_candidates = [
            os.environ.get("SystemRoot"),
            os.environ.get("WINDIR"),
            os.environ.get("ProgramFiles"),
            os.environ.get("ProgramFiles(x86)"),
            os.environ.get("ProgramData"),
            os.environ.get("LOCALAPPDATA"),
            os.environ.get("APPDATA"),
            os.environ.get("USERPROFILE") and str(home / "AppData"),
        ]

        protected_roots = []
        for candidate in protected_candidates:
            if candidate:
                try:
                    protected_roots.append(Path(candidate).resolve())
                except Exception:
                    pass

        if target.anchor and str(target) == target.anchor:
            return True

        return any(FileLogic._is_relative_to(target, root) for root in protected_roots)

    @staticmethod
    def _sposta_nel_cestino_sicuro(target, percorso_attuale):
        target_path = Path(target).resolve()

        if FileLogic._percorso_protetto(target_path):
            raise PermissionError("Eliminazione bloccata: file/cartella di sistema")

        trash_dir = FileLogic.cestino_path()
        if FileLogic._is_relative_to(target_path, trash_dir):
            raise PermissionError("Eliminazione bloccata: il file e' gia' nel cestino TommyAI")

        if FileLogic._sembra_progetto(target_path) or target_path.name in {"main.py", "ai_brain.py", "file_ops.py"}:
            raise PermissionError("Eliminazione bloccata: elemento del progetto protetto")

        trash_dir.mkdir(exist_ok=True)

        timestamp = time.strftime("%Y%m%d_%H%M%S")
        destination = trash_dir / f"{timestamp}_{target_path.name}"
        if target_path.is_dir():
            destination = trash_dir / f"{timestamp}_{target_path.name}"

        counter = 1
        original_destination = destination
        while destination.exists():
            destination = original_destination.with_name(f"{original_destination.name}_{counter}")
            counter += 1

        shutil.move(str(target_path), str(destination))
        FileLogic._scrivi_manifest_cestino(target_path, destination)
        return str(destination)

    @staticmethod
    def cestino_path():
        return (FileLogic.APP_ROOT / ".tommyai_trash").resolve()

    @staticmethod
    def inizializza_cestino():
        trash_dir = FileLogic.cestino_path()
        trash_dir.mkdir(exist_ok=True)
        readme = trash_dir / "README_RECUPERO.txt"
        if not readme.exists():
            readme.write_text(
                "Cestino sicuro TommyAI\n\n"
                "Qui finiscono i file spostati con il comando elimina.\n"
                "Per recuperarli, spostali manualmente nella cartella desiderata.\n"
                "Il file manifest.jsonl contiene il percorso originale di ogni elemento.\n",
                encoding="utf-8",
            )
        return trash_dir

    @staticmethod
    def _scrivi_manifest_cestino(original_path, trash_path):
        manifest = FileLogic.cestino_path() / "manifest.jsonl"
        record = {
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
            "original_path": str(original_path),
            "trash_path": str(trash_path),
            "type": "directory" if Path(trash_path).is_dir() else "file",
        }
        with open(manifest, "a", encoding="utf-8") as handle:
            handle.write(json.dumps(record, ensure_ascii=False) + "\n")

    @staticmethod
    def _format_size(mb):
        if mb >= 1024:
            return f"{mb / 1024:.2f} GB"
        return f"{mb:.2f} MB"

    @staticmethod
    def _analizza_cartella(percorso_attuale):
        analyzer, files = FileLogic._scan(percorso_attuale, max_depth=0)
        report = analyzer.generate_report(files, include_duplicates=False)

        righe = [
            f"Analisi di: {percorso_attuale}",
            f"File trovati: {report['total_files']}",
            f"Spazio totale: {FileLogic._format_size(report['total_size_mb'])}",
            "Doppioni: usa 'trova doppioni' per una scansione dedicata",
            f"File vecchi oltre 1 anno: {report['old_files_count']}",
            "",
            "Categorie:",
        ]

        for categoria, stats in report["categories"].items():
            if stats["count"]:
                righe.append(
                    f"- {categoria}: {stats['count']} file, "
                    f"{FileLogic._format_size(stats['size'] / (1024 * 1024))}"
                )

        return "\n".join(righe)

    @staticmethod
    def _organizzazione(percorso_attuale, dry_run=True):
        if FileLogic._percorso_protetto(percorso_attuale):
            return (
                "Questa cartella e' in un percorso di sistema o applicazione. "
                "Per sicurezza non la riorganizzo automaticamente."
            )

        if not dry_run and FileLogic._sembra_progetto(percorso_attuale):
            return (
                "Questa cartella sembra un progetto software, quindi non sposto i file in automatico. "
                "Posso fare l'anteprima, oppure puoi andare in Downloads/Desktop/Documenti e ordinare quella cartella."
            )

        _, files = FileLogic._scan(percorso_attuale, max_depth=0)
        organizer = SmartOrganizer(percorso_attuale)
        result = organizer.auto_sort(files, dry_run=dry_run)

        if dry_run:
            suggestions = organizer.suggest_organization(files)
            if not suggestions:
                return "La cartella sembra gia' organizzata: non ho trovato file da spostare."

            righe = [
                f"Anteprima organizzazione: sposterei {len(suggestions)} file.",
                "Prime proposte:",
            ]
            for suggestion in suggestions[:10]:
                nome_file = Path(suggestion["file"]).name
                righe.append(f"- {nome_file} -> {suggestion['suggested_folder']}")

            if len(suggestions) > 10:
                righe.append(f"...e altri {len(suggestions) - 10} file.")

            righe.append("")
            righe.append("Per applicare davvero, scrivi: organizza davvero questa cartella")
            return "\n".join(righe)

        moved = len(result.get("moved", []))
        failed = len(result.get("failed", []))
        if moved == 0 and failed == 0:
            return result.get("message", "Nessun file da spostare.")

        righe = [f"Organizzazione completata: {moved} file spostati."]
        if failed:
            righe.append(f"Non sono riuscito a spostare {failed} file.")
        return "\n".join(righe)

    @staticmethod
    def _sembra_progetto(percorso_attuale):
        path = Path(percorso_attuale)
        marker_files = {
            "main.py",
            "pyproject.toml",
            "package.json",
            "requirements.txt",
            "Cargo.toml",
            "pom.xml",
            ".gitignore",
        }
        marker_dirs = {".git", "src", "core", "utils", "node_modules", ".venv", "venv"}

        try:
            names = {item.name for item in path.iterdir()}
        except Exception:
            return False

        return bool(names & marker_files) or bool(names & marker_dirs)

    @staticmethod
    def _trova_doppioni(percorso_attuale):
        analyzer, files = FileLogic._scan(percorso_attuale, max_depth=0)
        duplicates = analyzer.find_duplicates(files)

        if not duplicates:
            return "Non ho trovato doppioni nella cartella corrente."

        handler = DuplicateHandler(percorso_attuale)
        recommendations = handler.recommend_delete(duplicates)
        total_mb = sum(rec["size_mb"] for rec in recommendations)

        righe = [
            f"Ho trovato {len(duplicates)} gruppi di doppioni.",
            f"Spazio potenzialmente liberabile: {FileLogic._format_size(total_mb)}",
            "File che potresti rimuovere dopo controllo:",
        ]

        for rec in recommendations[:10]:
            righe.append(f"- {Path(rec['file']).name} (tengo: {Path(rec['keep_file']).name})")

        if len(recommendations) > 10:
            righe.append(f"...e altri {len(recommendations) - 10} file.")

        righe.append("Per sicurezza non elimino doppioni in automatico.")
        return "\n".join(righe)

    @staticmethod
    def _stima_pulizia(percorso_attuale):
        cleanup = CacheManager().estimate_cleanup(percorso_attuale, include_system=False, max_items=500)

        righe = [
            f"Stima pulizia per: {percorso_attuale}",
            f"File temporanei trovati: {len(cleanup['temp_files'])}",
            f"Cartelle cache trovate: {len(cleanup['cache_dirs'])}",
            f"Spazio stimato recuperabile: {FileLogic._format_size(cleanup['estimated_mb'])}",
            "Scansione veloce locale: non includo cache di sistema pesanti.",
            "Per sicurezza questa e' solo una stima: non ho cancellato nulla.",
        ]

        return "\n".join(righe)
