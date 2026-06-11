import os
import shutil
from pathlib import Path
from datetime import datetime

class DuplicateHandler:
    """Gestisce rilevamento e eliminazione sicura di doppioni"""

    def __init__(self, root_path="."):
        self.root_path = Path(root_path)
        self.backup_folder = Path(root_path) / ".tommyai_backup"

    def recommend_delete(self, duplicates):
        """Raccomanda quali file eliminare dai doppioni"""
        recommendations = []

        for hash_val, file_list in duplicates.items():
            if len(file_list) <= 1:
                continue

            # Strategia: mantieni il file più nuovo, togli gli altri
            sorted_files = sorted(file_list, key=lambda x: x["modified"], reverse=True)
            keep_file = sorted_files[0]
            delete_files = sorted_files[1:]

            for del_file in delete_files:
                recommendations.append({
                    "file": del_file["path"],
                    "reason": "duplicate",
                    "keep_file": keep_file["path"],
                    "size": del_file["size"],
                    "size_mb": del_file["size"] / (1024 * 1024)
                })

        return recommendations

    def create_backup(self):
        """Crea cartella backup per file eliminati"""
        self.backup_folder.mkdir(exist_ok=True)
        return self.backup_folder

    def safe_delete(self, file_path, create_backup=True):
        """Elimina file in modo sicuro con backup"""
        try:
            file_path = Path(file_path)

            if not file_path.exists():
                return {"success": False, "error": "File non trovato"}

            if create_backup:
                backup_dir = self.create_backup()
                backup_file = backup_dir / file_path.name
                # Se esiste già backup con nome uguale, rinomina
                counter = 1
                stem = backup_file.stem
                suffix = backup_file.suffix
                while backup_file.exists():
                    backup_file = backup_dir / f"{stem}_{counter}{suffix}"
                    counter += 1

                shutil.copy2(file_path, backup_file)

            # Elimina originale
            os.remove(file_path)

            return {
                "success": True,
                "deleted_file": str(file_path),
                "backup_location": str(backup_dir / backup_file.name) if create_backup else None
            }

        except Exception as e:
            return {"success": False, "error": str(e)}

    def batch_delete_duplicates(self, recommendations, create_backup=True):
        """Elimina batch di doppioni consigliati"""
        results = {
            "deleted": [],
            "failed": [],
            "total_freed_mb": 0
        }

        for rec in recommendations:
            result = self.safe_delete(rec["file"], create_backup)
            if result["success"]:
                results["deleted"].append(rec["file"])
                results["total_freed_mb"] += rec["size_mb"]
            else:
                results["failed"].append({
                    "file": rec["file"],
                    "error": result["error"]
                })

        return results

    def find_similar_files(self, files_data, similarity_threshold=0.8):
        """Rileva file simili (fuzzy matching su nome)"""
        from difflib import SequenceMatcher

        similar_groups = []
        processed = set()

        for i, file1 in enumerate(files_data):
            if file1["path"] in processed:
                continue

            group = [file1]
            for file2 in files_data[i + 1:]:
                if file2["path"] in processed:
                    continue

                # Confronta nome file
                ratio = SequenceMatcher(None, file1["name"], file2["name"]).ratio()
                if ratio >= similarity_threshold:
                    group.append(file2)
                    processed.add(file2["path"])

            if len(group) > 1:
                similar_groups.append(group)
                processed.add(file1["path"])

        return similar_groups
