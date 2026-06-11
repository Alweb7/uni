import shutil
import time
from pathlib import Path

from ai_brain import AIAction, GeminiBrain
from file_inspector import FileInspector
from file_ops import FileLogic


def check(name, condition):
    if not condition:
        raise AssertionError(name)
    print(f"OK {name}")


def main():
    started = time.perf_counter()
    root = Path("tmp_sanity_checks").resolve()
    if root.exists():
        shutil.rmtree(root)
    root.mkdir()

    try:
        brain = GeminiBrain()
        check("ollama model selected", bool(brain.model_id))
        check("local greeting", brain.elabora_comando("ciao", ".").azione == "messaggio")
        check("local folder command", brain.elabora_comando("crea cartella test", ".").azione == "crea_cartella")
        check("protected windows path", FileLogic._percorso_protetto("C:/Windows"))

        try:
            FileLogic._target_sicuro(str(root), "../outside.txt")
            raise AssertionError("path traversal not blocked")
        except PermissionError:
            print("OK path traversal blocked")

        sample = root / "sample.txt"
        sample.write_text("TommyAI sanity file", encoding="utf-8")
        inspected = FileInspector.inspect(sample)
        check("file inspector text", inspected["kind"] == "text")

        action = AIAction(azione="elimina", nome="sample.txt", testo_risposta="")
        FileLogic.esegui_azione(action, str(root))
        check("safe trash move", not sample.exists())
        check("trash exists", FileLogic.cestino_path().exists())
    finally:
        if root.exists():
            shutil.rmtree(root)

    elapsed = time.perf_counter() - started
    print(f"Done in {elapsed:.2f}s")


if __name__ == "__main__":
    main()
