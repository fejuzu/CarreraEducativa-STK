-- IGH EDUCATIVO - MySQL 8 schema
-- Stores users, question banks, race attempts, answers and history.

CREATE DATABASE IF NOT EXISTS igh_educativo
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE igh_educativo;

CREATE TABLE users (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    employee_code VARCHAR(50) NOT NULL UNIQUE,
    full_name VARCHAR(150) NOT NULL,
    email VARCHAR(190) NULL UNIQUE,
    department VARCHAR(120) NULL,
    pin_hash VARCHAR(255) NOT NULL,
    active TINYINT(1) NOT NULL DEFAULT 1,
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;

CREATE TABLE user_sessions (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    user_id BIGINT UNSIGNED NOT NULL,
    token_hash CHAR(64) NOT NULL UNIQUE,
    expires_at DATETIME NOT NULL,
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_used_at DATETIME NULL,
    CONSTRAINT fk_sessions_user
        FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_sessions_user (user_id),
    INDEX idx_sessions_expires (expires_at)
) ENGINE=InnoDB;

CREATE TABLE topics (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(120) NOT NULL UNIQUE,
    description TEXT NULL,
    active TINYINT(1) NOT NULL DEFAULT 1,
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

CREATE TABLE subtopics (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    topic_id BIGINT UNSIGNED NOT NULL,
    name VARCHAR(120) NOT NULL,
    active TINYINT(1) NOT NULL DEFAULT 1,
    CONSTRAINT fk_subtopics_topic
        FOREIGN KEY (topic_id) REFERENCES topics(id) ON DELETE CASCADE,
    UNIQUE KEY uq_subtopic_topic_name (topic_id, name)
) ENGINE=InnoDB;

CREATE TABLE questions (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    topic_id BIGINT UNSIGNED NOT NULL,
    subtopic_id BIGINT UNSIGNED NULL,
    question_type ENUM('multiple_choice','true_false') NOT NULL DEFAULT 'multiple_choice',
    prompt TEXT NOT NULL,
    explanation TEXT NULL,
    level INT NOT NULL DEFAULT 1,
    active TINYINT(1) NOT NULL DEFAULT 1,
    version INT NOT NULL DEFAULT 1,
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    CONSTRAINT fk_questions_topic
        FOREIGN KEY (topic_id) REFERENCES topics(id),
    CONSTRAINT fk_questions_subtopic
        FOREIGN KEY (subtopic_id) REFERENCES subtopics(id) ON DELETE SET NULL,
    INDEX idx_questions_topic_active (topic_id, active),
    INDEX idx_questions_subtopic (subtopic_id)
) ENGINE=InnoDB;

CREATE TABLE question_options (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    question_id BIGINT UNSIGNED NOT NULL,
    option_order TINYINT UNSIGNED NOT NULL,
    option_text TEXT NOT NULL,
    is_correct TINYINT(1) NOT NULL DEFAULT 0,
    CONSTRAINT fk_options_question
        FOREIGN KEY (question_id) REFERENCES questions(id) ON DELETE CASCADE,
    UNIQUE KEY uq_question_option_order (question_id, option_order)
) ENGINE=InnoDB;

CREATE TABLE attempts (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    user_id BIGINT UNSIGNED NOT NULL,
    topic_id BIGINT UNSIGNED NOT NULL,
    status ENUM('in_progress','completed','abandoned') NOT NULL DEFAULT 'in_progress',
    total_questions SMALLINT UNSIGNED NOT NULL DEFAULT 10,
    answered_count SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    correct_count SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    incorrect_count SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    educational_score INT NOT NULL DEFAULT 0,
    race_position SMALLINT UNSIGNED NULL,
    race_time_ms BIGINT UNSIGNED NULL,
    started_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    finished_at DATETIME NULL,
    CONSTRAINT fk_attempts_user
        FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_attempts_topic
        FOREIGN KEY (topic_id) REFERENCES topics(id),
    INDEX idx_attempts_user_date (user_id, started_at),
    INDEX idx_attempts_topic_date (topic_id, started_at),
    INDEX idx_attempts_status (status)
) ENGINE=InnoDB;

CREATE TABLE attempt_questions (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    attempt_id BIGINT UNSIGNED NOT NULL,
    question_id BIGINT UNSIGNED NOT NULL,
    question_order SMALLINT UNSIGNED NOT NULL,
    answered TINYINT(1) NOT NULL DEFAULT 0,
    CONSTRAINT fk_attempt_questions_attempt
        FOREIGN KEY (attempt_id) REFERENCES attempts(id) ON DELETE CASCADE,
    CONSTRAINT fk_attempt_questions_question
        FOREIGN KEY (question_id) REFERENCES questions(id),
    UNIQUE KEY uq_attempt_question (attempt_id, question_id),
    UNIQUE KEY uq_attempt_order (attempt_id, question_order)
) ENGINE=InnoDB;

CREATE TABLE attempt_answers (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    attempt_id BIGINT UNSIGNED NOT NULL,
    question_id BIGINT UNSIGNED NOT NULL,
    selected_option_id BIGINT UNSIGNED NOT NULL,
    is_correct TINYINT(1) NOT NULL,
    response_time_ms INT UNSIGNED NOT NULL DEFAULT 0,
    points_awarded INT NOT NULL DEFAULT 0,
    answered_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_answers_attempt
        FOREIGN KEY (attempt_id) REFERENCES attempts(id) ON DELETE CASCADE,
    CONSTRAINT fk_answers_question
        FOREIGN KEY (question_id) REFERENCES questions(id),
    CONSTRAINT fk_answers_option
        FOREIGN KEY (selected_option_id) REFERENCES question_options(id),
    UNIQUE KEY uq_attempt_answer_question (attempt_id, question_id),
    INDEX idx_answers_correct (is_correct),
    INDEX idx_answers_date (answered_at)
) ENGINE=InnoDB;

-- Example monthly ranking:
-- Highest educational score accumulated during a month.
-- SELECT u.id, u.full_name,
--        SUM(a.educational_score) AS score,
--        SUM(a.correct_count) AS correct_answers,
--        SUM(a.answered_count) AS answered_questions,
--        COUNT(*) AS races
-- FROM attempts a
-- JOIN users u ON u.id = a.user_id
-- WHERE a.status = 'completed'
--   AND a.finished_at >= '2026-09-01'
--   AND a.finished_at <  '2026-10-01'
-- GROUP BY u.id, u.full_name
-- ORDER BY score DESC, correct_answers DESC;
