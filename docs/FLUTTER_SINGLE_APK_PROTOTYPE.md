# Prototipo: Flutter + IGH EDUCATIVO en una sola APK

Este experimento NO modifica la rama estable `feature/educational-questions`.

## Ramas de seguridad

- Estado estable: `feature/educational-questions`
- Copia exacta de respaldo: `backup/educational-working-2026-10-01`
- Experimento: `prototype/flutter-single-apk`

Si el prototipo falla, volver a la versión estable no requiere revertir commits:

```bash
git switch feature/educational-questions
git pull origin feature/educational-questions
```

## Objetivo

Generar un AAR de IGH EDUCATIVO e incluirlo dentro de una aplicación Flutter.
El usuario instala una sola APK. Flutter es la pantalla principal y abre
`SuperTuxKartActivity` desde el mismo paquete instalado.

El juego se ejecuta en el proceso Android `:ighgame`. Esto permite que, si
el motor nativo se cierra o falla, no tenga que terminar también el proceso
principal de Flutter.

## 1. Construir el juego como AAR

Desde WSL:

```bash
cd ~/STK-EDU/stk-code
git fetch origin
git switch prototype/flutter-single-apk
git pull origin prototype/flutter-single-apk

export SDK_PATH="$HOME/Android/Sdk"
export NDK_PATH="$HOME/Android/Sdk/ndk"
export COMPILE_ARCH=aarch64
export BUILD_TYPE=debug
export BUILD_AS_LIBRARY=1

cd android
./generate_assets.sh
./make.sh

find build/outputs/aar -type f -name "*.aar" -print
```

El resultado esperado es un archivo parecido a:

```text
android/build/outputs/aar/android-debug.aar
```

## 2. Crear una app Flutter de prueba

```bash
flutter create igh_flutter_demo
cd igh_flutter_demo
mkdir -p android/app/libs
```

Copiar el AAR:

```bash
cp ~/STK-EDU/stk-code/android/build/outputs/aar/android-debug.aar \
   android/app/libs/igh-educativo-debug.aar
```

## 3. Agregar el AAR al proyecto Android de Flutter

Si tu proyecto usa `android/app/build.gradle.kts`, dentro de
`dependencies { ... }` agrega:

```kotlin
implementation(files("libs/igh-educativo-debug.aar"))
implementation("org.minidns:minidns-hla:0.3.3")
```

El manifiesto del AAR registra automáticamente
`com.ighgroup.educativo.demo.SuperTuxKartActivity`.

## 4. Puente Flutter -> juego

En `MainActivity.kt` usa un MethodChannel con el nombre:

```text
igh.educativo/game
```

El ejemplo se encuentra en:

```text
examples/flutter_host/MainActivity.kt.example
```

Y el botón Flutter en:

```text
examples/flutter_host/game_page.dart
```

## 5. Resultado esperado

```text
Una sola APK
   |
   +-- Flutter
   |     Inicio
   |     Cursos
   |     Perfil
   |     Juegos
   |        |
   |        +-- [IGH EDUCATIVO]
   |                    |
   +--------------------+
                        |
               SuperTuxKartActivity
                        |
                   C++ / STK
```

Al pulsar Atrás desde el juego, Android vuelve a la Activity de Flutter.

## Importante

Este primer prototipo solo valida tres cosas:

1. El AAR se genera correctamente.
2. Flutter puede incluirlo en una sola APK.
3. El botón puede abrir el motor del juego y volver a Flutter.

Todavía no implementa usuario, intento, preguntas de servidor ni devolución
de resultados. Eso se agrega después de validar este paso.
