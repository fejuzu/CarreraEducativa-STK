# Carrera Educativa STK

Esta rama agrega la base educativa sobre el fork de SuperTuxKart.

## Reglas funcionales acordadas

- Plataforma objetivo principal: Android.
- 5 energías por jugador al día.
- Se pueden iniciar carreras desde las 07:00 hasta antes de las 21:00.
- Una carrera iniciada antes de las 21:00 puede terminar después de esa hora.
- Cada nivel utiliza 20 preguntas obligatorias.
- Tipos de pregunta: opción múltiple y verdadero/falso.
- Una respuesta correcta entrega una recompensa aleatoria.
- Una respuesta incorrecta aplica un castigo aleatorio.
- Se registrará cada respuesta, incluyendo tiempo, tema, subtema y respuesta correcta.
- Los reportes mostrarán qué preguntas y temas necesita reforzar el jugador.

## Primera implementación

El módulo `src/education/question_manager.*` mantiene un banco de preguntas,
selecciona exactamente 20 preguntas únicas para una carrera y registra cada
respuesta en orden. No permite avanzar internamente a la siguiente pregunta
sin registrar primero la actual.

`src/education/education_rules.hpp` contiene las reglas básicas de horario y
energía. Más adelante la autoridad real de estas reglas estará en la API del
servidor para evitar manipulación local.

## Próximos pasos

1. Crear `QuestionDialog` usando `GUIEngine::ModalDialog`.
2. Activarlo desde un checkpoint/objeto educativo de pista.
3. Pausar la interacción del jugador mientras responde.
4. Correcta -> asignar power-up de STK.
5. Incorrecta -> aplicar penalización.
6. Conectar resultados de la carrera con la API y MySQL.
7. Añadir reportes por tema/subtema.
