//
// Carrera Educativa STK - offline question cache
// GPL v3 or later.
//

#include "education/question_cache.hpp"

#include "io/file_manager.hpp"

#ifdef ENABLE_SQLITE3
#include <sqlite3.h>
#endif

#include <sstream>

namespace Education
{

std::string getOfflineQuestionCachePath()
{
    return file_manager->getUserConfigFile("igh_educativo_cache.sqlite");
}

#ifdef ENABLE_SQLITE3
namespace
{

bool setSqliteError(sqlite3* db, const std::string& prefix, std::string* error)
{
    if (error)
    {
        *error = prefix;
        if (db)
        {
            *error += ": ";
            *error += sqlite3_errmsg(db);
        }
    }
    return false;
}

bool execSql(sqlite3* db, const char* sql, std::string* error)
{
    char* message = NULL;
    const int rc = sqlite3_exec(db, sql, NULL, NULL, &message);
    if (rc == SQLITE_OK)
        return true;

    if (error)
    {
        *error = "SQLite error";
        if (message)
        {
            *error += ": ";
            *error += message;
        }
    }

    if (message)
        sqlite3_free(message);
    return false;
}

bool bindText(sqlite3_stmt* stmt, int index, const std::string& value)
{
    return sqlite3_bind_text(
        stmt, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

} // anonymous namespace
#endif

bool saveOfflineQuestionCache(const std::vector<Question>& questions,
                              std::string* error)
{
#ifndef ENABLE_SQLITE3
    if (error)
        *error = "SQLite support is not enabled in this build.";
    return false;
#else
    if (questions.empty())
    {
        if (error)
            *error = "Cannot save an empty offline question bank.";
        return false;
    }

    sqlite3* db = NULL;
    const std::string path = getOfflineQuestionCachePath();

    int rc = sqlite3_open_v2(
        path.c_str(),
        &db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        NULL);

    if (rc != SQLITE_OK)
    {
        const bool result = setSqliteError(
            db, "Cannot open offline question cache", error);
        if (db)
            sqlite3_close(db);
        return result;
    }

    sqlite3_busy_timeout(db, 3000);

    const char* schema =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS edu_question_cache ("
        " id INTEGER PRIMARY KEY,"
        " level INTEGER NOT NULL,"
        " type TEXT NOT NULL,"
        " topic TEXT NOT NULL,"
        " subtopic TEXT NOT NULL DEFAULT '',"
        " prompt TEXT NOT NULL,"
        " explanation TEXT NOT NULL DEFAULT '',"
        " correct_option_id INTEGER NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS edu_option_cache ("
        " id INTEGER PRIMARY KEY,"
        " question_id INTEGER NOT NULL,"
        " option_order INTEGER NOT NULL,"
        " option_text TEXT NOT NULL,"
        " FOREIGN KEY(question_id) REFERENCES edu_question_cache(id)"
        "     ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_edu_option_question"
        " ON edu_option_cache(question_id, option_order);"
        "CREATE TABLE IF NOT EXISTS edu_cache_meta ("
        " meta_key TEXT PRIMARY KEY,"
        " meta_value TEXT NOT NULL"
        ");";

    if (!execSql(db, schema, error) ||
        !execSql(db, "BEGIN IMMEDIATE TRANSACTION;", error) ||
        !execSql(db, "DELETE FROM edu_option_cache;", error) ||
        !execSql(db, "DELETE FROM edu_question_cache;", error))
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        sqlite3_close(db);
        return false;
    }

    sqlite3_stmt* question_stmt = NULL;
    sqlite3_stmt* option_stmt = NULL;

    rc = sqlite3_prepare_v2(
        db,
        "INSERT INTO edu_question_cache "
        "(id, level, type, topic, subtopic, prompt, explanation, "
        " correct_option_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
        -1,
        &question_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        const bool result = setSqliteError(
            db, "Cannot prepare offline question insert", error);
        sqlite3_close(db);
        return result;
    }

    rc = sqlite3_prepare_v2(
        db,
        "INSERT INTO edu_option_cache "
        "(id, question_id, option_order, option_text) VALUES (?, ?, ?, ?);",
        -1,
        &option_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(question_stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        const bool result = setSqliteError(
            db, "Cannot prepare offline option insert", error);
        sqlite3_close(db);
        return result;
    }

    bool ok = true;

    for (std::size_t q = 0; q < questions.size() && ok; q++)
    {
        const Question& question = questions[q];

        if (question.id <= 0 ||
            question.options.size() < 2 ||
            question.option_ids.size() != question.options.size() ||
            !question.has_offline_answer_key ||
            question.correct_index >= question.option_ids.size())
        {
            if (error)
                *error = "Server returned a question that is not offline-ready.";
            ok = false;
            break;
        }

        const int correct_option_id =
            question.option_ids[question.correct_index];
        const std::string type =
            question.type == QuestionType::TRUE_FALSE
                ? "true_false"
                : "multiple_choice";

        sqlite3_reset(question_stmt);
        sqlite3_clear_bindings(question_stmt);

        ok =
            sqlite3_bind_int(question_stmt, 1, question.id) == SQLITE_OK &&
            sqlite3_bind_int(question_stmt, 2, question.level) == SQLITE_OK &&
            bindText(question_stmt, 3, type) &&
            bindText(question_stmt, 4, question.topic) &&
            bindText(question_stmt, 5, question.subtopic) &&
            bindText(question_stmt, 6, question.prompt) &&
            bindText(question_stmt, 7, question.explanation) &&
            sqlite3_bind_int(
                question_stmt, 8, correct_option_id) == SQLITE_OK &&
            sqlite3_step(question_stmt) == SQLITE_DONE;

        if (!ok)
        {
            setSqliteError(db, "Cannot write offline question", error);
            break;
        }

        for (std::size_t i = 0; i < question.options.size(); i++)
        {
            sqlite3_reset(option_stmt);
            sqlite3_clear_bindings(option_stmt);

            ok =
                sqlite3_bind_int(
                    option_stmt, 1, question.option_ids[i]) == SQLITE_OK &&
                sqlite3_bind_int(
                    option_stmt, 2, question.id) == SQLITE_OK &&
                sqlite3_bind_int(
                    option_stmt, 3, static_cast<int>(i)) == SQLITE_OK &&
                bindText(option_stmt, 4, question.options[i]) &&
                sqlite3_step(option_stmt) == SQLITE_DONE;

            if (!ok)
            {
                setSqliteError(db, "Cannot write offline option", error);
                break;
            }
        }
    }

    sqlite3_finalize(option_stmt);
    sqlite3_finalize(question_stmt);

    if (ok)
    {
        ok = execSql(
            db,
            "INSERT OR REPLACE INTO edu_cache_meta(meta_key, meta_value) "
            "VALUES('schema_version', '1');",
            error);
    }

    if (ok)
        ok = execSql(db, "COMMIT;", error);
    else
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

    sqlite3_close(db);
    return ok;
#endif
}

bool loadOfflineQuestionCache(std::vector<Question>* questions,
                              std::string* error)
{
#ifndef ENABLE_SQLITE3
    if (error)
        *error = "SQLite support is not enabled in this build.";
    return false;
#else
    if (!questions)
    {
        if (error)
            *error = "Question output pointer is null.";
        return false;
    }

    questions->clear();

    sqlite3* db = NULL;
    const std::string path = getOfflineQuestionCachePath();

    int rc = sqlite3_open_v2(
        path.c_str(),
        &db,
        SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
        NULL);

    if (rc != SQLITE_OK)
    {
        const bool result = setSqliteError(
            db, "Offline question cache is not available", error);
        if (db)
            sqlite3_close(db);
        return result;
    }

    sqlite3_stmt* question_stmt = NULL;
    sqlite3_stmt* option_stmt = NULL;

    rc = sqlite3_prepare_v2(
        db,
        "SELECT id, level, type, topic, subtopic, prompt, explanation, "
        "correct_option_id "
        "FROM edu_question_cache ORDER BY id;",
        -1,
        &question_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        const bool result = setSqliteError(
            db, "Cannot read offline question cache", error);
        sqlite3_close(db);
        return result;
    }

    rc = sqlite3_prepare_v2(
        db,
        "SELECT id, option_order, option_text "
        "FROM edu_option_cache "
        "WHERE question_id = ? "
        "ORDER BY option_order;",
        -1,
        &option_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(question_stmt);
        const bool result = setSqliteError(
            db, "Cannot read offline option cache", error);
        sqlite3_close(db);
        return result;
    }

    bool ok = true;

    while ((rc = sqlite3_step(question_stmt)) == SQLITE_ROW)
    {
        Question question;
        question.id = sqlite3_column_int(question_stmt, 0);
        question.level = sqlite3_column_int(question_stmt, 1);

        const unsigned char* type = sqlite3_column_text(question_stmt, 2);
        const unsigned char* topic = sqlite3_column_text(question_stmt, 3);
        const unsigned char* subtopic = sqlite3_column_text(question_stmt, 4);
        const unsigned char* prompt = sqlite3_column_text(question_stmt, 5);
        const unsigned char* explanation = sqlite3_column_text(question_stmt, 6);
        const int correct_option_id = sqlite3_column_int(question_stmt, 7);

        question.type =
            type && std::string(reinterpret_cast<const char*>(type)) ==
                        "true_false"
                ? QuestionType::TRUE_FALSE
                : QuestionType::MULTIPLE_CHOICE;
        question.topic =
            topic ? reinterpret_cast<const char*>(topic) : "";
        question.subtopic =
            subtopic ? reinterpret_cast<const char*>(subtopic) : "";
        question.prompt =
            prompt ? reinterpret_cast<const char*>(prompt) : "";
        question.explanation =
            explanation ? reinterpret_cast<const char*>(explanation) : "";
        question.server_validated = true;
        question.has_offline_answer_key = true;

        sqlite3_reset(option_stmt);
        sqlite3_clear_bindings(option_stmt);

        if (sqlite3_bind_int(option_stmt, 1, question.id) != SQLITE_OK)
        {
            ok = false;
            setSqliteError(db, "Cannot bind cached question id", error);
            break;
        }

        std::size_t correct_index = 0;
        bool correct_found = false;

        int option_rc = SQLITE_ROW;
        while ((option_rc = sqlite3_step(option_stmt)) == SQLITE_ROW)
        {
            const int option_id = sqlite3_column_int(option_stmt, 0);
            const unsigned char* option_text =
                sqlite3_column_text(option_stmt, 2);

            if (option_id == correct_option_id)
            {
                correct_index = question.options.size();
                correct_found = true;
            }

            question.option_ids.push_back(option_id);
            question.options.push_back(
                option_text
                    ? reinterpret_cast<const char*>(option_text)
                    : "");
        }

        if (option_rc != SQLITE_DONE)
        {
            ok = false;
            setSqliteError(db, "Cannot read cached options", error);
            break;
        }

        if (!correct_found ||
            question.options.size() < 2 ||
            question.option_ids.size() != question.options.size())
        {
            if (error)
                *error = "Offline question cache contains incomplete data.";
            ok = false;
            break;
        }

        question.correct_index = correct_index;
        questions->push_back(question);
    }

    if (rc != SQLITE_DONE)
    {
        ok = false;
        setSqliteError(db, "Cannot finish reading cached questions", error);
    }

    sqlite3_finalize(option_stmt);
    sqlite3_finalize(question_stmt);
    sqlite3_close(db);

    if (!ok)
    {
        questions->clear();
        return false;
    }

    if (questions->size() < QuestionManager::QUESTIONS_PER_RACE)
    {
        if (error)
            *error = "Offline cache contains fewer than 10 questions.";
        questions->clear();
        return false;
    }

    return true;
#endif
}

} // namespace Education
