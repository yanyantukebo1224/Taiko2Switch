# Taiko2Switch

A Nintendo Switch homebrew TJA rhythm-game player.

## Build

The repository is configured to build automatically with GitHub Actions.

1. Open the **Actions** tab.
2. Select **Nintendo Switch Build**.
3. Select **Run workflow**, or push a commit to `main`.
4. Download the `Taiko2Switch-NRO` artifact from a successful run.

The build uses the official devkitPro `devkitpro/devkita64` container and produces `Taiko2Switch.nro`.

## Install

Copy the generated NRO to:

```text
sd:/switch/Taiko2Switch/Taiko2Switch.nro
```

This initial bootstrap build only verifies the Homebrew application startup. It does not yet play TJA charts.

## Planned modules

- TJA parser and chart model
- Audio playback and audio-clock-based timing
- Note renderer and judgement engine
- Joy-Con / standard controller input
- USB drum input compatibility layer
- Song browser, results, settings and score persistence

## USB drum support

USB and Bluetooth controller compatibility are separate concerns. The application will first use the normal Switch controller-input API for devices exposed to the system as controllers. A model-specific USB input backend may be needed if the drum is not exposed through the standard controller interface.

## Development status

- [x] GitHub Actions build pipeline
- [x] Initial libnx application entry point
- [ ] TJA parser
- [ ] Audio engine
- [ ] Gameplay and judgement
- [ ] Song browser and score saving
- [ ] USB drum model validation
