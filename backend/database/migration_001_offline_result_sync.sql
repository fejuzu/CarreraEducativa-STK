-- IGH EDUCATIVO - migration 001
-- Adds an idempotency key used when offline race results are synchronized.

USE igh_educativo;

ALTER TABLE attempts
    ADD COLUMN client_sync_id VARCHAR(64) NULL AFTER id;

ALTER TABLE attempts
    ADD UNIQUE KEY uq_attempt_client_sync (client_sync_id);
