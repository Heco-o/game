plugins {
    id("com.android.application")
}

android {
    namespace = "com.example.maze"
    compileSdk = 36
    buildToolsVersion = "36.0.0"
    ndkVersion = "30.0.16248370"

    defaultConfig {
        applicationId = "com.example.maze"
        
        enableKotlin = false

        // Android 9 (Pie) is API 28.
        minSdk = 28

        // Latest stable Android platform as of 2026-11-25.
        targetSdk = 36

        versionCode = 1
        versionName = "1.0"

        ndk {
            abiFilters += listOf("arm64-v8a")
        }

        externalNativeBuild {
            cmake {
                cppFlags += listOf("-std=c++20")
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "4.1.2"
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
}
