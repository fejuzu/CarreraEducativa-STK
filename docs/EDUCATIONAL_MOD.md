# Carrera Educativa STK

Esta rama agrega el modo educativo sobre el fork de SuperTuxKart.

## Reglas actuales de prueba

- Plataforma objetivo final: Android.
- El sistema de energías queda pospuesto mientras se realizan pruebas.
- Cada carrera educativa usa 10 preguntas obligatorias.
- Tipos de pregunta: opción múltiple y verdadero/falso.
- Las preguntas ya no aparecen por distancia automáticamente.
- Se crean 10 zonas de regalos educativos a lo largo de la pista.
- Cada zona contiene varias cajas de regalo atravesando el ancho de la pista
  para que el jugador pueda recoger una con facilidad.
- Al recoger una caja de una zona educativa, la carrera se pausa y aparece la
  siguiente pregunta.
- Respuesta correcta: recompensa aleatoria (turbo, nitro o power-up).
- Respuesta incorrecta: penalización aleatoria (reducción de velocidad,
  limitación temporal o pérdida de nitro).
- Las zonas educativas son obligatorias: si el jugador pasa la siguiente zona
  sin recoger un regalo, se activa la animación normal de rescate y el kart
  vuelve a una posición inmediatamente anterior a esa fila de regalos.
- Los karts controlados por IA no consumen los regalos educativos.
- Al terminar la carrera se muestran respuestas correctas, incorrectas y
  porcentaje de precisión.

## Implementación

`src/education/question_manager.*` administra el banco de preguntas, selecciona
10 preguntas únicas para la carrera y registra cada respuesta.

`src/education/question_dialog.*` muestra la pregunta, bloquea el avance hasta
responder y aplica recompensa o penalización.

`src/modes/linear_world.*` crea las filas de regalos educativos, controla el
orden obligatorio y solicita el rescate cuando una fila es omitida.

`src/items/item_manager.cpp` distingue los regalos educativos de las cajas de
power-up normales. Una caja educativa abre una pregunta en lugar de entregar el
power-up estándar de SuperTuxKart.

## Pendiente

- Sustituir el banco demo por el banco real de preguntas.
- Persistencia/API/MySQL.
- Reportes detallados por tema y subtema.
- Ajustes visuales de personajes y UI.
- Compilación y pruebas Android/APK.
