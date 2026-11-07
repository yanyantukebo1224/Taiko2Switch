# Taiko2Switch

Nintendo Switch homebrew TJA rhythm-game player project, with a companion desktop audio-to-TJA draft generator.

## Current implementation

- libnx application entry point and SD card mounting
- SD card song-folder scanner and basic TJA metadata browser
- Initial TJA header/note parser (common single-chart format)
- Folder-based default skin manifest (runtime skin switching is not implemented yet)
- Desktop GUI that analyzes an audio file and creates an editable TJA draft plus a song folder
- GitHub Actions NRO build

This is an early porting milestone, **not yet a complete playable Taiko game**. Audio playback, note rendering, timing/judgement, result screen, score persistence, course selection, and skin rendering still need implementation. TJA branch charts and timing/scroll gimmicks are currently rejected by the parser instead of being silently misread.

## SD card layout

Copy the NRO to:

```text
sd:/switch/Taiko2Switch/Taiko2Switch.nro
```

Put songs here:

```text
sd:/switch/Taiko2Switch/songs/<Song Folder>/<Song>.tja
sd:/switch/Taiko2Switch/songs/<Song Folder>/<Audio File>
```

The TJA `WAVE:` value must match the audio filename. The first TJA found in each song folder is listed.

## Desktop audio-to-TJA generator

Requirements: Python 3.10+ recommended.

```powershell
py -m pip install -r tools/requirements.txt
py tools/tja_autogen.py
```

Choose an audio file, choose a difficulty from 1–10, and generate. The tool creates a folder containing the copied audio and a TJA file. It uses onset detection and beat estimation to make a **draft**, not a guaranteed accurate transcription. Review the BPM, offset, note placements, and difficulty before distributing or playing it.

## Skins

Skin folders live under `skins/`. Each skin currently has a `skin.ini` manifest. The default manifest is a starting point for the upcoming renderer and skin selector; it is not yet loaded by the Switch UI.

## Build

GitHub Actions builds `Taiko2Switch.nro` on pushes to `main` and via manual workflow dispatch. Open the Actions tab and download the `Taiko2Switch-NRO` artifact from a successful run.

## Controller status

Joy-Con/standard controller navigation currently uses libnx HID. Gameplay drum mapping is not implemented yet. The exact older PS4 drum model still needs identification and USB protocol testing. MissionControl is for Bluetooth devices; it is not a generic USB-drum driver. sys-con may help with some USB devices, but compatibility must be confirmed per model.

## Planned work

1. Audio decoder/mixer and audio-clock timing
2. TJA gameplay parser expansion, note lane renderer, judgement and results
3. Course/difficulty selection and persistent scores
4. Runtime skin loading and skin selector
5. Joy-Con and USB drum gameplay mapping
6. TJA format compatibility tests and broader chart support
