# Android APK Build Guide

## Prerequisites
- **Android Studio**: Install from [developer.android.com](https://developer.android.com/studio)
- **Java Development Kit (JDK)**: JDK 17 is recommended.
- **Android SDK**: Ensure SDK Platform 34 and Build Tools are installed via Android Studio's SDK Manager.

## Step-by-Step Build Instructions

### 1. Build the Quasar Project
Run the following command in your terminal (`c:\Users\gaura\Desktop\IIT Jodhpur\asthama`):
```bash
quasar build -m capacitor -T android
```
This command will:
- Build the production web files (`dist/spa`)
- Synchronize them with the Android project (`src-capacitor/android`)

### 2. Open in Android Studio
After the build completes, open Android Studio and select the following folder:
`c:\Users\gaura\Desktop\IIT Jodhpur\asthama\src-capacitor\android`

### 3. Build the APK
1. Wait for Gradle sync to complete in Android Studio.
2. In the top menu, go to **Build** → **Build Bundle(s) / APK(s)** → **Build APK(s)**.
3. Once the build is finished, a notification will appear. Click **Locate** to find your `app-debug.apk`.

### 4. Install on Mobile
1. Connect your Android phone to your PC via USB.
2. Ensure **USB Debugging** is enabled in Developer Options.
3. Drag and drop the `app-debug.apk` to your phone or use the **Run** button in Android Studio to install directly.

## Tips for Better APKs

### App Icon & Splash Screen
To generate icons and splash screens:
1. Install Quasar IconGen: `npm install -g @quasar/icongenerator`
2. Run: `quasar icongen -i public/icons/favicon.ico`

### Production Release (Signed APK)
When you are ready for a production release:
1. Go to **Build** → **Generate Signed Bundle / APK...**
2. Follow the wizard to create a KeyStore and sign your app.

## Troubleshooting
- **Gradle Sync Fails**: Ensure you have a stable internet connection and the correct version of Android Studio.
- **Missing SDK**: Use Android Studio SDK Manager to install missing platforms.
- **Node version**: Ensure you are using Node 18 or later.

Your app is now ready for Android! 🚀📱
