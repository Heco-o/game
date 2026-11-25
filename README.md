# SFML 3.1 Android starter (arm64-v8a)

This repository is a Gradle + CMake + Android NativeActivity starter for an SFML 3.1 C++ game. GitHub Actions builds a **release APK** for **arm64-v8a**.

## First build

1. Extract/copy these files into the root of your empty GitHub repository.
2. Commit and push to the `main` branch (or `master`).
3. Open the repository on GitHub.
4. Open **Actions** and select **Android Release APK**.
5. Wait for the workflow to finish successfully.
6. Open the completed workflow run, find **Artifacts**, and download `maze-game-arm64-release`.
7. Extract the ZIP artifact and install the APK on your Android device.

The workflow is also manually runnable from **Actions → Android Release APK → Run workflow**.

## What this first APK contains

`app/src/main/cpp/main.cpp` is deliberately a tiny SFML smoke test. It verifies the Android toolchain, SFML Android backend, native activity, packaging, and APK installation before your full game is migrated.

## Integrating the existing desktop game

Do **not** copy the current desktop `.cpp` over the smoke test unchanged. The source currently contains desktop-only pieces that need porting:

- Remove `#include <uwebsockets/App.h>`, `ClientData`, `startWebInputService`, WebSocket variables, and the input server thread.
- Replace the WebSocket-fed `inputState` with Android touch input (SFML touch events) or an on-screen joystick.
- Keep the EnTT, GLM, collision, level, and render-queue code; those parts are not inherently desktop-only.
- Revisit the desktop `sf::Keyboard` / close-event assumptions and Android pause/resume behavior.
- Ensure the logic thread stops and joins safely when the activity is paused or destroyed.
- Put `ProggyClean.ttf` under `app/src/main/assets/`. SFML's Android resource stream lets `font.openFromFile("ProggyClean.ttf")` load an APK asset with the same filename.
- The current source's `sf::RenderWindow` creation and event loop should be checked on-device after integration.

The app's native library name is `maze_game`. Keep this consistent in:
- `add_library(maze_game SHARED main.cpp)` in CMake
- `android.app.lib_name` in `AndroidManifest.xml`

## Build configuration

- SFML: 3.1.0
- EnTT: v3.15.0
- GLM: 1.0.1
- C++: C++20
- Android ABI: arm64-v8a
- Android min SDK: 23
- Compile SDK / target SDK: 35
- NDK: 27.2.12479018
- CMake: 3.28.0
- JDK: 17
- Gradle: 8.10.2
- Build: Release APK

Dependencies are downloaded by CMake during the GitHub Actions run. No SDK, NDK, CMake, or Gradle installation is required in Termux.

## Free GitHub Actions usage

For public repositories, GitHub-hosted standard runner usage is free under GitHub's current Actions policy. The build still consumes runner time and may be subject to GitHub's fair-use and service limits.
