# Goatrance Gater V2 — VST3 / Standalone

Version 2 adds swing and independently adjustable attack/release smoothing to the tempo-synced 16-step gate. It supports mono/stereo, host tempo/PPQ sync (120 BPM free-run fallback), 1/2/4-bar patterns, depth, wet/dry, and saved step patterns.

## Build

Requirements: CMake 3.22+, C++17 compiler, Git, and the platform's JUCE dependencies. CMake fetches JUCE 8.0.6 during configuration.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The VST3 is placed under `build/GoatranceGaterV2_artefacts/Release/VST3/` (or the matching build configuration directory). This project has not been precompiled; install the resulting bundle and rescan plugins in your DAW.

## Controls

- **Loop**: repeat the 16-step sequence over 1, 2, or 4 bars.
- **Depth**: attenuation applied to inactive steps.
- **Wet**: blend dry and gated signals.
- **Attack / Release**: gate envelope times in milliseconds.
- **Swing**: delays alternating (odd-numbered) steps for a swung rhythm.
- **Steps 1–16**: toggle whether each step passes the signal.

## Download source ZIP

Use GitHub's branch ZIP download: [Goatrance Gater V2 source ZIP](https://github.com/privatman1984-ship-it/PsyGo-Gater/archive/refs/heads/v2-goatrance-gater.zip). This is the source archive, not a compiled VST3 binary.

You can also create the ZIP locally: `./scripts/package.sh` on macOS/Linux, or `powershell -ExecutionPolicy Bypass -File scripts/package.ps1` on Windows. The output is `GoatranceGaterV2-source.zip`.

## License

Project source is MIT licensed. JUCE is fetched separately and remains under its own license; review JUCE licensing terms before distributing compiled plugins.
