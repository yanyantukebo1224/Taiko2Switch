#!/usr/bin/env python3
"""Create an editable onset-based TJA draft from an audio file."""
from __future__ import annotations
import os
import re
import shutil
import threading
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

try:
    import librosa
    import numpy as np
except ImportError:
    librosa = None
    np = None


def safe_name(value: str) -> str:
    value = re.sub(r'[\\/:*?"<>|]+', "_", value).strip(" .")
    return value or "GeneratedSong"


def make_tja(audio_path: str, difficulty: int, output_root: str) -> str:
    if librosa is None:
        raise RuntimeError("Install dependencies: python -m pip install -r tools/requirements.txt")
    y, sr = librosa.load(audio_path, sr=22050, mono=True)
    if len(y) < sr * 3:
        raise ValueError("Choose an audio track at least 3 seconds long.")
    tempo, _ = librosa.beat.beat_track(y=y, sr=sr, units="time")
    bpm = float(np.asarray(tempo).reshape(-1)[0])
    if not np.isfinite(bpm) or not 50 <= bpm <= 260:
        bpm = 120.0

    hop = 256
    frames = librosa.onset.onset_detect(y=y, sr=sr, hop_length=hop, backtrack=False)
    times = librosa.frames_to_time(frames, sr=sr, hop_length=hop)
    centroid = librosa.feature.spectral_centroid(y=y, sr=sr, hop_length=hop)[0]
    rms = librosa.feature.rms(y=y, hop_length=hop)[0]
    median_centroid = float(np.median(centroid))
    loud_cut = float(np.percentile(rms, 75))
    min_gap = {1:4, 2:4, 3:3, 4:3, 5:2, 6:2, 7:2, 8:1, 9:1, 10:1}[difficulty]
    step_seconds = 60.0 / bpm / 4.0
    notes = {}
    last_step = -999
    for seconds in times:
        step = max(0, int(round(float(seconds) / step_seconds)))
        if step - last_step < min_gap:
            continue
        frame = min(len(centroid) - 1, max(0, int(round(float(seconds) * sr / hop))))
        note = "2" if centroid[frame] > median_centroid else "1"
        if difficulty >= 7 and rms[min(frame, len(rms)-1)] > loud_cut:
            note = "4" if note == "2" else "3"
        notes[step] = note
        last_step = step
    if not notes:
        raise ValueError("No clear attacks detected. Try a different track.")

    rows = []
    for bar in range(max(notes) // 16 + 1):
        rows.append("".join(notes.get(bar * 16 + i, "0") for i in range(16)) + ",")
    title = safe_name(os.path.splitext(os.path.basename(audio_path))[0])
    song_dir = os.path.join(output_root, title)
    os.makedirs(song_dir, exist_ok=True)
    wave_name = os.path.basename(audio_path)
    shutil.copy2(audio_path, os.path.join(song_dir, wave_name))
    text = (
        f"TITLE:{title}\nARTIST:Unknown\nGENRE:バラエティ\nBPM:{bpm:.3f}\n"
        f"WAVE:{wave_name}\nOFFSET:0\nCOURSE:Oni\nLEVEL:{difficulty}\n"
        "#START\n" + "\n".join(rows) + "\n#END\n"
    )
    output = os.path.join(song_dir, title + ".tja")
    with open(output, "w", encoding="utf-8-sig", newline="\n") as fp:
        fp.write(text)
    return output


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Taiko2Switch - Audio to TJA")
        self.geometry("570x265")
        self.resizable(False, False)
        self.audio = tk.StringVar()
        self.output = tk.StringVar(value=os.path.abspath("generated_songs"))
        self.level = tk.IntVar(value=5)
        ttk.Label(self, text="Audio file").pack(anchor="w", padx=16, pady=(14, 2))
        row = ttk.Frame(self); row.pack(fill="x", padx=16)
        ttk.Entry(row, textvariable=self.audio).pack(side="left", fill="x", expand=True)
        ttk.Button(row, text="Browse…", command=self.pick_audio).pack(side="left", padx=(8, 0))
        ttk.Label(self, text="Output folder").pack(anchor="w", padx=16, pady=(10, 2))
        row = ttk.Frame(self); row.pack(fill="x", padx=16)
        ttk.Entry(row, textvariable=self.output).pack(side="left", fill="x", expand=True)
        ttk.Button(row, text="Browse…", command=self.pick_output).pack(side="left", padx=(8, 0))
        row = ttk.Frame(self); row.pack(fill="x", padx=16, pady=12)
        ttk.Label(row, text="Difficulty").pack(side="left")
        ttk.Scale(row, from_=1, to=10, variable=self.level, orient="horizontal").pack(
            side="left", fill="x", expand=True, padx=12)
        self.level_text = ttk.Label(row, text="5"); self.level_text.pack(side="right")
        self.level.trace_add("write", lambda *_: self.level_text.configure(text=str(self.level.get())))
        self.status = tk.StringVar(value="Select audio, set a level, and generate a TJA draft.")
        ttk.Label(self, textvariable=self.status, wraplength=530).pack(anchor="w", padx=16)
        ttk.Button(self, text="Generate TJA + song folder", command=self.generate).pack(
            anchor="e", padx=16, pady=12)

    def pick_audio(self):
        path = filedialog.askopenfilename(filetypes=[
            ("Audio", "*.wav *.mp3 *.ogg *.flac *.m4a"), ("All files", "*.*")])
        if path: self.audio.set(path)

    def pick_output(self):
        path = filedialog.askdirectory()
        if path: self.output.set(path)

    def generate(self):
        if not self.audio.get():
            messagebox.showwarning("Missing audio", "Choose an audio file first."); return
        self.status.set("Analyzing audio…")
        def worker():
            try:
                result = make_tja(self.audio.get(), max(1, min(10, round(self.level.get()))),
                                  self.output.get())
                self.after(0, lambda: self.status.set("Created: " + result))
                self.after(0, lambda: messagebox.showinfo("Done",
                    "TJA draft created. Review timing and notes before playing."))
            except Exception as exc:
                self.after(0, lambda: self.status.set("Failed: " + str(exc)))
                self.after(0, lambda: messagebox.showerror("Generation failed", str(exc)))
        threading.Thread(target=worker, daemon=True).start()


if __name__ == "__main__":
    if librosa is None:
        root = tk.Tk(); root.withdraw()
        messagebox.showerror("Dependencies missing",
            "Run: python -m pip install -r tools/requirements.txt")
        root.destroy()
    else:
        App().mainloop()
