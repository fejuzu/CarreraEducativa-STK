<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    jsonResponse([
        'ok' => false,
        'error' => 'Method not allowed',
    ], 405);
}

$raw = file_get_contents('php://input');
$data = json_decode($raw ?: '', true);

if (!is_array($data)) {
    jsonResponse([
        'ok' => false,
        'error' => 'Invalid JSON body',
    ], 400);
}

$questionId = isset($data['question_id']) ? (int)$data['question_id'] : 0;
$optionId = isset($data['option_id']) ? (int)$data['option_id'] : 0;

if ($questionId <= 0 || $optionId <= 0) {
    jsonResponse([
        'ok' => false,
        'error' => 'question_id and option_id are required',
    ], 422);
}

$stmt = $pdo->prepare(
    'SELECT
         q.id AS question_id,
         q.prompt,
         q.explanation,
         qo.id AS option_id,
         qo.option_order,
         qo.option_text,
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
    jsonResponse([
        'ok' => false,
        'error' => 'Question or option not found',
    ], 404);
}

$correctStmt = $pdo->prepare(
    'SELECT id, option_order, option_text
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
    jsonResponse([
        'ok' => false,
        'error' => 'Question has no valid correct option',
    ], 500);
}

$isCorrect = (int)$selected['is_correct'] === 1;

jsonResponse([
    'ok' => true,
    'question_id' => (int)$selected['question_id'],
    'selected_option_id' => (int)$selected['option_id'],
    'correct' => $isCorrect,
    'correct_option_id' => (int)$correctOption['id'],
    'correct_option_order' => (int)$correctOption['option_order'],
    'correct_option_text' => $correctOption['option_text'],
    'explanation' => $selected['explanation'],
]);
