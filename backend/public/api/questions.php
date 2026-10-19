<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

$defaultLimit = (int)($config['app']['questions_per_race'] ?? 10);
$limit = isset($_GET['limit']) ? (int)$_GET['limit'] : $defaultLimit;
$limit = max(1, min(50, $limit));

$topicId = isset($_GET['topic_id']) ? (int)$_GET['topic_id'] : 0;

$where = [
    'q.active = 1',
    't.active = 1',
];

$params = [];

if ($topicId > 0) {
    $where[] = 'q.topic_id = :topic_id';
    $params['topic_id'] = $topicId;
}

$sql = '
    SELECT
        q.id,
        q.topic_id,
        q.subtopic_id,
        q.question_type,
        q.prompt,
        q.level,
        t.name AS topic,
        s.name AS subtopic
    FROM questions q
    INNER JOIN topics t ON t.id = q.topic_id
    LEFT JOIN subtopics s ON s.id = q.subtopic_id
    WHERE ' . implode(' AND ', $where) . '
      AND (
          SELECT COUNT(*)
          FROM question_options qo
          WHERE qo.question_id = q.id
      ) >= 2
      AND (
          SELECT COUNT(*)
          FROM question_options qo
          WHERE qo.question_id = q.id
            AND qo.is_correct = 1
      ) = 1
    ORDER BY RAND()
    LIMIT ' . $limit;

$stmt = $pdo->prepare($sql);
$stmt->execute($params);
$rows = $stmt->fetchAll();

if (!$rows) {
    jsonResponse([
        'ok' => true,
        'count' => 0,
        'questions' => [],
    ]);
}

$questionIds = array_map(
    static fn(array $row): int => (int)$row['id'],
    $rows
);

$placeholders = implode(',', array_fill(0, count($questionIds), '?'));

$optionsStmt = $pdo->prepare(
    'SELECT id, question_id, option_order, option_text
     FROM question_options
     WHERE question_id IN (' . $placeholders . ')
     ORDER BY question_id, option_order'
);

$optionsStmt->execute($questionIds);

$optionsByQuestion = [];

foreach ($optionsStmt->fetchAll() as $option) {
    $questionId = (int)$option['question_id'];

    $optionsByQuestion[$questionId][] = [
        'id' => (int)$option['id'],
        'order' => (int)$option['option_order'],
        'text' => $option['option_text'],
    ];
}

$questions = [];

foreach ($rows as $row) {
    $questionId = (int)$row['id'];

    $questions[] = [
        'id' => $questionId,
        'topic_id' => (int)$row['topic_id'],
        'topic' => $row['topic'],
        'subtopic_id' => $row['subtopic_id'] !== null
            ? (int)$row['subtopic_id']
            : null,
        'subtopic' => $row['subtopic'],
        'type' => $row['question_type'],
        'prompt' => $row['prompt'],
        'level' => (int)$row['level'],
        'options' => $optionsByQuestion[$questionId] ?? [],
    ];
}

jsonResponse([
    'ok' => true,
    'count' => count($questions),
    'questions' => $questions,
]);
