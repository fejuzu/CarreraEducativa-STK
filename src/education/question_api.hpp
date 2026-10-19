//
// Carrera Educativa STK - question API loader
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_QUESTION_API_HPP
#define HEADER_EDUCATION_QUESTION_API_HPP

#include "education/question_manager.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace Education
{
    struct AnswerValidation
    {
        bool correct;
        std::size_t correct_index;
        std::string explanation;

        AnswerValidation()
            : correct(false),
              correct_index(0)
        {
        }
    };

    /**
     * Downloads race questions from the IGH EDUCATIVO API without exposing
     * the correct answer.
     */
    bool loadQuestionsFromApi(const std::string& url,
                              std::vector<Question>* questions,
                              std::string* error);

    /**
     * Sends the selected database option id to PHP/MySQL and returns the
     * server-validated answer.
     */
    bool validateAnswerWithApi(const std::string& url,
                               const Question& question,
                               std::size_t selected_index,
                               AnswerValidation* validation,
                               std::string* error);

    /**
     * Downloads the complete offline-ready bank, including the answer key,
     * and atomically replaces the local SQLite cache.
     */
    bool syncQuestionsToOfflineCache(const std::string& url,
                                     std::size_t* synced_count,
                                     std::string* error);
}

#endif
