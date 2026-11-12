# Goatrance Gater — VST3 / Standalone

A 16-step rhythmic gate effect for psytrance and goatrance. It follows the host tempo when available, supports loop lengths of 1, 2, or 4 bars, and includes a click-reduced gate, wet/dry mix, and editable step pattern.

## Build

Requirements: CMake 3.22+, a C++17 compiler, Git, and a supported JUCE build environment. CMake downloads JUCE 8.0.6 on the first configure. VST3 and Standalone formats are built.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The VST3 bundle is generated under `build/GoatranceGater_artefacts/Release/VST3/` (or the corresponding configuration directory on your platform). Copy the bundle to your system's VST3 plug-in folder and rescan plug-ins in your DAW.

On Linux, install the compiler, CMake, Git, and JUCE's required GUI/audio development dependencies for your distribution before building. On macOS, use Xcode command-line tools; on Windows, use Visual Studio 2022 with C++ desktop development tools.

## Controls

- **Loop**: repeats the 16-step pattern over 1, 2, or 4 bars.
- **Depth**: amount of attenuation on inactive steps.
- **Wet**: blend between the dry signal and gated signal.
- **Steps 1–16**: click a cell to toggle that step on/off. Active steps pass audio; inactive steps are attenuated.

When the DAW supplies tempo and musical position, the pattern follows them. Otherwise it free-runs at 120 BPM. The plug-in accepts mono and stereo audio and saves its parameters and pattern with the project.

## Create a ZIP

Run `./scripts/package.sh` on macOS/Linux, or `powershell -ExecutionPolicy Bypass -File scripts/package.ps1` on Windows. The script creates `GoatranceGater-source.zip` containing the source project (not precompiled binaries). Build the VST3 separately using the steps above.

## License

This project source is provided under the MIT License. JUCE is fetched separately and remains under its own license; review JUCE's licensing terms before distributing compiled plug-ins.
