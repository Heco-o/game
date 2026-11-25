# Stable Android build configuration upgrade

Prepared for the `Heco-o/game` SFML/EnTT Android starter project.

## Version set

- JDK: Eclipse Temurin 25
- Android Gradle Plugin: 9.5.2
- Gradle: 9.9.2
- Android SDK Platform: 36 (Android 16, stable)
- Android Build Tools: 36.0.0
- Android NDK: 30.0.16248370 (r30 LTS)
- Android SDK CMake: 4.1.2
- SFML: 3.1.0
- EnTT: 4.0.0
- GLM: 1.0.3
- C++: C++20

## Android OS support

`minSdk = 28` means Android 9 (Pie) is the minimum supported Android version.

`compileSdk = 36` and `targetSdk = 36` use the latest stable Android platform selected for this configuration. Android API 37 is not selected because it is a preview platform in this version set.

The build targets only `arm64-v8a`. Consequently, Android 9 devices must have a 64-bit ARM CPU.

## Apply this ZIP

Extract the ZIP into the root of your repository and allow it to overwrite the matching configuration files.

This is a configuration overlay, not a complete source-code replacement. It deliberately does not contain:
- `app/src/main/cpp/main.cpp`
- `app/src/main/AndroidManifest.xml`
- font files or other assets
- your game source files

Keep those existing files.

## Important checks before building

1. Confirm `app/src/main/AndroidManifest.xml` contains NativeActivity metadata with:
   `android.app.lib_name` = `maze_game`.
2. Confirm the app module is named `app`.
3. Confirm the C++ entry file is `app/src/main/cpp/main.cpp`.
4. If your current `namespace` or `applicationId` differs from `com.example.maze`, preserve your existing values in `app/build.gradle.kts`.
5. EnTT 4.0.0 is a major-version upgrade from 3.x. Review any EnTT API changes if the full game source is added.

## Build status

These files are version-pinned configuration updates. They have not been run through a complete GitHub Actions Android build in this environment, so the ZIP does not claim that an APK has already built successfully.
