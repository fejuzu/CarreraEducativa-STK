//
// Carrera Educativa STK - mandatory question dialog
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_QUESTION_DIALOG_HPP
#define HEADER_EDUCATION_QUESTION_DIALOG_HPP

#include "education/question_manager.hpp"
#include "guiengine/modaldialog.hpp"
#include "utils/cpp2011.hpp"

#include <chrono>
#include <cstddef>
#include <string>

class AbstractKart;

namespace Education
{

class QuestionDialog : public GUIEngine::ModalDialog
{
private:
    QuestionManager* m_manager;
    AbstractKart* m_kart;
    Question m_question;
    std::size_t m_question_number;
    std::size_t m_total_questions;
    bool m_answered;
    std::chrono::steady_clock::time_point m_opened_at;

    void setupWidgets();
    void answer(std::size_t option_index);
    std::string applyReward();
    std::string applyPenalty();

public:
    QuestionDialog(QuestionManager* manager, AbstractKart* kart,
                   const Question& question,
                   std::size_t question_number,
                   std::size_t total_questions);
    virtual ~QuestionDialog();

    virtual void onEnterPressedInternal() OVERRIDE;
    virtual void onUpdate(float dt) OVERRIDE;
    virtual bool onEscapePressed() OVERRIDE { return false; }

    virtual GUIEngine::EventPropagation
        processEvent(const std::string& eventSource) OVERRIDE;
};

} // namespace Education

#endif
