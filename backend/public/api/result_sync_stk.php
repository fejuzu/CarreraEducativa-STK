<?php
declare(strict_types=1);

require dirname(__DIR__, 2) . '/src/bootstrap.php';

function stkResultError(string $message, int $status = 400): void
{
    http_response_code($status);
    header('Content-Type: text/plain; charset=utf-8');
    echo 'ERR|' . str_replace(["\r", "\n", "|"], ' ', $message) . "\n";
    exit;
}

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    stkResultError('Method not allowed', 405);
}

$clientSyncId = trim((string)($_POST['client_result_id'] ?? ''));
$employeeCode = trim((string)($_POST['employee_code'] ?? ''));
$racePosition = isset($_POST['race_position']) ? (int)$_POST['race_position'] : 0;
$raceTimeMs = isset($_POST['race_time_ms']) ? (int)$_POST['race_time_ms'] : 0;
$answersRaw = trim((string)($_POST['answers'] ?? ''));

if ($clientSyncId === '' || strlen($clientSyncId) > 64 ||
    !preg_match('/^[A-Za-z0-9_-]+$/', $clientSyncId)) {
    stkResultError('Invalid client_result_id', 422);
}

if ($employeeCode === '' || strlen($employeeCode) > 50) {
    stkResultError('Invalid employee_code', 422);
}

if ($racePosition < 0 || $raceTimeMs < 0 || $answersRaw === '') {
    stkResultError('Invalid race result payload', 422);
}

$answerTokens = explode('|', $answersRaw);
$answers = [];

foreach ($answerTokens as $order => $token) {
    $parts = explode(',', $token);

    if (count($parts) !== 3) {
        stkResultError('Invalid answer payload', 422);
    }

    $questionId = (int)$parts[0];
    $optionOrder = (int)$parts[1];
    $responseTimeMs = (int)$parts[2];

    if ($questionId <= 0 || $optionOrder < 0 || $responseTimeMs < 0) {
        stkResultError('Invalid answer values', 422);
    }

    $answers[] = [
        'question_id' => $questionId,
        'option_order' => $optionOrder,
        'response_time_ms' => $responseTimeMs,
        'question_order' => $order + 1,
    ];
}

if (count($answers) < 1 || count($answers) > 50) {
    stkResultError('Invalid answer count', 422);
}

$existingStmt = $pdo->prepare(
    'SELECT id
     FROM attempts
     WHERE client_sync_id = :client_sync_id
     LIMIT 1'
);
$existingStmt->execute([
    'client_sync_id' => $clientSyncId,
]);
$existing = $existingStmt->fetch();

if ($existing) {
    header('Content-Type: text/plain; charset=utf-8');
    echo 'OK|' . (int)$existing['id'] . "|DUPLICATE\n";
    exit;
}

$userStmt = $pdo->prepare(
    'SELECT id
     FROM users
     WHERE employee_code = :employee_code
       AND active = 1
     LIMIT 1'
);
$userStmt->execute([
    'employee_code' => $employeeCode,
]);
$user = $userStmt->fetch();

if (!$user) {
    $environment = (string)($config['app']['environment'] ?? 'production');

    if ($environment !== 'development') {
        stkResultError('Unknown or inactive user', 401);
    }

    // Development-only identity so Windows/XAMPP can test offline syncing
    // before the real login flow is connected.
    $insertUser = $pdo->prepare(
        'INSERT INTO users
            (employee_code, full_name, email, department, pin_hash, active)
         VALUES
            (:employee_code, :full_name, NULL, :department, :pin_hash, 1)'
    );

    try {
        $insertUser->execute([
            'employee_code' => $employeeCode,
            'full_name' => 'Jugador local de prueba',
            'department' => 'Desarrollo',
            'pin_hash' => password_hash('0000', PASSWORD_DEFAULT),
        ]);
        $userId = (int)$pdo->lastInsertId();
    } catch (PDOException $e) {
        $userStmt->execute([
            'employee_code' => $employeeCode,
        ]);
        $user = $userStmt->fetch();

        if (!$user) {
            stkResultError('Could not resolve development user', 500);
        }

        $userId = (int)$user['id'];
    }
} else {
    $userId = (int)$user['id'];
}

$optionStmt = $pdo->prepare(
    'SELECT
         q.topic_id,
         qo.id AS option_id,
         qo.is_correct
     FROM questions q
     INNER JOIN question_options qo
         ON qo.question_id = q.id
     WHERE q.id = :question_id
       AND qo.option_order = :option_order
     LIMIT 1'
);

$validatedAnswers = [];
$topicId = 0;
$correctCount = 0;
$incorrectCount = 0;
$pointsPerCorrect = (int)($config['app']['points_per_correct_answer'] ?? 100);

foreach ($answers as $answer) {
    $optionStmt->execute([
        'question_id' => $answer['question_id'],
        'option_order' => $answer['option_order'],
    ]);

    $row = $optionStmt->fetch();

    if (!$row) {
        stkResultError(
            'Question or selected option no longer exists: ' .
            $answer['question_id'],
            409
        );
    }

    $answerTopicId = (int)$row['topic_id'];

    if ($topicId === 0) {
        $topicId = $answerTopicId;
    } elseif ($topicId !== $answerTopicId) {
        stkResultError('A race result cannot mix different topics', 422);
    }

    $isCorrect = (int)$row['is_correct'] === 1;

    if ($isCorrect) {
        $correctCount++;
    } else {
        $incorrectCount++;
    }

    $validatedAnswers[] = [
        'question_id' => $answer['question_id'],
        'question_order' => $answer['question_order'],
        'option_id' => (int)$row['option_id'],
        'is_correct' => $isCorrect ? 1 : 0,
        'response_time_ms' => $answer['response_time_ms'],
        'points_awarded' => $isCorrect ? $pointsPerCorrect : 0,
    ];
}

if ($topicId <= 0) {
    stkResultError('Could not determine topic', 422);
}

$educationalScore = $correctCount * $pointsPerCorrect;

try {
    $pdo->beginTransaction();

    // Re-check inside the transaction for safe retry/idempotency.
    $existingStmt->execute([
        'client_sync_id' => $clientSyncId,
    ]);
    $existing = $existingStmt->fetch();

    if ($existing) {
        $pdo->rollBack();
        header('Content-Type: text/plain; charset=utf-8');
        echo 'OK|' . (int)$existing['id'] . "|DUPLICATE\n";
        exit;
    }

    $attemptStmt = $pdo->prepare(
        'INSERT INTO attempts
            (client_sync_id, user_id, topic_id, status, total_questions,
             answered_count, correct_count, incorrect_count,
             educational_score, race_position, race_time_ms,
             started_at, finished_at)
         VALUES
            (:client_sync_id, :user_id, :topic_id, "completed",
             :total_questions, :answered_count, :correct_count,
             :incorrect_count, :educational_score, :race_position,
             :race_time_ms, NOW(), NOW())'
    );

    $attemptStmt->execute([
        'client_sync_id' => $clientSyncId,
        'user_id' => $userId,
        'topic_id' => $topicId,
        'total_questions' => count($validatedAnswers),
        'answered_count' => count($validatedAnswers),
        'correct_count' => $correctCount,
        'incorrect_count' => $incorrectCount,
        'educational_score' => $educationalScore,
        'race_position' => $racePosition > 0 ? $racePosition : null,
        'race_time_ms' => $raceTimeMs > 0 ? $raceTimeMs : null,
    ]);

    $attemptId = (int)$pdo->lastInsertId();

    $attemptQuestionStmt = $pdo->prepare(
        'INSERT INTO attempt_questions
            (attempt_id, question_id, question_order, answered)
         VALUES
            (:attempt_id, :question_id, :question_order, 1)'
    );

    $attemptAnswerStmt = $pdo->prepare(
        'INSERT INTO attempt_answers
            (attempt_id, question_id, selected_option_id, is_correct,
             response_time_ms, points_awarded)
         VALUES
            (:attempt_id, :question_id, :selected_option_id, :is_correct,
             :response_time_ms, :points_awarded)'
    );

    foreach ($validatedAnswers as $answer) {
        $attemptQuestionStmt->execute([
            'attempt_id' => $attemptId,
            'question_id' => $answer['question_id'],
            'question_order' => $answer['question_order'],
        ]);

        $attemptAnswerStmt->execute([
            'attempt_id' => $attemptId,
            'question_id' => $answer['question_id'],
            'selected_option_id' => $answer['option_id'],
            'is_correct' => $answer['is_correct'],
            'response_time_ms' => $answer['response_time_ms'],
            'points_awarded' => $answer['points_awarded'],
        ]);
    }

    $pdo->commit();

    header('Content-Type: text/plain; charset=utf-8');
    echo 'OK|' . $attemptId . "|SYNCED\n";
} catch (Throwable $e) {
    if ($pdo->inTransaction()) {
        $pdo->rollBack();
    }

    stkResultError('Could not save synchronized race result', 500);
}
