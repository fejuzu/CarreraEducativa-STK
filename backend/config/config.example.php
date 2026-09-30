<?php
declare(strict_types=1);

return [
    'db' => [
        'dsn' => 'mysql:host=127.0.0.1;port=3306;dbname=igh_educativo;charset=utf8mb4',
        'user' => 'root',
        'password' => '',
    ],
    'app' => [
        'environment' => 'development',
        'session_days' => 30,
        'questions_per_race' => 10,
        'points_per_correct_answer' => 100,
    ],
];
