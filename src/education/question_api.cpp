//
// Carrera Educativa STK - question API loader
// GPL v3 or later.
//

#include "education/question_api.hpp"

#include "online/http_request.hpp"

#include <cstdlib>
#include <sstream>

namespace Education
{
namespace
{
std::vector<std::string> splitEscaped(const std::string& line)
{
    std::vector<std::string> fields;
    std::string current;
    bool escaped = false;

    for (std::size_t i = 0; i < line.size(); i++)
    {
        const char c = line[i];

        if (escaped)
        {
            switch (c)
            {
                case 'n': current += '\n'; break;
                case 'r': current += '\r'; break;
                case '|': current += '|'; break;
                case '\\': current += '\\'; break;
                default:
                    current += '\\';
                    current += c;
                    break;
            }
            escaped = false;
            continue;
        }

        if (c == '\\')
        {
            escaped = true;
            continue;
        }

        if (c == '|')
        {
            fields.push_back(current);
            current.clear();
            continue;
        }

        current += c;
    }

    if (escaped)
        current += '\\';

    fields.push_back(current);
    return fields;
}
}

bool loadQuestionsFromApi(const std::string& url,
                          std::vector<Question>* questions,
                          std::string* error)
{
    if (!questions)
    {
        if (error)
            *error = "Question output pointer is null.";
        return false;
    }

    questions->clear();

    Online::HTTPRequest request;
    request.setURL(url);
    request.setDownloadAssetsRequest(true);
    request.executeNow();

    if (request.hadDownloadError())
    {
        if (error)
            *error = request.getDownloadErrorMessage();
        return false;
    }

    const std::string& data = request.getData();
    if (data.empty())
    {
        if (error)
            *error = "Question API returned an empty response.";
        return false;
    }

    std::istringstream stream(data);
    std::string line;

    if (!std::getline(stream, line))
    {
        if (error)
            *error = "Question API response has no header.";
        return false;
    }

    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    const std::vector<std::string> header = splitEscaped(line);
    if (header.size() < 2 || header[0] != "OK")
    {
        if (error)
            *error = "Question API returned an invalid header.";
        return false;
    }

    Question current;
    bool in_question = false;
    int correct_order = -1;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        const std::vector<std::string> fields = splitEscaped(line);
        if (fields.empty())
            continue;

        if (fields[0] == "Q")
        {
            if (in_question)
            {
                if (error)
                    *error = "Question API response contains a nested question.";
                questions->clear();
                return false;
            }

            if (fields.size() != 9)
            {
                if (error)
                    *error = "Question API returned an invalid question row.";
                questions->clear();
                return false;
            }

            current = Question();
            current.id = std::atoi(fields[1].c_str());
            current.level = std::atoi(fields[2].c_str());
            current.type = fields[3] == "true_false"
                ? QuestionType::TRUE_FALSE
                : QuestionType::MULTIPLE_CHOICE;
            current.topic = fields[4];
            current.subtopic = fields[5];
            correct_order = std::atoi(fields[6].c_str());
            current.prompt = fields[7];
            current.explanation = fields[8];
            current.options.clear();

            in_question = true;
            continue;
        }

        if (fields[0] == "O")
        {
            if (!in_question || fields.size() != 4)
            {
                if (error)
                    *error = "Question API returned an option outside a question.";
                questions->clear();
                return false;
            }

            const int option_order = std::atoi(fields[2].c_str());
            if (option_order == correct_order)
                current.correct_index = current.options.size();

            current.options.push_back(fields[3]);
            continue;
        }

        if (fields[0] == "E")
        {
            if (!in_question || current.id <= 0 ||
                current.prompt.empty() || current.options.size() < 2 ||
                current.correct_index >= current.options.size())
            {
                if (error)
                    *error = "Question API returned an incomplete question.";
                questions->clear();
                return false;
            }

            if (current.type == QuestionType::TRUE_FALSE &&
                current.options.size() != 2)
            {
                if (error)
                    *error = "True/false question does not contain exactly two options.";
                questions->clear();
                return false;
            }

            questions->push_back(current);
            in_question = false;
            correct_order = -1;
            continue;
        }
    }

    if (in_question)
    {
        if (error)
            *error = "Question API response ended before the question was closed.";
        questions->clear();
        return false;
    }

    if (questions->size() < QuestionManager::QUESTIONS_PER_RACE)
    {
        if (error)
            *error = "Question API returned fewer than 10 valid questions.";
        questions->clear();
        return false;
    }

    return true;
}

} // namespace Education
