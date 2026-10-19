//
// Carrera Educativa STK - offline race result queue
// GPL v3 or later.
//

#include "education/result_queue.hpp"

#include "education/question_cache.hpp"
#include "online/http_request.hpp"

#ifdef ENABLE_SQLITE3
#include <sqlite3.h>
#endif

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

namespace Education
{

#ifdef ENABLE_SQLITE3
namespace
{

struct PendingAnswer
{
    int question_id;
    int option_order;
    std::uint32_t response_time_ms;
};

struct PendingResult
{
    std::string id;
    std::string employee_code;
    int race_position;
    std::uint64_t race_time_ms;
    std::vector<PendingAnswer> answers;
};

bool setError(sqlite3* db, const std::string& prefix, std::string* error)
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

bool openQueueDatabase(sqlite3** db, std::string* error)
{
    if (!db)
        return false;

    *db = NULL;

    const std::string path = getOfflineQuestionCachePath();
    const int rc = sqlite3_open_v2(
        path.c_str(),
        db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        NULL);

    if (rc != SQLITE_OK)
    {
        const bool result = setError(
            *db, "Cannot open offline result queue", error);
        if (*db)
        {
            sqlite3_close(*db);
            *db = NULL;
        }
        return result;
    }

    sqlite3_busy_timeout(*db, 3000);

    const char* schema =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS edu_result_queue ("
        " id TEXT PRIMARY KEY,"
        " employee_code TEXT NOT NULL,"
        " race_position INTEGER NOT NULL DEFAULT 0,"
        " race_time_ms INTEGER NOT NULL DEFAULT 0,"
        " created_at_ms INTEGER NOT NULL,"
        " sync_attempts INTEGER NOT NULL DEFAULT 0,"
        " last_error TEXT NOT NULL DEFAULT ''"
        ");"
        "CREATE TABLE IF NOT EXISTS edu_result_answer_queue ("
        " result_id TEXT NOT NULL,"
        " answer_order INTEGER NOT NULL,"
        " question_id INTEGER NOT NULL,"
        " option_order INTEGER NOT NULL,"
        " response_time_ms INTEGER NOT NULL DEFAULT 0,"
        " PRIMARY KEY(result_id, answer_order),"
        " FOREIGN KEY(result_id) REFERENCES edu_result_queue(id)"
        "     ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_edu_result_created"
        " ON edu_result_queue(created_at_ms);";

    if (!execSql(*db, schema, error))
    {
        sqlite3_close(*db);
        *db = NULL;
        return false;
    }

    return true;
}

std::string makeResultId()
{
    const std::uint64_t now =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    std::random_device rd;
    const unsigned int random_value = rd();

    std::ostringstream out;
    out << "race_" << now << "_"
        << std::hex << std::setw(8) << std::setfill('0') << random_value;
    return out.str();
}

std::vector<std::string> splitSimple(const std::string& value, char delimiter)
{
    std::vector<std::string> parts;
    std::string current;

    for (std::size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == delimiter)
        {
            parts.push_back(current);
            current.clear();
        }
        else
        {
            current += value[i];
        }
    }

    parts.push_back(current);
    return parts;
}

bool markSyncFailure(sqlite3* db,
                     const std::string& id,
                     const std::string& message)
{
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(
        db,
        "UPDATE edu_result_queue "
        "SET sync_attempts = sync_attempts + 1, last_error = ? "
        "WHERE id = ?;",
        -1,
        &stmt,
        NULL);

    if (rc != SQLITE_OK)
        return false;

    const bool ok =
        bindText(stmt, 1, message) &&
        bindText(stmt, 2, id) &&
        sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return ok;
}

bool deleteSyncedResult(sqlite3* db, const std::string& id)
{
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(
        db,
        "DELETE FROM edu_result_queue WHERE id = ?;",
        -1,
        &stmt,
        NULL);

    if (rc != SQLITE_OK)
        return false;

    const bool ok =
        bindText(stmt, 1, id) &&
        sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return ok;
}

bool loadPendingResults(sqlite3* db,
                        std::vector<PendingResult>* pending,
                        std::string* error)
{
    if (!pending)
        return false;

    pending->clear();

    sqlite3_stmt* result_stmt = NULL;
    sqlite3_stmt* answer_stmt = NULL;

    int rc = sqlite3_prepare_v2(
        db,
        "SELECT id, employee_code, race_position, race_time_ms "
        "FROM edu_result_queue "
        "ORDER BY created_at_ms ASC;",
        -1,
        &result_stmt,
        NULL);

    if (rc != SQLITE_OK)
        return setError(db, "Cannot read pending race results", error);

    rc = sqlite3_prepare_v2(
        db,
        "SELECT question_id, option_order, response_time_ms "
        "FROM edu_result_answer_queue "
        "WHERE result_id = ? "
        "ORDER BY answer_order ASC;",
        -1,
        &answer_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(result_stmt);
        return setError(db, "Cannot read pending race answers", error);
    }

    bool ok = true;

    while ((rc = sqlite3_step(result_stmt)) == SQLITE_ROW)
    {
        PendingResult item;

        const unsigned char* id = sqlite3_column_text(result_stmt, 0);
        const unsigned char* employee = sqlite3_column_text(result_stmt, 1);

        item.id = id ? reinterpret_cast<const char*>(id) : "";
        item.employee_code =
            employee ? reinterpret_cast<const char*>(employee) : "";
        item.race_position = sqlite3_column_int(result_stmt, 2);
        item.race_time_ms =
            static_cast<std::uint64_t>(
                sqlite3_column_int64(result_stmt, 3));

        sqlite3_reset(answer_stmt);
        sqlite3_clear_bindings(answer_stmt);

        if (!bindText(answer_stmt, 1, item.id))
        {
            ok = false;
            setError(db, "Cannot bind pending result id", error);
            break;
        }

        int answer_rc = SQLITE_ROW;

        while ((answer_rc = sqlite3_step(answer_stmt)) == SQLITE_ROW)
        {
            PendingAnswer answer;
            answer.question_id = sqlite3_column_int(answer_stmt, 0);
            answer.option_order = sqlite3_column_int(answer_stmt, 1);
            answer.response_time_ms =
                static_cast<std::uint32_t>(
                    sqlite3_column_int64(answer_stmt, 2));

            item.answers.push_back(answer);
        }

        if (answer_rc != SQLITE_DONE || item.answers.empty())
        {
            ok = false;
            setError(db, "Cannot read pending result answers", error);
            break;
        }

        pending->push_back(item);
    }

    if (rc != SQLITE_DONE)
    {
        ok = false;
        setError(db, "Cannot finish reading pending results", error);
    }

    sqlite3_finalize(answer_stmt);
    sqlite3_finalize(result_stmt);

    if (!ok)
        pending->clear();

    return ok;
}

std::string buildAnswersPayload(const PendingResult& result)
{
    std::ostringstream out;

    for (std::size_t i = 0; i < result.answers.size(); i++)
    {
        if (i > 0)
            out << "|";

        out << result.answers[i].question_id << ","
            << result.answers[i].option_order << ","
            << result.answers[i].response_time_ms;
    }

    return out.str();
}

} // anonymous namespace
#endif

bool queueOfflineRaceResult(const QuestionManager& manager,
                            int race_position,
                            std::uint64_t race_time_ms,
                            const std::string& employee_code,
                            std::string* result_id,
                            std::string* error)
{
#ifndef ENABLE_SQLITE3
    if (error)
        *error = "SQLite support is not enabled in this build.";
    return false;
#else
    const std::vector<AnswerResult>& results = manager.getResults();
    const std::vector<Question>& questions = manager.getRaceQuestions();

    if (results.empty() || results.size() != questions.size())
    {
        if (error)
            *error = "Educational race results are incomplete.";
        return false;
    }

    if (employee_code.empty())
    {
        if (error)
            *error = "Employee code is empty.";
        return false;
    }

    sqlite3* db = NULL;
    if (!openQueueDatabase(&db, error))
        return false;

    const std::string id = makeResultId();
    const std::uint64_t created_at_ms =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

    if (!execSql(db, "BEGIN IMMEDIATE TRANSACTION;", error))
    {
        sqlite3_close(db);
        return false;
    }

    sqlite3_stmt* result_stmt = NULL;
    sqlite3_stmt* answer_stmt = NULL;

    int rc = sqlite3_prepare_v2(
        db,
        "INSERT INTO edu_result_queue "
        "(id, employee_code, race_position, race_time_ms, created_at_ms) "
        "VALUES (?, ?, ?, ?, ?);",
        -1,
        &result_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        const bool result = setError(
            db, "Cannot prepare offline race result insert", error);
        sqlite3_close(db);
        return result;
    }

    bool ok =
        bindText(result_stmt, 1, id) &&
        bindText(result_stmt, 2, employee_code) &&
        sqlite3_bind_int(result_stmt, 3, race_position) == SQLITE_OK &&
        sqlite3_bind_int64(
            result_stmt,
            4,
            static_cast<sqlite3_int64>(race_time_ms)) == SQLITE_OK &&
        sqlite3_bind_int64(
            result_stmt,
            5,
            static_cast<sqlite3_int64>(created_at_ms)) == SQLITE_OK &&
        sqlite3_step(result_stmt) == SQLITE_DONE;

    sqlite3_finalize(result_stmt);

    if (!ok)
    {
        setError(db, "Cannot store offline race result", error);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        sqlite3_close(db);
        return false;
    }

    rc = sqlite3_prepare_v2(
        db,
        "INSERT INTO edu_result_answer_queue "
        "(result_id, answer_order, question_id, option_order, "
        " response_time_ms) VALUES (?, ?, ?, ?, ?);",
        -1,
        &answer_stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        const bool result = setError(
            db, "Cannot prepare offline answer insert", error);
        sqlite3_close(db);
        return result;
    }

    for (std::size_t i = 0; i < results.size(); i++)
    {
        const AnswerResult& answer = results[i];

        sqlite3_reset(answer_stmt);
        sqlite3_clear_bindings(answer_stmt);

        ok =
            bindText(answer_stmt, 1, id) &&
            sqlite3_bind_int(
                answer_stmt, 2, static_cast<int>(i + 1)) == SQLITE_OK &&
            sqlite3_bind_int(
                answer_stmt, 3, answer.question_id) == SQLITE_OK &&
            sqlite3_bind_int(
                answer_stmt,
                4,
                static_cast<int>(answer.selected_index)) == SQLITE_OK &&
            sqlite3_bind_int64(
                answer_stmt,
                5,
                static_cast<sqlite3_int64>(
                    answer.response_time_ms)) == SQLITE_OK &&
            sqlite3_step(answer_stmt) == SQLITE_DONE;

        if (!ok)
        {
            setError(db, "Cannot store offline race answer", error);
            break;
        }
    }

    sqlite3_finalize(answer_stmt);

    if (ok)
        ok = execSql(db, "COMMIT;", error);
    else
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

    sqlite3_close(db);

    if (!ok)
        return false;

    if (result_id)
        *result_id = id;

    return true;
#endif
}

bool getPendingRaceResultCount(std::size_t* count,
                               std::string* error)
{
#ifndef ENABLE_SQLITE3
    if (error)
        *error = "SQLite support is not enabled in this build.";
    return false;
#else
    if (!count)
        return false;

    *count = 0;

    sqlite3* db = NULL;
    if (!openQueueDatabase(&db, error))
        return false;

    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(
        db,
        "SELECT COUNT(*) FROM edu_result_queue;",
        -1,
        &stmt,
        NULL);

    if (rc != SQLITE_OK)
    {
        const bool result = setError(
            db, "Cannot count pending race results", error);
        sqlite3_close(db);
        return result;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
        *count = static_cast<std::size_t>(sqlite3_column_int64(stmt, 0));
    else
    {
        sqlite3_finalize(stmt);
        const bool result = setError(
            db, "Cannot read pending race result count", error);
        sqlite3_close(db);
        return result;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
#endif
}

bool syncPendingRaceResults(const std::string& url,
                            std::size_t* synced_count,
                            std::size_t* remaining_count,
                            std::string* error)
{
#ifndef ENABLE_SQLITE3
    if (error)
        *error = "SQLite support is not enabled in this build.";
    return false;
#else
    if (synced_count)
        *synced_count = 0;
    if (remaining_count)
        *remaining_count = 0;

    sqlite3* db = NULL;
    if (!openQueueDatabase(&db, error))
        return false;

    std::vector<PendingResult> pending;
    if (!loadPendingResults(db, &pending, error))
    {
        sqlite3_close(db);
        return false;
    }

    std::size_t synced = 0;
    std::string last_error;

    for (std::size_t i = 0; i < pending.size(); i++)
    {
        const PendingResult& item = pending[i];

        Online::HTTPRequest request;
        request.setURL(url);
        request.addParameter("client_result_id", item.id);
        request.addParameter("employee_code", item.employee_code);
        request.addParameter("race_position", item.race_position);
        request.addParameter(
            "race_time_ms",
            static_cast<unsigned long long>(item.race_time_ms));
        request.addParameter("answers", buildAnswersPayload(item));
        request.executeNow();

        if (request.hadDownloadError())
        {
            last_error = request.getDownloadErrorMessage();
            markSyncFailure(db, item.id, last_error);
            break;
        }

        std::string response = request.getData();
        const std::size_t newline = response.find_first_of("\r\n");
        if (newline != std::string::npos)
            response.erase(newline);

        const std::vector<std::string> fields =
            splitSimple(response, '|');

        if (fields.size() >= 2 && fields[0] == "OK")
        {
            if (deleteSyncedResult(db, item.id))
                synced++;
            else
            {
                last_error = "Server accepted result but local queue cleanup failed.";
                markSyncFailure(db, item.id, last_error);
                break;
            }
        }
        else
        {
            last_error = response.empty()
                ? "Result sync endpoint returned an empty response."
                : response;
            markSyncFailure(db, item.id, last_error);
        }
    }

    sqlite3_close(db);

    std::size_t remaining = 0;
    std::string count_error;
    getPendingRaceResultCount(&remaining, &count_error);

    if (synced_count)
        *synced_count = synced;
    if (remaining_count)
        *remaining_count = remaining;

    if (!last_error.empty())
    {
        if (error)
            *error = last_error;
        return false;
    }

    return true;
#endif
}

} // namespace Education
