//
//  Carrera Educativa STK - educational extension for SuperTuxKart
//  Copyright (C) 2026
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//

#include "education/question_manager.hpp"

#include <algorithm>
#include <numeric>
#include <random>

namespace Education
{

const std::size_t QuestionManager::QUESTIONS_PER_RACE;

Question::Question()
    : id(0),
      level(1),
      type(QuestionType::MULTIPLE_CHOICE),
      correct_index(0)
{
}

AnswerResult::AnswerResult()
    : accepted(false),
      correct(false),
      question_id(0),
      selected_index(0),
      correct_index(0),
      response_time_ms(0)
{
}

QuestionManager::QuestionManager()
    : m_current_index(0)
{
}

void QuestionManager::clear()
{
    m_question_bank.clear();
    m_race_questions.clear();
    m_results.clear();
    m_current_index = 0;
}

void QuestionManager::setQuestionBank(const std::vector<Question>& questions)
{
    m_question_bank = questions;
    m_race_questions.clear();
    m_results.clear();
    m_current_index = 0;
}

const std::vector<Question>& QuestionManager::getQuestionBank() const
{
    return m_question_bank;
}

bool QuestionManager::isQuestionValid(const Question& question) const
{
    if (question.id <= 0 || question.prompt.empty())
        return false;

    if (question.options.size() < 2)
        return false;

    if (question.correct_index >= question.options.size())
        return false;

    if (question.type == QuestionType::TRUE_FALSE &&
        question.options.size() != 2)
        return false;

    return true;
}

bool QuestionManager::startRace(unsigned int seed)
{
    m_race_questions.clear();
    m_results.clear();
    m_current_index = 0;

    std::vector<std::size_t> valid_indices;
    valid_indices.reserve(m_question_bank.size());

    for (std::size_t i = 0; i < m_question_bank.size(); i++)
    {
        if (isQuestionValid(m_question_bank[i]))
            valid_indices.push_back(i);
    }

    if (valid_indices.size() < QUESTIONS_PER_RACE)
        return false;

    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }

    std::mt19937 generator(seed);
    std::shuffle(valid_indices.begin(), valid_indices.end(), generator);

    m_race_questions.reserve(QUESTIONS_PER_RACE);
    for (std::size_t i = 0; i < QUESTIONS_PER_RACE; i++)
        m_race_questions.push_back(m_question_bank[valid_indices[i]]);

    return true;
}

const Question* QuestionManager::getCurrentQuestion() const
{
    if (m_current_index >= m_race_questions.size())
        return NULL;

    return &m_race_questions[m_current_index];
}

AnswerResult QuestionManager::submitAnswer(
    std::size_t selected_index,
    std::uint32_t response_time_ms)
{
    AnswerResult result;
    const Question* question = getCurrentQuestion();

    if (question == NULL || selected_index >= question->options.size())
        return result;

    result.accepted = true;
    result.correct = selected_index == question->correct_index;
    result.question_id = question->id;
    result.selected_index = selected_index;
    result.correct_index = question->correct_index;
    result.response_time_ms = response_time_ms;

    m_results.push_back(result);
    m_current_index++;

    return result;
}

bool QuestionManager::isRaceQuestionSetComplete() const
{
    return !m_race_questions.empty() &&
           m_current_index >= m_race_questions.size();
}

std::size_t QuestionManager::getAnsweredCount() const
{
    return m_results.size();
}

std::size_t QuestionManager::getRemainingCount() const
{
    if (m_current_index >= m_race_questions.size())
        return 0;

    return m_race_questions.size() - m_current_index;
}

std::size_t QuestionManager::getCorrectCount() const
{
    std::size_t count = 0;
    for (std::size_t i = 0; i < m_results.size(); i++)
    {
        if (m_results[i].correct)
            count++;
    }
    return count;
}

std::size_t QuestionManager::getIncorrectCount() const
{
    return getAnsweredCount() - getCorrectCount();
}

const std::vector<Question>& QuestionManager::getRaceQuestions() const
{
    return m_race_questions;
}

const std::vector<AnswerResult>& QuestionManager::getResults() const
{
    return m_results;
}

const Question* QuestionManager::findQuestionById(int id) const
{
    for (std::size_t i = 0; i < m_question_bank.size(); i++)
    {
        if (m_question_bank[i].id == id)
            return &m_question_bank[i];
    }

    return NULL;
}

} // namespace Education
