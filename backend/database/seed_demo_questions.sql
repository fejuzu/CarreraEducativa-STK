-- IGH EDUCATIVO - seed inicial de preguntas
-- Migra el banco hardcodeado de src/education/demo_questions.cpp a MySQL.
-- Requiere ejecutar primero backend/database/schema.sql.

USE igh_educativo;

START TRANSACTION;

INSERT INTO topics (name, description, active)
SELECT 'Seguridad en obra', 'Banco inicial de preguntas de seguridad usado por IGH EDUCATIVO.', 1
WHERE NOT EXISTS (
    SELECT 1 FROM topics WHERE name = 'Seguridad en obra'
);

SET @topic_id := (SELECT id FROM topics WHERE name = 'Seguridad en obra' LIMIT 1);

INSERT IGNORE INTO subtopics (topic_id, name, active) VALUES
(@topic_id, 'EPP', 1),
(@topic_id, 'Señalización', 1),
(@topic_id, 'Orden y limpieza', 1),
(@topic_id, 'Altura', 1),
(@topic_id, 'Electricidad', 1),
(@topic_id, 'Herramientas', 1),
(@topic_id, 'Excavaciones', 1),
(@topic_id, 'Emergencias', 1),
(@topic_id, 'Manipulación manual', 1),
(@topic_id, 'Ruido', 1),
(@topic_id, 'Tránsito interno', 1),
(@topic_id, 'Andamios', 1),
(@topic_id, 'Químicos', 1),
(@topic_id, 'Caídas', 1),
(@topic_id, 'Equipos', 1),
(@topic_id, 'Trabajo en equipo', 1),
(@topic_id, 'Prevención', 1);

-- Pregunta 1
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='EPP'),
       'multiple_choice',
       '¿Qué elemento protege principalmente la cabeza frente a golpes o caída de objetos?',
       'El casco reduce el riesgo de lesiones en la cabeza.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='¿Qué elemento protege principalmente la cabeza frente a golpes o caída de objetos?');
SET @q1 := (SELECT id FROM questions WHERE prompt='¿Qué elemento protege principalmente la cabeza frente a golpes o caída de objetos?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q1,0,'Casco de seguridad',1),(@q1,1,'Guantes',0),(@q1,2,'Chaleco reflectivo',0),(@q1,3,'Botas',0);

-- Pregunta 2
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Señalización'),
       'multiple_choice',
       'Una señal circular roja con una acción tachada normalmente indica:',
       'Las señales de prohibición suelen usar círculo rojo y una barra diagonal.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Una señal circular roja con una acción tachada normalmente indica:');
SET @q2 := (SELECT id FROM questions WHERE prompt='Una señal circular roja con una acción tachada normalmente indica:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q2,0,'Obligación',0),(@q2,1,'Prohibición',1),(@q2,2,'Información',0),(@q2,3,'Ruta de evacuación',0);

-- Pregunta 3
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Orden y limpieza'),
       'multiple_choice',
       'Mantener libres los pasillos de trabajo ayuda principalmente a evitar:',
       'El orden y la limpieza reducen caídas al mismo nivel.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Mantener libres los pasillos de trabajo ayuda principalmente a evitar:');
SET @q3 := (SELECT id FROM questions WHERE prompt='Mantener libres los pasillos de trabajo ayuda principalmente a evitar:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q3,0,'Caídas y tropiezos',1),(@q3,1,'Ruido excesivo',0),(@q3,2,'Radiación',0),(@q3,3,'Cambios de temperatura',0);

-- Pregunta 4
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Altura'),
       'multiple_choice',
       'Antes de usar una escalera portátil se debe:',
       'La inspección y un apoyo estable son controles básicos antes de usarla.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Antes de usar una escalera portátil se debe:');
SET @q4 := (SELECT id FROM questions WHERE prompt='Antes de usar una escalera portátil se debe:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q4,0,'Correr con ella',0),(@q4,1,'Revisar su estado y apoyo',1),(@q4,2,'Subir de espaldas',0),(@q4,3,'Apoyarla sobre cajas',0);

-- Pregunta 5
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Electricidad'),
       'multiple_choice',
       '¿Qué debes hacer antes de intervenir un equipo eléctrico?',
       'La intervención segura requiere desenergización y verificación de ausencia de tensión.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='¿Qué debes hacer antes de intervenir un equipo eléctrico?');
SET @q5 := (SELECT id FROM questions WHERE prompt='¿Qué debes hacer antes de intervenir un equipo eléctrico?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q5,0,'Mojar el área',0),(@q5,1,'Aumentar la carga',0),(@q5,2,'Desenergizar y verificar',1),(@q5,3,'Retirar la puesta a tierra',0);

-- Pregunta 6
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='EPP'),
       'multiple_choice',
       'Los lentes de seguridad sirven principalmente para proteger:',
       'Los lentes protegen frente a partículas, polvo y salpicaduras según el modelo.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Los lentes de seguridad sirven principalmente para proteger:');
SET @q6 := (SELECT id FROM questions WHERE prompt='Los lentes de seguridad sirven principalmente para proteger:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q6,0,'Los ojos',1),(@q6,1,'Los oídos',0),(@q6,2,'Las rodillas',0),(@q6,3,'La espalda',0);

-- Pregunta 7
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Herramientas'),
       'multiple_choice',
       'Una herramienta con cable eléctrico dañado debe:',
       'Un cable dañado puede producir choque eléctrico o incendio.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Una herramienta con cable eléctrico dañado debe:');
SET @q7 := (SELECT id FROM questions WHERE prompt='Una herramienta con cable eléctrico dañado debe:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q7,0,'Seguir usándose con cuidado',0),(@q7,1,'Retirarse de servicio',1),(@q7,2,'Mojarse',0),(@q7,3,'Prestarse a otro trabajador',0);

-- Pregunta 8
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Excavaciones'),
       'multiple_choice',
       'En una excavación, el material retirado debe mantenerse:',
       'Alejar el material reduce sobrecarga del borde y caída de objetos.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='En una excavación, el material retirado debe mantenerse:');
SET @q8 := (SELECT id FROM questions WHERE prompt='En una excavación, el material retirado debe mantenerse:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q8,0,'Al borde',0),(@q8,1,'A distancia segura del borde',1),(@q8,2,'Dentro de la excavación',0),(@q8,3,'Sobre la escalera',0);

-- Pregunta 9
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Emergencias'),
       'multiple_choice',
       'Si detectas humo o fuego fuera de control, lo prioritario es:',
       'La respuesta debe seguir el plan de emergencia y las rutas establecidas.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Si detectas humo o fuego fuera de control, lo prioritario es:');
SET @q9 := (SELECT id FROM questions WHERE prompt='Si detectas humo o fuego fuera de control, lo prioritario es:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q9,0,'Ocultarlo',0),(@q9,1,'Dar la alarma y seguir el plan de emergencia',1),(@q9,2,'Continuar trabajando',0),(@q9,3,'Cerrar todas las salidas',0);

-- Pregunta 10
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Manipulación manual'),
       'multiple_choice',
       'Al levantar una carga manualmente conviene:',
       'Mantener la carga cerca reduce el esfuerzo sobre la espalda.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Al levantar una carga manualmente conviene:');
SET @q10 := (SELECT id FROM questions WHERE prompt='Al levantar una carga manualmente conviene:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q10,0,'Girar el tronco mientras se levanta',0),(@q10,1,'Mantener la carga cerca del cuerpo',1),(@q10,2,'Levantar con una sola mano',0),(@q10,3,'Doblar solo la espalda',0);

-- Pregunta 11
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='EPP'),
       'true_false',
       '¿Verdadero o falso? El EPP reemplaza todos los demás controles de seguridad.',
       'El EPP es una barrera adicional; no reemplaza controles de ingeniería, administrativos u otros.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='¿Verdadero o falso? El EPP reemplaza todos los demás controles de seguridad.');
SET @q11 := (SELECT id FROM questions WHERE prompt='¿Verdadero o falso? El EPP reemplaza todos los demás controles de seguridad.' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q11,0,'Verdadero',0),(@q11,1,'Falso',1);

-- Pregunta 12
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ruido'),
       'multiple_choice',
       'En una zona con nivel de ruido peligroso se debe usar:',
       'La protección auditiva debe seleccionarse según la exposición al ruido.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='En una zona con nivel de ruido peligroso se debe usar:');
SET @q12 := (SELECT id FROM questions WHERE prompt='En una zona con nivel de ruido peligroso se debe usar:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q12,0,'Protección auditiva adecuada',1),(@q12,1,'Solo chaleco',0),(@q12,2,'Solo lentes',0),(@q12,3,'Ningún EPP',0);

-- Pregunta 13
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Tránsito interno'),
       'multiple_choice',
       'Al caminar cerca de maquinaria móvil es mejor:',
       'Las rutas segregadas disminuyen el riesgo de atropello.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Al caminar cerca de maquinaria móvil es mejor:');
SET @q13 := (SELECT id FROM questions WHERE prompt='Al caminar cerca de maquinaria móvil es mejor:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q13,0,'Usar rutas peatonales señalizadas',1),(@q13,1,'Caminar detrás sin avisar',0),(@q13,2,'Usar audífonos',0),(@q13,3,'Cruzar por puntos ciegos',0);

-- Pregunta 14
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Andamios'),
       'multiple_choice',
       'Antes de trabajar sobre un andamio se debe comprobar:',
       'El andamio debe estar correctamente montado e inspeccionado.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Antes de trabajar sobre un andamio se debe comprobar:');
SET @q14 := (SELECT id FROM questions WHERE prompt='Antes de trabajar sobre un andamio se debe comprobar:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q14,0,'Que esté estable y habilitado',1),(@q14,1,'Que tenga ruedas sueltas',0),(@q14,2,'Que falten barandas',0),(@q14,3,'Que esté inclinado',0);

-- Pregunta 15
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Químicos'),
       'multiple_choice',
       'Para conocer peligros y medidas de un producto químico se consulta:',
       'La SDS/HDS contiene información de peligros, manejo y respuesta a emergencias.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Para conocer peligros y medidas de un producto químico se consulta:');
SET @q15 := (SELECT id FROM questions WHERE prompt='Para conocer peligros y medidas de un producto químico se consulta:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q15,0,'La hoja de datos de seguridad',1),(@q15,1,'La tarjeta de asistencia',0),(@q15,2,'El calendario',0),(@q15,3,'El plano de estacionamiento',0);

-- Pregunta 16
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Caídas'),
       'true_false',
       '¿Verdadero o falso? Un piso húmedo sin señalizar puede generar riesgo de caída.',
       'Las superficies húmedas pueden reducir la adherencia y causar resbalones.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='¿Verdadero o falso? Un piso húmedo sin señalizar puede generar riesgo de caída.');
SET @q16 := (SELECT id FROM questions WHERE prompt='¿Verdadero o falso? Un piso húmedo sin señalizar puede generar riesgo de caída.' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q16,0,'Verdadero',1),(@q16,1,'Falso',0);

-- Pregunta 17
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Equipos'),
       'multiple_choice',
       'Antes de operar una máquina que no conoces debes:',
       'La operación debe realizarse por personal autorizado y capacitado.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Antes de operar una máquina que no conoces debes:');
SET @q17 := (SELECT id FROM questions WHERE prompt='Antes de operar una máquina que no conoces debes:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q17,0,'Improvisar',0),(@q17,1,'Recibir autorización/capacitación correspondiente',1),(@q17,2,'Desactivar protecciones',0),(@q17,3,'Aumentar la velocidad',0);

-- Pregunta 18
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Trabajo en equipo'),
       'multiple_choice',
       'Si observas una condición insegura grave debes:',
       'Reportar oportunamente permite controlar el peligro antes de un incidente.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='Si observas una condición insegura grave debes:');
SET @q18 := (SELECT id FROM questions WHERE prompt='Si observas una condición insegura grave debes:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q18,0,'Ignorarla',0),(@q18,1,'Reportarla y aplicar el procedimiento correspondiente',1),(@q18,2,'Ocultarla',0),(@q18,3,'Esperar varios días',0);

-- Pregunta 19
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Señalización'),
       'multiple_choice',
       'El color verde en señalización de seguridad suele asociarse con:',
       'El verde suele identificar condiciones seguras, primeros auxilios o rutas de evacuación.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='El color verde en señalización de seguridad suele asociarse con:');
SET @q19 := (SELECT id FROM questions WHERE prompt='El color verde en señalización de seguridad suele asociarse con:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q19,0,'Prohibición',0),(@q19,1,'Equipos contra incendio',0),(@q19,2,'Condición segura o evacuación',1),(@q19,3,'Peligro eléctrico exclusivamente',0);

-- Pregunta 20
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id, (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Prevención'),
       'multiple_choice',
       'La acción más efectiva ante un peligro es, cuando sea posible:',
       'Eliminar el peligro se encuentra en la parte superior de la jerarquía de controles.', 1, 1
WHERE NOT EXISTS (SELECT 1 FROM questions WHERE prompt='La acción más efectiva ante un peligro es, cuando sea posible:');
SET @q20 := (SELECT id FROM questions WHERE prompt='La acción más efectiva ante un peligro es, cuando sea posible:' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q20,0,'Eliminar el peligro',1),(@q20,1,'Ignorarlo',0),(@q20,2,'Depender solo del EPP',0),(@q20,3,'Aumentar la exposición',0);

COMMIT;

-- Verificación rápida
SELECT q.id, t.name AS topic, s.name AS subtopic, q.question_type, q.prompt,
       COUNT(o.id) AS options_count,
       SUM(o.is_correct) AS correct_options
FROM questions q
JOIN topics t ON t.id=q.topic_id
LEFT JOIN subtopics s ON s.id=q.subtopic_id
LEFT JOIN question_options o ON o.question_id=q.id
WHERE t.name='Seguridad en obra'
GROUP BY q.id, t.name, s.name, q.question_type, q.prompt
ORDER BY q.id;
