import os
import hashlib
from pathlib import Path
from collections import defaultdict
from datetime import datetime, timedelta

class FileAnalyzer:
    """Analizza file system per duplicati, stats, e categorizzazione"""

    def __init__(self, root_path="."):
        self.root_path = Path(root_path)
        self.file_hashes = defaultdict(list)
        self.file_stats = {}
        self.file_categories = defaultdict(list)

    def scan_directory(self, max_depth=None):
        """Scansione veloce directory - ritorna lista file con metadata"""
        files_data = []

        try:
            for entry in os.scandir(self.root_path):
                if entry.is_file(follow_symlinks=False):
                    try:
                        stat = entry.stat()
                        files_data.append({
                            "path": entry.path,
                            "name": entry.name,
                            "size": stat.st_size,
                            "modified": stat.st_mtime,
                            "created": stat.st_ctime,
                            "ext": Path(entry.name).suffix.lower(),
                        })
                    except:
                        pass
                elif entry.is_dir(follow_symlinks=False) and (max_depth is None or max_depth > 0):
                    sub_analyzer = FileAnalyzer(entry.path)
                    files_data.extend(sub_analyzer.scan_directory(max_depth - 1 if max_depth else None))
        except PermissionError:
            pass

        return files_data

    def find_duplicates(self, files_data, chunk_size=8192):
        """Rileva file duplicati usando SHA256, ma hasha solo file con stessa dimensione."""
        duplicates = defaultdict(list)
        by_size = defaultdict(list)

        for file_info in files_data:
            by_size[file_info.get("size", 0)].append(file_info)

        candidates = [
            file_info
            for same_size_files in by_size.values()
            if len(same_size_files) > 1
            for file_info in same_size_files
        ]

        for file_info in candidates:
            try:
                file_hash = self._hash_file(file_info["path"], chunk_size)
                if file_hash:
                    duplicates[file_hash].append(file_info)
            except:
                pass

        # Ritorna solo i veri duplicati (hash con 2+ file)
        return {k: v for k, v in duplicates.items() if len(v) > 1}

    def _hash_file(self, filepath, chunk_size=8192):
        """Calcola SHA256 di un file"""
        sha256_hash = hashlib.sha256()
        try:
            with open(filepath, "rb") as f:
                for chunk in iter(lambda: f.read(chunk_size), b""):
                    sha256_hash.update(chunk)
            return sha256_hash.hexdigest()
        except:
            return None

    def categorize_files(self, files_data):
        """Classifica file per tipo/estensione"""
        categories = {
            "images": [".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp"],
            "documents": [".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt"],
            "videos": [".mp4", ".avi", ".mkv", ".mov", ".flv"],
            "audio": [".mp3", ".wav", ".flac", ".aac", ".m4a"],
            "archives": [".zip", ".rar", ".7z", ".tar", ".gz"],
            "code": [".py", ".js", ".java", ".cpp", ".c", ".html", ".css"],
            "other": []
        }

        categorized = defaultdict(list)
        for file_info in files_data:
            ext = file_info["ext"]
            found = False
            for category, extensions in categories.items():
                if ext in extensions:
                    categorized[category].append(file_info)
                    found = True
                    break
            if not found:
                categorized["other"].append(file_info)

        return categorized

    def get_file_stats(self, files_data):
        """Calcola statistiche sui file"""
        total_size = sum(f["size"] for f in files_data)
        total_files = len(files_data)

        # File per categoria
        categories = self.categorize_files(files_data)
        category_stats = {
            cat: {
                "count": len(files),
                "size": sum(f["size"] for f in files)
            }
            for cat, files in categories.items()
        }

        # File più grandi
        largest_files = sorted(files_data, key=lambda x: x["size"], reverse=True)[:10]

        # File più vecchi
        now = datetime.now().timestamp()
        old_threshold = now - (365 * 24 * 3600)  # 1 anno fa
        old_files = [f for f in files_data if f["modified"] < old_threshold]

        return {
            "total_files": total_files,
            "total_size": total_size,
            "total_size_mb": total_size / (1024 * 1024),
            "categories": category_stats,
            "largest_files": largest_files,
            "old_files_count": len(old_files),
            "old_files": old_files
        }

    def generate_report(self, files_data, include_duplicates=True):
        """Genera report completo"""
        stats = self.get_file_stats(files_data)
        duplicates = self.find_duplicates(files_data) if include_duplicates else {}

        # Calcola spazio risparmiabile da doppioni
        saveable_size = 0
        dup_count = 0
        for hash_val, file_list in duplicates.items():
            # Mantieni 1 copia, togli le altre
            for f in file_list[1:]:
                saveable_size += f["size"]
                dup_count += 1

        report = {
            "timestamp": datetime.now().isoformat(),
            "total_files": stats["total_files"],
            "total_size_mb": stats["total_size_mb"],
            "duplicate_count": dup_count,
            "saveable_mb": saveable_size / (1024 * 1024),
            "categories": stats["categories"],
            "old_files_count": stats["old_files_count"],
            "duplicates": duplicates
        }

        return report
