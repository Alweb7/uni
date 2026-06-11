import os
import shutil
from pathlib import Path

class CacheManager:
    """Gestisce pulizia cache, temp files, e file non utilizzati"""

    # Cartelle cache comuni
    CACHE_PATHS = [
        "~/.cache",  # Linux
        "~/AppData/Local/Temp",  # Windows
        "~/AppData/Local/Packages",  # Windows apps
        "~/Library/Caches",  # macOS
        "~/.thumbnails",  # Linux thumbnails
    ]

    # Pattern di file temporanei
    TEMP_PATTERNS = [
        "*.tmp",
        "*.temp",
        "*.bak",
        "~*",
        "*.swp",
        "Thumbs.db",
        ".DS_Store",
        "desktop.ini",
    ]

    def __init__(self):
        pass

    def find_cache_dirs(self, root_path=None, include_system=False):
        """Ritrova cartelle cache nel sistema"""
        cache_dirs = []

        if root_path:
            root = Path(root_path).expanduser()
            for pattern in ["**/.cache", "**/__pycache__", "**/.pytest_cache", "**/.mypy_cache"]:
                for path in root.glob(pattern):
                    if path.is_dir():
                        cache_dirs.append(str(path))

        if include_system:
            for cache_path in self.CACHE_PATHS:
                try:
                    expanded = Path(cache_path).expanduser()
                    if expanded.exists():
                        cache_dirs.append(str(expanded))
                except:
                    pass

        return list(set(cache_dirs))  # Rimuovi duplicati

    def find_temp_files(self, root_path=".", max_items=1000):
        """Trova file temporanei da eliminare"""
        temp_files = []
        root = Path(root_path)

        try:
            for pattern in self.TEMP_PATTERNS:
                for file_path in root.rglob(pattern):
                    if file_path.is_file():
                        try:
                            temp_files.append({
                                "path": str(file_path),
                                "size": file_path.stat().st_size,
                                "size_mb": file_path.stat().st_size / (1024 * 1024)
                            })
                            if len(temp_files) >= max_items:
                                return temp_files
                        except:
                            pass
        except:
            pass

        return temp_files

    def estimate_cleanup(self, root_path=".", include_system=False, max_items=1000):
        """Stima spazio liberabile"""
        stats = {
            "temp_files": [],
            "cache_dirs": [],
            "estimated_mb": 0,
            "breakdown": {
                "temp": 0,
                "cache": 0
            }
        }

        # Temp files
        temp_files = self.find_temp_files(root_path, max_items=max_items)
        stats["temp_files"] = temp_files
        temp_size = sum(f["size"] for f in temp_files)
        stats["breakdown"]["temp"] = temp_size / (1024 * 1024)

        # Cache dirs
        cache_dirs = self.find_cache_dirs(root_path, include_system=include_system)
        for cache_dir in cache_dirs:
            try:
                cache_path = Path(cache_dir)
                if cache_path.exists():
                    size = sum(f.stat().st_size for f in cache_path.rglob("*") if f.is_file())
                    stats["cache_dirs"].append({
                        "path": cache_dir,
                        "size_mb": size / (1024 * 1024)
                    })
                    stats["breakdown"]["cache"] += size / (1024 * 1024)
            except:
                pass

        stats["estimated_mb"] = stats["breakdown"]["temp"] + stats["breakdown"]["cache"]

        return stats

    def cleanup_temp_files(self, root_path=".", dry_run=True):
        """Pulisci file temporanei"""
        temp_files = self.find_temp_files(root_path)

        results = {
            "deleted": [],
            "failed": [],
            "total_freed_mb": 0,
            "dry_run": dry_run
        }

        for file_info in temp_files:
            try:
                file_path = Path(file_info["path"])
                if file_path.exists():
                    if not dry_run:
                        os.remove(file_path)
                    results["deleted"].append(file_info["path"])
                    results["total_freed_mb"] += file_info["size_mb"]
            except Exception as e:
                results["failed"].append({
                    "file": file_info["path"],
                    "error": str(e)
                })

        return results

    def cleanup_cache_dirs(self, root_path=".", dry_run=True):
        """Pulisci cartelle cache"""
        cache_dirs = self.find_cache_dirs(root_path)

        results = {
            "deleted_dirs": [],
            "failed": [],
            "total_freed_mb": 0,
            "dry_run": dry_run
        }

        for cache_dir in cache_dirs:
            try:
                cache_path = Path(cache_dir)
                if cache_path.exists():
                    # Calcola size prima di eliminare
                    size = sum(f.stat().st_size for f in cache_path.rglob("*") if f.is_file())

                    if not dry_run:
                        shutil.rmtree(cache_path)
                        cache_path.mkdir(parents=True, exist_ok=True)  # Ricrea vuota

                    results["deleted_dirs"].append(cache_dir)
                    results["total_freed_mb"] += size / (1024 * 1024)
            except Exception as e:
                results["failed"].append({
                    "dir": cache_dir,
                    "error": str(e)
                })

        return results

    def find_orphan_files(self, root_path="."):
        """Trova file orfani (senza riferimenti)"""
        # Semplificato: rileva file vecchi > 1 anno non acceduti
        from datetime import datetime, timedelta
        import time

        orphan_files = []
        root = Path(root_path)

        now = time.time()
        one_year_ago = now - (365 * 24 * 3600)

        try:
            for file_path in root.rglob("*"):
                if file_path.is_file():
                    try:
                        atime = file_path.stat().st_atime
                        if atime < one_year_ago:
                            orphan_files.append({
                                "path": str(file_path),
                                "size": file_path.stat().st_size,
                                "size_mb": file_path.stat().st_size / (1024 * 1024),
                                "last_accessed": datetime.fromtimestamp(atime).isoformat()
                            })
                    except:
                        pass
        except:
            pass

        return orphan_files
