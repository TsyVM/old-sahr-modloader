# SHAR ModLoader (Legacy)

## ⚠️ Status: Legacy / Being Replaced

This is our **old** SHAR ModLoader for *The Simpsons: Hit & Run*. It still works, but it is no longer under active development in this form.

A newer, rebuilt version of the mod loader is in progress here:
**https://www.github.com/tsyvm/sahr-modloader**

If you're looking for ongoing updates and new features, check that repo. This repo will remain available as a stable, working fallback.

## Is this usable right now?

**Yes.** This version is functional, and a compiled release is available on the [Releases](../../releases) page. You don't need to build it yourself unless you want to modify the source.

## How to Use It

### Option 1: Use the prebuilt release (recommended)
1. Download `SHARModLoader.dll` and `SHARInjector.exe` from the [Releases](../../releases) page.
2. Copy both files into your *Simpsons: Hit & Run* game folder (the same folder as `Simpsons.exe`).
3. Copy `LaunchWithMods.bat` into that same folder.
4. Create a `mods` folder in the game directory (or just run the launcher once — it creates one for you).
5. Drop your mods into the `mods` folder, each in its own subfolder (e.g. `mods/01_example_mod/`).
6. Run `LaunchWithMods.bat` to start the game with mods loaded.
   - Run with the `--safe` flag (`LaunchWithMods.bat --safe`) to launch with mods disabled.
   - Holding **Shift** at launch also enables Safe Mode.
7. Check `ModLoader.log` in the game folder if something doesn't load correctly.

### Option 2: Build it yourself
1. Install [CMake](https://cmake.org/download/) and Visual Studio 2019 or 2022 (with the "Desktop development with C++" workload).
2. Run `build.bat` from this folder. It will:
   - Detect your Visual Studio installation
   - Generate a 32-bit (Win32) project
   - Build `SHARModLoader.dll` and `SHARInjector.exe` in Release mode
3. Find the built files in `build\bin\`.
4. Follow steps 2–7 from Option 1 above using your freshly built files.

## Requirements
- Windows
- *The Simpsons: Hit & Run* (32-bit)
- Visual Studio 2019/2022 + CMake (only if building from source)
