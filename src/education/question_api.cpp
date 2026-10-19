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

            if (fields.size() != 7)
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
            current.prompt = fields[6];
            current.explanation.clear();
            current.options.clear();
            current.option_ids.clear();
            current.server_validated = true;

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

            current.option_ids.push_back(std::atoi(fields[1].c_str()));
            current.options.push_back(fields[3]);
            continue;
        }

        if (fields[0] == "E")
        {
            if (!in_question || current.id <= 0 ||
                current.prompt.empty() || current.options.size() < 2 ||
                current.option_ids.size() != current.options.size())
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

bool validateAnswerWithApi(const std::string& url,
                           const Question& question,
                           std::size_t selected_index,
                           AnswerValidation* validation,
                           std::string* error)
{
    if (!validation)
    {
        if (error)
            *error = "Answer validation output pointer is null.";
        return false;
    }

    if (!question.server_validated ||
        selected_index >= question.option_ids.size())
    {
        if (error)
            *error = "Question does not contain a valid database option id.";
        return false;
    }

    Online::HTTPRequest request;
    request.setURL(url);
    request.addParameter("question_id", question.id);
    request.addParameter("option_id", question.option_ids[selected_index]);
    request.executeNow();

    if (request.hadDownloadError())
    {
        if (error)
            *error = request.getDownloadErrorMessage();
        return false;
    }

    std::string line = request.getData();
    const std::size_t newline = line.find_first_of("\r\n");
    if (newline != std::string::npos)
        line.erase(newline);

    const std::vector<std::string> fields = splitEscaped(line);
    if (fields.size() != 4 || fields[0] != "OK")
    {
        if (error)
            *error = "Answer API returned an invalid response.";
        return false;
    }

    const int correct_option_id = std::atoi(fields[2].c_str());
    std::size_t correct_index = question.option_ids.size();

    for (std::size_t i = 0; i < question.option_ids.size(); i++)
    {
        if (question.option_ids[i] == correct_option_id)
        {
            correct_index = i;
            break;
        }
    }

    if (correct_index >= question.option_ids.size())
    {
        if (error)
            *error = "Answer API returned an option not present in this question.";
        return false;
    }

    validation->correct = fields[1] == "1";
    validation->correct_index = correct_index;
    validation->explanation = fields[3];

    return true;
}

} // namespace Education
