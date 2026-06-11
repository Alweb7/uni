import asyncio
import edge_tts
import pdfplumber
import sys
import os
from pathlib import Path

VOICE = "it-IT-IsabellaNeural"
CHUNK_SIZE = 4000


def extract_text(pdf_path):
    text = ""
    with pdfplumber.open(pdf_path) as pdf:
        for page in pdf.pages:
            page_text = page.extract_text()
            if page_text:
                text += page_text + "\n"
    return text.strip()


def split_text(text):
    chunks = []
    while len(text) > CHUNK_SIZE:
        split_at = text.rfind(". ", 0, CHUNK_SIZE)
        if split_at == -1:
            split_at = CHUNK_SIZE
        chunks.append(text[:split_at + 1])
        text = text[split_at + 1:].strip()
    if text:
        chunks.append(text)
    return chunks


async def convert(pdf_path):
    pdf_path = Path(pdf_path)
    print(f"Lettura: {pdf_path.name}")

    text = extract_text(pdf_path)
    if not text:
        print("Errore: nessun testo trovato (PDF scansionato o immagine)")
        return

    chunks = split_text(text)
    print(f"Testo: {len(text)} caratteri → {len(chunks)} parti")

    for i, chunk in enumerate(chunks, 1):
        if len(chunks) == 1:
            out = pdf_path.with_suffix(".mp3")
        else:
            out = pdf_path.parent / f"{pdf_path.stem}_parte{i}.mp3"

        print(f"Generazione audio parte {i}/{len(chunks)}...")
        communicate = edge_tts.Communicate(chunk, VOICE)
        await communicate.save(str(out))
        print(f"Salvato: {out.name}")

    print("Fatto!")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python pdf_to_mp3.py <percorso_del_pdf>")
        print("Esempio: python pdf_to_mp3.py \"SO/Teoria/01_introduzione_UNIX_T2.pdf\"")
        sys.exit(1)

    asyncio.run(convert(sys.argv[1]))
