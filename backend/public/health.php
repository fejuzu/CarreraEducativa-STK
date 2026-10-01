<?php
declare(strict_types=1);

require dirname(__DIR__) . '/src/bootstrap.php';

$pdo->query('SELECT 1');

jsonResponse([
    'ok' => true,
    'service' => 'IGH EDUCATIVO API',
    'database' => 'connected',
]);
