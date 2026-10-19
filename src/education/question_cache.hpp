//
// Carrera Educativa STK - offline question cache
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_QUESTION_CACHE_HPP
#define HEADER_EDUCATION_QUESTION_CACHE_HPP

#include "education/question_manager.hpp"

#include <string>
#include <vector>

namespace Education
{
    /**
     * Replaces the local SQLite question cache with a complete synchronized
     * bank from the server. The replacement is transactional.
     */
    bool saveOfflineQuestionCache(const std::vector<Question>& questions,
                                  std::string* error);

    /**
     * Loads the locally cached question bank. Cached questions contain both
     * database option ids and a local answer key for offline validation.
     */
    bool loadOfflineQuestionCache(std::vector<Question>* questions,
                                  std::string* error);

    /** Returns the platform-specific writable cache path. */
    std::string getOfflineQuestionCachePath();
}

#endif
