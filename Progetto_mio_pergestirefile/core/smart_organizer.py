import os
from pathlib import Path

class SmartOrganizer:
    """Organizza file in cartelle intelligenti"""

    # Mappature intelligenti per categoria → cartelle
    FOLDER_MAPPING = {
        "images": "Pictures",
        "documents": "Documents",
        "videos": "Videos",
        "audio": "Music",
        "archives": "Archives",
        "code": "Code",
        "downloads": "Downloads"
    }

    EXTENSION_MAPPING = {
        ".jpg": "images",
        ".jpeg": "images",
        ".png": "images",
        ".gif": "images",
        ".bmp": "images",
        ".webp": "images",
        ".svg": "images",
        ".ico": "images",

        ".pdf": "documents",
        ".doc": "documents",
        ".docx": "documents",
        ".xls": "documents",
        ".xlsx": "documents",
        ".ppt": "documents",
        ".pptx": "documents",
        ".txt": "documents",
        ".odt": "documents",

        ".mp4": "videos",
        ".avi": "videos",
        ".mkv": "videos",
        ".mov": "videos",
        ".flv": "videos",
        ".wmv": "videos",
        ".webm": "videos",

        ".mp3": "audio",
        ".wav": "audio",
        ".flac": "audio",
        ".aac": "audio",
        ".m4a": "audio",
        ".ogg": "audio",

        ".zip": "archives",
        ".rar": "archives",
        ".7z": "archives",
        ".tar": "archives",
        ".gz": "archives",

        ".py": "code",
        ".js": "code",
        ".java": "code",
        ".cpp": "code",
        ".c": "code",
        ".html": "code",
        ".css": "code",
        ".json": "code",
        ".xml": "code",
    }

    def __init__(self, root_path="."):
        self.root_path = Path(root_path)

    def suggest_organization(self, files_data):
        """Suggerisce come organizzare i file"""
        suggestions = []

        for file_info in files_data:
            ext = file_info["ext"]
            category = self.EXTENSION_MAPPING.get(ext, "other")
            target_folder = self.FOLDER_MAPPING.get(category, "Miscellaneous")

            file_path = Path(file_info["path"])
            current_folder = file_path.parent.name

            # Se non è già nella cartella giusta, suggerisci
            if current_folder != target_folder:
                suggestions.append({
                    "file": file_info["path"],
                    "current_folder": current_folder,
                    "suggested_folder": target_folder,
                    "category": category,
                    "reason": f"File {category}"
                })

        return suggestions

    def create_folders(self, folder_names):
        """Crea cartelle necessarie"""
        created = []
        errors = []

        for folder_name in folder_names:
            try:
                folder_path = self.root_path / folder_name
                folder_path.mkdir(parents=True, exist_ok=True)
                created.append(str(folder_path))
            except Exception as e:
                errors.append({"folder": folder_name, "error": str(e)})

        return {"created": created, "errors": errors}

    def move_file(self, source_file, dest_folder):
        """Sposta file in cartella destinazione in modo sicuro"""
        try:
            source_path = Path(source_file)
            if not source_path.exists():
                return {"success": False, "error": "File non trovato"}

            dest_path = self.root_path / dest_folder
            dest_path.mkdir(parents=True, exist_ok=True)

            dest_file = dest_path / source_path.name

            # Se file esiste nella destinazione, rinomina source
            if dest_file.exists():
                stem = source_path.stem
                suffix = source_path.suffix
                counter = 1
                while dest_file.exists():
                    dest_file = dest_path / f"{stem}_{counter}{suffix}"
                    counter += 1

            source_path.rename(dest_file)

            return {
                "success": True,
                "source": str(source_path),
                "destination": str(dest_file)
            }

        except Exception as e:
            return {"success": False, "error": str(e)}

    def auto_sort(self, files_data, dry_run=True):
        """Organizza automaticamente tutti i file"""
        suggestions = self.suggest_organization(files_data)

        if not suggestions:
            return {"message": "Tutti i file sono già organizzati!"}

        # Crea cartelle necessarie
        target_folders = set(s["suggested_folder"] for s in suggestions)
        self.create_folders(target_folders)

        # Sposta file
        results = {"moved": [], "failed": [], "skipped": 0}

        for suggestion in suggestions:
            if dry_run:
                results["skipped"] += 1
            else:
                result = self.move_file(suggestion["file"], suggestion["suggested_folder"])
                if result["success"]:
                    results["moved"].append(result)
                else:
                    results["failed"].append(result)

        if dry_run:
            results["message"] = f"[DRY RUN] Sposterei {len(suggestions)} file"

        return results

    def get_organization_stats(self, files_data):
        """Statistiche organizzazione attuale"""
        suggestions = self.suggest_organization(files_data)
        organized_count = len(files_data) - len(suggestions)
        organization_percent = (organized_count / len(files_data) * 100) if files_data else 0

        return {
            "total_files": len(files_data),
            "well_organized": organized_count,
            "disorganized": len(suggestions),
            "organization_percent": round(organization_percent, 2),
            "improvement_potential": suggestions
        }
