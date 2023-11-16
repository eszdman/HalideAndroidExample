plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}
afterEvaluate{
    println("current folder: " + System.getProperty("user.dir"))
    val cmd1 = Runtime.getRuntime().exec("cmake -S ./app/src/main/cpp/generator/ -B ./app/src/main/cpp/generator/build/")
    cmd1.waitFor()
    println(cmd1.inputReader().readText())
    val err1 = cmd1.errorReader().readText()
    if(err1 != "")
        error(err1)
    val cmd2 = Runtime.getRuntime().exec("cmake --build ./app/src/main/cpp/generator/build/")
    cmd2.waitFor()
    println(cmd2.inputReader().readText())
    val err2 = cmd2.errorReader().readText()
    if(err2 != "")
        error(err2)
}
android {
    buildFeatures {
        prefab = true
    }
    namespace = "com.example.halideandroidexample"
    compileSdk = 33

    defaultConfig {
        applicationId = "com.example.halideandroidexample"
        minSdk = 24
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"
        ndk {
            abiFilters.add("arm64-v8a")
        }
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
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
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"

        }
    }


    buildFeatures {
        viewBinding = true
    }

}

dependencies {
    implementation("io.github.hexhacking:xdl:2.0.0")
    implementation("androidx.core:core-ktx:1.9.0")
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.9.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}