//
//  Carrera Educativa STK - educational extension for SuperTuxKart
//  Copyright (C) 2026
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//

#ifndef HEADER_EDUCATION_QUESTION_MANAGER_HPP
#define HEADER_EDUCATION_QUESTION_MANAGER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Education
{

enum class QuestionType
{
    MULTIPLE_CHOICE,
    TRUE_FALSE
};

struct Question
{
    int id;
    int level;
    QuestionType type;
    std::string topic;
    std::string subtopic;
    std::string prompt;
    std::vector<std::string> options;

    // Database option ids are populated only for API questions.
    std::vector<int> option_ids;

    // Built-in demo questions can still be validated locally as fallback.
    std::size_t correct_index;
    std::string explanation;

    // True when the correct answer should be validated by the PHP/MySQL API.
    bool server_validated;

    // True when this question also carries a local cached answer key, allowing
    // the race to continue when the API is temporarily unreachable.
    bool has_offline_answer_key;

    Question();
};

struct AnswerResult
{
    bool accepted;
    bool correct;
    int question_id;
    std::size_t selected_index;
    std::size_t correct_index;
    std::uint32_t response_time_ms;

    AnswerResult();
};

class QuestionManager
{
public:
    static const std::size_t QUESTIONS_PER_RACE = 10;

private:
    std::vector<Question> m_question_bank;
    std::vector<Question> m_race_questions;
    std::vector<AnswerResult> m_results;
    std::size_t m_current_index;

    bool isQuestionValid(const Question& question) const;

public:
    QuestionManager();

    void clear();
    void setQuestionBank(const std::vector<Question>& questions);
    const std::vector<Question>& getQuestionBank() const;

    /**
     * Selects exactly 10 unique questions for one race.
     * Returns false if the bank has fewer than 10 valid questions.
     */
    bool startRace(unsigned int seed = 0);

    const Question* getCurrentQuestion() const;

    /**
     * Stores one locally validated answer and advances to the next mandatory
     * question. Used only by the built-in demo bank fallback.
     */
    AnswerResult submitAnswer(std::size_t selected_index,
                              std::uint32_t response_time_ms);

    /**
     * Stores an answer whose correctness has already been validated by the
     * PHP/MySQL API.
     */
    AnswerResult submitValidatedAnswer(std::size_t selected_index,
                                       bool correct,
                                       std::size_t correct_index,
                                       std::uint32_t response_time_ms);

    bool isRaceQuestionSetComplete() const;
    std::size_t getAnsweredCount() const;
    std::size_t getRemainingCount() const;
    std::size_t getCorrectCount() const;
    std::size_t getIncorrectCount() const;

    const std::vector<Question>& getRaceQuestions() const;
    const std::vector<AnswerResult>& getResults() const;
    const Question* findQuestionById(int id) const;
};

} // namespace Education

#endif
