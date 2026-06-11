from kokoro_onnx import Kokoro
import pdfplumber
import soundfile as sf
import re
import sys
from pathlib import Path

MODEL_PATH  = r"c:\Users\tommy\uni\models\kokoro-v1_0.onnx"
VOICES_PATH = r"c:\Users\tommy\uni\models\voices-v1.0.bin"
VOICE       = "af_heart"
LANG        = "it"
SPEED       = 0.95
CHUNK_SIZE  = 400


def clean_text(text):
    text = re.sub(r'\n\s*\d+\s*\n', '\n', text)
    text = re.sub(r'-\n(\w)', r'\1', text)
    text = re.sub(r'(?<![.!?:;])\n(?![•\-\d\n])', ' ', text)
    text = re.sub(r' {2,}', ' ', text)
    text = re.sub(r'\n{3,}', '\n\n', text)
    return text.strip()


def extract_text(pdf_path):
    text = ""
    with pdfplumber.open(pdf_path) as pdf:
        for page in pdf.pages:
            page_text = page.extract_text()
            if page_text:
                text += page_text + "\n"
    return clean_text(text)


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


def convert(pdf_path):
    pdf_path = Path(pdf_path)
    print(f"Lettura: {pdf_path.name}")

    text = extract_text(pdf_path)
    if not text:
        print("Errore: nessun testo trovato (PDF scansionato o immagine)")
        return

    chunks = split_text(text)
    print(f"Testo: {len(text)} caratteri -> {len(chunks)} parti")
    print("Caricamento modello Kokoro...")

    kokoro = Kokoro(MODEL_PATH, VOICES_PATH)
    out = pdf_path.with_suffix(".wav")

    sample_rate = None
    with sf.SoundFile(str(out), mode='w', samplerate=24000, channels=1, subtype='PCM_16') as audio_file:
        for i, chunk in enumerate(chunks, 1):
            print(f"Generazione {i}/{len(chunks)}...", end="\r")
            samples, sample_rate = kokoro.create(chunk, voice=VOICE, speed=SPEED, lang=LANG)
            audio_file.write(samples)

    print(f"\nSalvato: {out.name}")
    print("Fatto!")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python pdf_to_mp3.py <percorso_del_pdf>")
        print('Esempio: python pdf_to_mp3.py "prog ad oggetti/Teoria/Dispense_PPOO_2526.pdf"')
        sys.exit(1)

    convert(sys.argv[1])
