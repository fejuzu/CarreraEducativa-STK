//
// Carrera Educativa STK - mandatory question dialog
// GPL v3 or later.
//

#include "education/question_dialog.hpp"

#include "guiengine/widgets/button_widget.hpp"
#include "guiengine/widgets/label_widget.hpp"
#include "items/powerup_manager.hpp"
#include "karts/abstract_kart.hpp"
#include "modes/world.hpp"
#include "states_screens/state_manager.hpp"
#include "utils/string_utils.hpp"

#include <algorithm>
#include <cstdint>
#include <sstream>

using namespace GUIEngine;

namespace Education
{

QuestionDialog::QuestionDialog(QuestionManager* manager, AbstractKart* kart,
                               const Question& question,
                               std::size_t question_number,
                               std::size_t total_questions)
    : ModalDialog(0.82f, 0.86f, MODAL_DIALOG_LOCATION_CENTER),
      m_manager(manager),
      m_kart(kart),
      m_question(question),
      m_question_number(question_number),
      m_total_questions(total_questions),
      m_answered(false)
{
    if (StateManager::get()->getGameState() == GUIEngine::GAME &&
        World::getWorld())
    {
        World::getWorld()->schedulePause(World::IN_GAME_MENU_PHASE);
    }

    loadFromFile("education_question_dialog.stkgui");
    setupWidgets();
    m_opened_at = std::chrono::steady_clock::now();
}

QuestionDialog::~QuestionDialog()
{
    if (StateManager::get()->getGameState() == GUIEngine::GAME &&
        World::getWorld())
    {
        World::getWorld()->scheduleUnpause();
    }
}

void QuestionDialog::setupWidgets()
{
    std::ostringstream counter;
    counter << "PREGUNTA " << m_question_number
            << " / " << m_total_questions;

    getWidget<LabelWidget>("counter")->setText(
        StringUtils::utf8ToWide(counter.str()), false);

    std::string topic = m_question.topic;
    if (!m_question.subtopic.empty())
        topic += "  ·  " + m_question.subtopic;
    getWidget<LabelWidget>("topic")->setText(
        StringUtils::utf8ToWide(topic), false);

    getWidget<LabelWidget>("question")->setText(
        StringUtils::utf8ToWide(m_question.prompt), false);

    const char* ids[] = {"answer_a", "answer_b", "answer_c", "answer_d"};
    const char* prefixes[] = {"A) ", "B) ", "C) ", "D) "};

    for (std::size_t i = 0; i < 4; i++)
    {
        ButtonWidget* button = getWidget<ButtonWidget>(ids[i]);
        if (i < m_question.options.size())
        {
            button->setLabel(StringUtils::utf8ToWide(
                std::string(prefixes[i]) + m_question.options[i]));
            button->setVisible(true);
            button->setActive(true);
        }
        else
        {
            button->setVisible(false);
            button->setActive(false);
        }
    }

    getWidget<LabelWidget>("feedback")->setVisible(false);

    ButtonWidget* continue_button = getWidget<ButtonWidget>("continue");
    continue_button->setVisible(false);
    continue_button->setActive(false);

    getWidget<ButtonWidget>("answer_a")->setFocusForPlayer(
        PLAYER_ID_GAME_MASTER);
}

std::string QuestionDialog::applyReward()
{
    if (!m_kart)
        return "Recompensa aplicada.";

    const int selector =
        (m_question.id + World::getWorld()->getTicksSinceStart()) % 3;

    switch (selector)
    {
        case 0:
            m_kart->handleZipper(NULL, true);
            return "Recompensa: TURBO inmediato.";
        case 1:
            m_kart->setEnergy(std::min(100.0f, m_kart->getEnergy() + 20.0f));
            return "Recompensa: +20 de NITRO.";
        default:
            m_kart->setPowerup(PowerupManager::POWERUP_ZIPPER, 1);
            return "Recompensa: POWER-UP turbo.";
    }
}

std::string QuestionDialog::applyPenalty()
{
    if (!m_kart)
        return "Penalización aplicada.";

    const int selector =
        (m_question.id + World::getWorld()->getTicksSinceStart()) % 3;

    switch (selector)
    {
        case 0:
            m_kart->adjustSpeed(0.55f);
            return "Penalización: reducción inmediata de velocidad.";
        case 1:
            m_kart->setSquash(3.0f, 0.55f);
            return "Penalización: velocidad limitada durante unos segundos.";
        default:
            m_kart->setEnergy(std::max(0.0f, m_kart->getEnergy() - 20.0f));
            return "Penalización: -20 de NITRO.";
    }
}

void QuestionDialog::answer(std::size_t option_index)
{
    if (m_answered || !m_manager)
        return;

    const std::chrono::steady_clock::time_point now =
        std::chrono::steady_clock::now();
    const std::uint32_t elapsed_ms = static_cast<std::uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_opened_at).count());

    const AnswerResult result =
        m_manager->submitAnswer(option_index, elapsed_ms);

    if (!result.accepted)
        return;

    m_answered = true;

    std::string feedback;
    if (result.correct)
        feedback = "¡CORRECTO!  " + applyReward();
    else
    {
        feedback = "INCORRECTO.  " + applyPenalty();
        if (!m_question.explanation.empty())
            feedback += "\n" + m_question.explanation;
    }

    LabelWidget* feedback_widget = getWidget<LabelWidget>("feedback");
    feedback_widget->setText(StringUtils::utf8ToWide(feedback), false);
    feedback_widget->setVisible(true);

    const char* ids[] = {"answer_a", "answer_b", "answer_c", "answer_d"};
    for (std::size_t i = 0; i < 4; i++)
        getWidget<ButtonWidget>(ids[i])->setActive(false);

    ButtonWidget* continue_button = getWidget<ButtonWidget>("continue");
    continue_button->setVisible(true);
    continue_button->setActive(true);
    continue_button->setFocusForPlayer(PLAYER_ID_GAME_MASTER);
}

GUIEngine::EventPropagation
QuestionDialog::processEvent(const std::string& eventSource)
{
    if (eventSource == "answer_a")
    {
        answer(0);
        return GUIEngine::EVENT_BLOCK;
    }
    if (eventSource == "answer_b")
    {
        answer(1);
        return GUIEngine::EVENT_BLOCK;
    }
    if (eventSource == "answer_c")
    {
        answer(2);
        return GUIEngine::EVENT_BLOCK;
    }
    if (eventSource == "answer_d")
    {
        answer(3);
        return GUIEngine::EVENT_BLOCK;
    }
    if (eventSource == "continue" && m_answered)
    {
        ModalDialog::dismiss();
        return GUIEngine::EVENT_BLOCK;
    }

    return GUIEngine::EVENT_LET;
}

void QuestionDialog::onEnterPressedInternal()
{
}

void QuestionDialog::onUpdate(float dt)
{
    (void)dt;
}

} // namespace Education
