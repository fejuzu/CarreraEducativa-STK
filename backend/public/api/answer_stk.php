<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

function stkEscapeAnswer(string $value): string
{
    return str_replace(
        ["\\", "|", "\r", "\n"],
        ["\\\\", "\\|", "\\r", "\\n"],
        $value
    );
}

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    header('Content-Type: text/plain; charset=utf-8');
    echo "ERR|Method not allowed\n";
    exit;
}

$questionId = isset($_POST['question_id']) ? (int)$_POST['question_id'] : 0;
$optionId = isset($_POST['option_id']) ? (int)$_POST['option_id'] : 0;

if ($questionId <= 0 || $optionId <= 0) {
    http_response_code(422);
    header('Content-Type: text/plain; charset=utf-8');
    echo "ERR|question_id and option_id are required\n";
    exit;
}

$stmt = $pdo->prepare(
    'SELECT
         q.id AS question_id,
         q.explanation,
         qo.id AS option_id,
         qo.is_correct
     FROM questions q
     INNER JOIN question_options qo
         ON qo.question_id = q.id
     WHERE q.id = :question_id
       AND qo.id = :option_id
       AND q.active = 1
     LIMIT 1'
);

$stmt->execute([
    'question_id' => $questionId,
    'option_id' => $optionId,
]);

$selected = $stmt->fetch();

if (!$selected) {
    http_response_code(404);
    header('Content-Type: text/plain; charset=utf-8');
    echo "ERR|Question or option not found\n";
    exit;
}

$correctStmt = $pdo->prepare(
    'SELECT id
     FROM question_options
     WHERE question_id = :question_id
       AND is_correct = 1
     LIMIT 1'
);

$correctStmt->execute([
    'question_id' => $questionId,
]);

$correctOption = $correctStmt->fetch();

if (!$correctOption) {
    http_response_code(500);
    header('Content-Type: text/plain; charset=utf-8');
    echo "ERR|Question has no valid correct option\n";
    exit;
}

header('Content-Type: text/plain; charset=utf-8');

echo implode('|', [
    'OK',
    ((int)$selected['is_correct'] === 1) ? '1' : '0',
    (string)$correctOption['id'],
    stkEscapeAnswer((string)($selected['explanation'] ?? '')),
]) . "\n";
