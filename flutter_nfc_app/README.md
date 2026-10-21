# Flutter NFC Starter App

This project is a minimal Flutter app for reading NFC tags.

## Prerequisites

- Install Flutter SDK from https://docs.flutter.dev/get-started/install
- Ensure `flutter` is available on your PATH

## Run it

```bash
cd flutter_nfc_app
flutter pub get
flutter run
```

## Android permission

The app requests NFC permission in `android/app/src/main/AndroidManifest.xml`.

## iOS permission

Add NFC usage description in the iOS app as shown in the generated Xcode project.

## Notes

This starter scans for a nearby NFC tag and prints the raw tag data to the screen.
