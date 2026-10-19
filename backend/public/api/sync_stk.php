<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

function stkSyncEscape(string $value): string
{
    return str_replace(
        ["\\", "|", "\r", "\n"],
        ["\\\\", "\\|", "\\r", "\\n"],
        $value
    );
}

$sql = '
    SELECT
        q.id,
        q.level,
        q.question_type,
        q.prompt,
        q.explanation,
        t.name AS topic,
        COALESCE(s.name, "") AS subtopic,
        (
            SELECT qo.id
            FROM question_options qo
            WHERE qo.question_id = q.id
              AND qo.is_correct = 1
            LIMIT 1
        ) AS correct_option_id
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
    ORDER BY q.id';

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
        stkSyncEscape((string)$question['question_type']),
        stkSyncEscape((string)$question['topic']),
        stkSyncEscape((string)$question['subtopic']),
        (string)$question['correct_option_id'],
        stkSyncEscape((string)$question['prompt']),
        stkSyncEscape((string)($question['explanation'] ?? '')),
    ]) . "\n";

    $optionStmt->execute([
        'question_id' => (int)$question['id'],
    ]);

    foreach ($optionStmt->fetchAll() as $option) {
        echo implode('|', [
            'O',
            (string)$option['id'],
            (string)$option['option_order'],
            stkSyncEscape((string)$option['option_text']),
        ]) . "\n";
    }

    echo "E\n";
}
