//
// Carrera Educativa STK - offline race result queue
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_RESULT_QUEUE_HPP
#define HEADER_EDUCATION_RESULT_QUEUE_HPP

#include "education/question_manager.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace Education
{
    /**
     * Stores one completed educational race in the local SQLite queue.
     * The answers are preserved even if the server is unavailable.
     */
    bool queueOfflineRaceResult(const QuestionManager& manager,
                                int race_position,
                                std::uint64_t race_time_ms,
                                const std::string& employee_code,
                                std::string* result_id,
                                std::string* error);

    /**
     * Sends pending local results to the PHP/MySQL API. Successfully
     * synchronized rows are removed from the queue.
     */
    bool syncPendingRaceResults(const std::string& url,
                                std::size_t* synced_count,
                                std::size_t* remaining_count,
                                std::string* error);

    /** Returns how many completed races are still waiting for synchronization. */
    bool getPendingRaceResultCount(std::size_t* count,
                                   std::string* error);
}

#endif
