<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

function stkEscape(string $value): string
{
    return str_replace(
        ["\\", "|", "\r", "\n"],
        ["\\\\", "\\|", "\\r", "\\n"],
        $value
    );
}

$defaultLimit = (int)($config['app']['questions_per_race'] ?? 10);
$limit = isset($_GET['limit']) ? (int)$_GET['limit'] : $defaultLimit;
$limit = max(1, min(50, $limit));

$sql = '
    SELECT
        q.id,
        q.level,
        q.question_type,
        q.prompt,
        t.name AS topic,
        COALESCE(s.name, "") AS subtopic
    FROM questions q
    INNER JOIN topics t ON t.id = q.topic_id
    LEFT JOIN subtopics s ON s.id = q.subtopic_id
    WHERE q.active = 1
      AND t.active = 1
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

$questions = $pdo->query($sql)->fetchAll();

header('Content-Type: text/plain; charset=utf-8');

echo 'OK|' . count($questions) . "\n";

$optionStmt = $pdo->prepare(
    'SELECT id, option_order, option_text
     FROM question_options
     WHERE question_id = :question_id
     ORDER BY option_order'
);

foreach ($questions as $question) {
    echo implode('|', [
        'Q',
        (string)$question['id'],
        (string)$question['level'],
        stkEscape((string)$question['question_type']),
        stkEscape((string)$question['topic']),
        stkEscape((string)$question['subtopic']),
        stkEscape((string)$question['prompt']),
    ]) . "\n";

    $optionStmt->execute([
        'question_id' => (int)$question['id'],
    ]);

    foreach ($optionStmt->fetchAll() as $option) {
        echo implode('|', [
            'O',
            (string)$option['id'],
            (string)$option['option_order'],
            stkEscape((string)$option['option_text']),
        ]) . "\n";
    }

    echo "E\n";
}
