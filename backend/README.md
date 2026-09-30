# IGH EDUCATIVO Backend

Backend inicial para que el juego descargue cuestionarios y guarde el historial de cada jugador.

## Objetivo del MVP

1. El jugador inicia sesión con código de trabajador + PIN.
2. La app consulta los temas activos.
3. El servidor crea un intento y selecciona 10 preguntas.
4. La app recibe las preguntas sin la respuesta correcta.
5. Cada respuesta se envía al servidor.
6. El servidor valida la respuesta y calcula el puntaje.
7. Al terminar la carrera se guarda el resumen.
8. El panel podrá consultar historial y rankings por periodo.

## Estructura

- `database/schema.sql`: esquema MySQL.
- `config/config.example.php`: configuración de ejemplo.
- `src/bootstrap.php`: conexión PDO y respuestas JSON.
- `public/health.php`: prueba de conexión API/Base de datos.

## Modelo de datos principal

- `users`: jugadores.
- `user_sessions`: sesiones de la API.
- `topics` / `subtopics`: clasificación del contenido.
- `questions` / `question_options`: banco de preguntas.
- `attempts`: una carrera/cuestionario iniciado por un jugador.
- `attempt_questions`: las 10 preguntas asignadas a ese intento.
- `attempt_answers`: respuesta, tiempo y puntaje de cada pregunta.

## API prevista

- `POST /api/auth/login.php`
- `GET /api/topics.php`
- `POST /api/attempts/start.php`
- `POST /api/attempts/answer.php`
- `POST /api/attempts/finish.php`
- `GET /api/history.php`
- `GET /api/ranking.php?month=2026-09`

Las respuestas correctas no se envían al cliente antes de responder. El servidor es quien valida la opción elegida y calcula el puntaje.

## Primer arranque local

1. Crear una base MySQL ejecutando `database/schema.sql`.
2. Copiar `config/config.example.php` a `config/config.php`.
3. Ajustar usuario/contraseña MySQL.
4. Publicar `backend/public` con Apache/PHP.
5. Abrir `health.php` y comprobar que devuelva:

```json
{"ok":true,"service":"IGH EDUCATIVO API","database":"connected"}
```

## Próximo paso

Implementar autenticación, selección de temas, entrega de 10 preguntas, registro de respuestas, cierre de intento e informes/ranking mensual.
