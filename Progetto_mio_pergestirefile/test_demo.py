"""
Demo TommyAI v2 - Test dei moduli core
Esegui questo per testare tutte le funzionalità prima di integrarle in UI
"""

from core import FileAnalyzer, DuplicateHandler, SmartOrganizer, CacheManager
from ai.hybrid_brain import HybridBrain
import json

def print_section(title):
    print(f"\n{'='*60}")
    print(f"  {title}")
    print(f"{'='*60}\n")

def demo_file_analyzer():
    print_section("📁 FILE ANALYZER - Scansione File")

    analyzer = FileAnalyzer(".")
    files = analyzer.scan_directory(max_depth=2)

    print(f"✅ Trovati {len(files)} file")
    print(f"📊 Generando report...")

    report = analyzer.generate_report(files)

    print(f"\n📈 REPORT:")
    print(f"  Total file: {report['total_files']}")
    print(f"  Total size: {report['total_size_mb']:.2f} MB")
    print(f"  Doppioni: {report['duplicate_count']}")
    print(f"  Spazio liberabile: {report['saveable_mb']:.2f} MB")
    print(f"  File vecchi (>1 anno): {report['old_files_count']}")

    print(f"\n📂 CATEGORIE:")
    for cat, stats in report['categories'].items():
        print(f"  {cat}: {stats['count']} file ({stats['size']/(1024*1024):.2f}MB)")

def demo_duplicate_handler():
    print_section("🔍 DUPLICATE HANDLER - Ricerca Doppioni")

    analyzer = FileAnalyzer(".")
    files = analyzer.scan_directory(max_depth=1)
    duplicates = analyzer.find_duplicates(files)

    if duplicates:
        print(f"⚠️ Trovati {len(duplicates)} gruppi di doppioni!\n")

        handler = DuplicateHandler(".")
        recommendations = handler.recommend_delete(duplicates)

        print(f"💡 RACCOMANDAZIONI:")
        for i, rec in enumerate(recommendations[:5], 1):
            print(f"  {i}. Elimina: {rec['file']}")
            print(f"     Spazio: {rec['size_mb']:.2f}MB")
            print(f"     Motivo: {rec['reason']}\n")

        print(f"Total file da eliminare: {len(recommendations)}")
        print(f"Spazio liberabile: {sum(r['size_mb'] for r in recommendations):.2f}MB")
    else:
        print("✅ Nessun doppione trovato!")

def demo_smart_organizer():
    print_section("📁 SMART ORGANIZER - Ordinamento Intelligente")

    analyzer = FileAnalyzer(".")
    files = analyzer.scan_directory(max_depth=1)

    organizer = SmartOrganizer(".")
    stats = organizer.get_organization_stats(files)

    print(f"📊 STATISTICHE ORGANIZZAZIONE:")
    print(f"  Total file: {stats['total_files']}")
    print(f"  Organizzati: {stats['well_organized']}")
    print(f"  Da organizzare: {stats['disorganized']}")
    print(f"  Percentuale: {stats['organization_percent']}%\n")

    if stats['improvement_potential']:
        print(f"💡 SUGGERIMENTI (primi 5):")
        for i, sugg in enumerate(stats['improvement_potential'][:5], 1):
            print(f"  {i}. {sugg['file'].split('/')[-1]}")
            print(f"     → {sugg['suggested_folder']} ({sugg['category']})\n")

def demo_cache_manager():
    print_section("🧹 CACHE MANAGER - Pulizia")

    cache_mgr = CacheManager()
    cleanup = cache_mgr.estimate_cleanup(".")

    print(f"📊 ANALISI PULIZIA:")
    print(f"  Temp file trovati: {len(cleanup['temp_files'])}")
    print(f"  Cache dirs trovati: {len(cleanup['cache_dirs'])}")
    print(f"  Spazio liberabile: {cleanup['estimated_mb']:.2f}MB\n")

    if cleanup['temp_files']:
        print(f"🗑️ TEMP FILES (primi 5):")
        for f in cleanup['temp_files'][:5]:
            print(f"  - {f['path']} ({f['size_mb']:.2f}MB)")

    if cleanup['cache_dirs']:
        print(f"\n💾 CACHE DIRS:")
        for c in cleanup['cache_dirs']:
            print(f"  - {c['path']} ({c['size_mb']:.2f}MB)")

def demo_hybrid_brain():
    print_section("🧠 HYBRID BRAIN - Backend Ibrido")

    brain = HybridBrain()
    health = brain.health_check()

    print(f"🔌 STATO BACKEND:")
    print(f"  Ollama: {'🟢 Online' if health['ollama'] else '🔴 Offline'}")
    print(f"  Claude: {'🟢 Configurato' if health['claude'] else '🟡 Non configurato'}\n")

    if health['ollama']:
        print("⚡ Testando Ollama (classificazione veloce)...")
        result = brain.quick_classify("photo.jpg", ".jpg", 2.5)
        print(f"  Risultato: {result}")
    else:
        print("⚠️ Ollama offline - assicurati che sia in esecuzione!")
        print("   Esegui: ollama serve")

def main():
    print("\n")
    print("╔═══════════════════════════════════════════════════════════╗")
    print("║         🎯 TommyAI v2 - DEMO MODULI CORE 🎯               ║")
    print("╚═══════════════════════════════════════════════════════════╝")

    try:
        demo_file_analyzer()
    except Exception as e:
        print(f"❌ Errore: {e}")

    try:
        demo_duplicate_handler()
    except Exception as e:
        print(f"❌ Errore: {e}")

    try:
        demo_smart_organizer()
    except Exception as e:
        print(f"❌ Errore: {e}")

    try:
        demo_cache_manager()
    except Exception as e:
        print(f"❌ Errore: {e}")

    try:
        demo_hybrid_brain()
    except Exception as e:
        print(f"❌ Errore: {e}")

    print_section("✅ DEMO COMPLETATO")
    print("Pronto per integrare in UI!")

if __name__ == "__main__":
    main()
