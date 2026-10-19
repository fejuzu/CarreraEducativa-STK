-- IGH EDUCATIVO - banco temporal de Cultura General
-- Desactiva el banco anterior y deja activo únicamente Cultura General.
-- No elimina historial ni intentos existentes.

USE igh_educativo;

START TRANSACTION;

-- Desactivar banco anterior sin borrar historial.
UPDATE questions SET active = 0;
UPDATE topics SET active = 0;

-- Crear o reactivar el tema de prueba.
INSERT INTO topics (name, description, active)
SELECT 'Cultura General',
       'Banco temporal para comprobar la sincronización de preguntas entre MySQL y el juego.',
       1
WHERE NOT EXISTS (
    SELECT 1 FROM topics WHERE name = 'Cultura General'
);

UPDATE topics
SET description = 'Banco temporal para comprobar la sincronización de preguntas entre MySQL y el juego.',
    active = 1
WHERE name = 'Cultura General';

SET @topic_id := (
    SELECT id FROM topics WHERE name = 'Cultura General' LIMIT 1
);

INSERT IGNORE INTO subtopics (topic_id, name, active) VALUES
(@topic_id, 'Geografía', 1),
(@topic_id, 'Historia', 1),
(@topic_id, 'Ciencia', 1),
(@topic_id, 'Arte y literatura', 1),
(@topic_id, 'Perú', 1),
(@topic_id, 'Tecnología', 1),
(@topic_id, 'Deportes', 1);

UPDATE subtopics SET active = 1 WHERE topic_id = @topic_id;

-- 1
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Geografía' LIMIT 1),
       'multiple_choice',
       '¿Cuál es el océano más grande del planeta?',
       'El océano Pacífico es el de mayor extensión de la Tierra.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es el océano más grande del planeta?'
);
SET @q1 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es el océano más grande del planeta?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q1,0,'Océano Pacífico',1),(@q1,1,'Océano Atlántico',0),(@q1,2,'Océano Índico',0),(@q1,3,'Océano Ártico',0);

-- 2
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Geografía' LIMIT 1),
       'multiple_choice',
       '¿Cuál es la capital de Japón?',
       'Tokio es la capital de Japón.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es la capital de Japón?'
);
SET @q2 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es la capital de Japón?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q2,0,'Osaka',0),(@q2,1,'Kioto',0),(@q2,2,'Tokio',1),(@q2,3,'Nagoya',0);

-- 3
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Geografía' LIMIT 1),
       'multiple_choice',
       '¿En qué continente se encuentra Egipto?',
       'Egipto se encuentra principalmente en África, aunque la península del Sinaí está en Asia.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿En qué continente se encuentra Egipto?'
);
SET @q3 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿En qué continente se encuentra Egipto?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q3,0,'Europa',0),(@q3,1,'África',1),(@q3,2,'América',0),(@q3,3,'Oceanía',0);

-- 4
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Historia' LIMIT 1),
       'multiple_choice',
       '¿En qué año llegó Cristóbal Colón a América?',
       'El primer viaje de Colón llegó a América en 1492.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿En qué año llegó Cristóbal Colón a América?'
);
SET @q4 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿En qué año llegó Cristóbal Colón a América?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q4,0,'1492',1),(@q4,1,'1453',0),(@q4,2,'1519',0),(@q4,3,'1776',0);

-- 5
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Historia' LIMIT 1),
       'multiple_choice',
       '¿Qué civilización construyó Machu Picchu?',
       'Machu Picchu fue construido por la civilización inca.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Qué civilización construyó Machu Picchu?'
);
SET @q5 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Qué civilización construyó Machu Picchu?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q5,0,'Maya',0),(@q5,1,'Azteca',0),(@q5,2,'Inca',1),(@q5,3,'Romana',0);

-- 6
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Historia' LIMIT 1),
       'true_false',
       '¿Verdadero o falso? La Segunda Guerra Mundial terminó en 1945.',
       'La Segunda Guerra Mundial terminó en 1945.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? La Segunda Guerra Mundial terminó en 1945.'
);
SET @q6 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? La Segunda Guerra Mundial terminó en 1945.' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q6,0,'Verdadero',1),(@q6,1,'Falso',0);

-- 7
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ciencia' LIMIT 1),
       'multiple_choice',
       '¿Cuál es el planeta más cercano al Sol?',
       'Mercurio es el planeta más cercano al Sol.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es el planeta más cercano al Sol?'
);
SET @q7 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es el planeta más cercano al Sol?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q7,0,'Venus',0),(@q7,1,'Mercurio',1),(@q7,2,'Marte',0),(@q7,3,'Tierra',0);

-- 8
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ciencia' LIMIT 1),
       'multiple_choice',
       '¿Qué gas necesitan principalmente los seres humanos para respirar?',
       'El oxígeno es esencial para la respiración celular humana.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Qué gas necesitan principalmente los seres humanos para respirar?'
);
SET @q8 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Qué gas necesitan principalmente los seres humanos para respirar?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q8,0,'Oxígeno',1),(@q8,1,'Helio',0),(@q8,2,'Hidrógeno',0),(@q8,3,'Metano',0);

-- 9
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ciencia' LIMIT 1),
       'multiple_choice',
       '¿Cuál es la fórmula química del agua?',
       'Una molécula de agua está formada por dos átomos de hidrógeno y uno de oxígeno.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es la fórmula química del agua?'
);
SET @q9 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es la fórmula química del agua?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q9,0,'CO2',0),(@q9,1,'H2O',1),(@q9,2,'O2',0),(@q9,3,'NaCl',0);

-- 10
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ciencia' LIMIT 1),
       'true_false',
       '¿Verdadero o falso? El Sol es una estrella.',
       'El Sol es la estrella situada en el centro del sistema solar.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? El Sol es una estrella.'
);
SET @q10 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? El Sol es una estrella.' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q10,0,'Verdadero',1),(@q10,1,'Falso',0);

-- 11
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Arte y literatura' LIMIT 1),
       'multiple_choice',
       '¿Quién escribió Don Quijote de la Mancha?',
       'Miguel de Cervantes Saavedra escribió Don Quijote de la Mancha.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Quién escribió Don Quijote de la Mancha?'
);
SET @q11 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Quién escribió Don Quijote de la Mancha?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q11,0,'Gabriel García Márquez',0),(@q11,1,'Miguel de Cervantes',1),(@q11,2,'Pablo Neruda',0),(@q11,3,'Jorge Luis Borges',0);

-- 12
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Arte y literatura' LIMIT 1),
       'multiple_choice',
       '¿Quién pintó la Mona Lisa?',
       'La Mona Lisa es una obra de Leonardo da Vinci.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Quién pintó la Mona Lisa?'
);
SET @q12 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Quién pintó la Mona Lisa?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q12,0,'Vincent van Gogh',0),(@q12,1,'Pablo Picasso',0),(@q12,2,'Leonardo da Vinci',1),(@q12,3,'Claude Monet',0);

-- 13
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Perú' LIMIT 1),
       'multiple_choice',
       '¿Cuál es la capital del Perú?',
       'Lima es la capital de la República del Perú.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es la capital del Perú?'
);
SET @q13 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es la capital del Perú?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q13,0,'Cusco',0),(@q13,1,'Arequipa',0),(@q13,2,'Lima',1),(@q13,3,'Trujillo',0);

-- 14
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Perú' LIMIT 1),
       'multiple_choice',
       '¿En qué región del Perú se encuentra Machu Picchu?',
       'Machu Picchu se encuentra en la región Cusco.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿En qué región del Perú se encuentra Machu Picchu?'
);
SET @q14 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿En qué región del Perú se encuentra Machu Picchu?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q14,0,'Cusco',1),(@q14,1,'Ica',0),(@q14,2,'Piura',0),(@q14,3,'Tacna',0);

-- 15
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Perú' LIMIT 1),
       'multiple_choice',
       '¿Cuál es la moneda oficial del Perú?',
       'La moneda oficial del Perú es el sol.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es la moneda oficial del Perú?'
);
SET @q15 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es la moneda oficial del Perú?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q15,0,'Peso',0),(@q15,1,'Dólar',0),(@q15,2,'Sol',1),(@q15,3,'Bolívar',0);

-- 16
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Tecnología' LIMIT 1),
       'multiple_choice',
       '¿Qué significan las siglas CPU en informática?',
       'CPU significa Unidad Central de Procesamiento.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Qué significan las siglas CPU en informática?'
);
SET @q16 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Qué significan las siglas CPU en informática?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q16,0,'Unidad Central de Procesamiento',1),(@q16,1,'Control Principal de Usuario',0),(@q16,2,'Unidad de Conexión Pública',0),(@q16,3,'Programa Central Universal',0);

-- 17
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Tecnología' LIMIT 1),
       'true_false',
       '¿Verdadero o falso? Un byte está compuesto por 8 bits.',
       'En informática moderna, un byte está formado por 8 bits.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? Un byte está compuesto por 8 bits.'
);
SET @q17 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Verdadero o falso? Un byte está compuesto por 8 bits.' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q17,0,'Verdadero',1),(@q17,1,'Falso',0);

-- 18
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Deportes' LIMIT 1),
       'multiple_choice',
       '¿Cuántos jugadores tiene un equipo de fútbol en el campo al comenzar un partido?',
       'Un equipo de fútbol comienza con 11 jugadores en el campo.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuántos jugadores tiene un equipo de fútbol en el campo al comenzar un partido?'
);
SET @q18 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuántos jugadores tiene un equipo de fútbol en el campo al comenzar un partido?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q18,0,'9',0),(@q18,1,'10',0),(@q18,2,'11',1),(@q18,3,'12',0);

-- 19
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Geografía' LIMIT 1),
       'multiple_choice',
       '¿Cuál es el país más grande del mundo por superficie?',
       'Rusia es el país con mayor superficie territorial del mundo.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es el país más grande del mundo por superficie?'
);
SET @q19 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es el país más grande del mundo por superficie?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q19,0,'Canadá',0),(@q19,1,'China',0),(@q19,2,'Estados Unidos',0),(@q19,3,'Rusia',1);

-- 20
INSERT INTO questions (topic_id, subtopic_id, question_type, prompt, explanation, level, active)
SELECT @topic_id,
       (SELECT id FROM subtopics WHERE topic_id=@topic_id AND name='Ciencia' LIMIT 1),
       'multiple_choice',
       '¿Cuál es el órgano más grande del cuerpo humano?',
       'La piel es el órgano más grande del cuerpo humano.',
       1, 1
WHERE NOT EXISTS (
    SELECT 1 FROM questions
    WHERE topic_id=@topic_id AND prompt='¿Cuál es el órgano más grande del cuerpo humano?'
);
SET @q20 := (SELECT id FROM questions WHERE topic_id=@topic_id AND prompt='¿Cuál es el órgano más grande del cuerpo humano?' LIMIT 1);
INSERT IGNORE INTO question_options (question_id, option_order, option_text, is_correct) VALUES
(@q20,0,'Corazón',0),(@q20,1,'Hígado',0),(@q20,2,'Piel',1),(@q20,3,'Pulmón',0);

-- Reactivar todas las preguntas del tema de prueba por si el script se ejecuta otra vez.
UPDATE questions SET active = 1 WHERE topic_id = @topic_id;

COMMIT;

-- Verificación: deben quedar 20 preguntas activas y todas de Cultura General.
SELECT
    q.id,
    t.name AS topic,
    s.name AS subtopic,
    q.question_type,
    q.prompt,
    COUNT(o.id) AS options_count,
    SUM(o.is_correct) AS correct_options
FROM questions q
JOIN topics t ON t.id = q.topic_id
LEFT JOIN subtopics s ON s.id = q.subtopic_id
LEFT JOIN question_options o ON o.question_id = q.id
WHERE q.active = 1
  AND t.active = 1
GROUP BY q.id, t.name, s.name, q.question_type, q.prompt
ORDER BY q.id;
