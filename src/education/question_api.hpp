//
// Carrera Educativa STK - question API loader
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_QUESTION_API_HPP
#define HEADER_EDUCATION_QUESTION_API_HPP

#include "education/question_manager.hpp"

#include <string>
#include <vector>

namespace Education
{
    /**
     * Downloads a question bank from the IGH EDUCATIVO PHP API.
     *
     * The endpoint uses a compact escaped text format so the game does not
     * need an additional JSON dependency. This first integration keeps the
     * existing QuestionManager contract, including correct_index, so we can
     * prove that races are reading the MySQL bank. Server-side answer
     * validation remains available through answer.php and can be wired next.
     */
    bool loadQuestionsFromApi(const std::string& url,
                              std::vector<Question>* questions,
                              std::string* error);
}

#endif
