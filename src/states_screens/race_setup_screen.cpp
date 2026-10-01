//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009-2015 Marianne Gagnon
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "states_screens/race_setup_screen.hpp"

#include "challenges/unlock_manager.hpp"
#include "config/player_manager.hpp"
#include "config/user_config.hpp"
#include "guiengine/widgets/dynamic_ribbon_widget.hpp"
#include "guiengine/widgets/ribbon_widget.hpp"
#include "guiengine/widgets/spinner_widget.hpp"
#include "input/input_manager.hpp"
#include "io/file_manager.hpp"
#include "race/race_manager.hpp"
#include "states_screens/arenas_screen.hpp"
#include "states_screens/easter_egg_screen.hpp"
#include "states_screens/ghost_replay_selection.hpp"
#include "states_screens/soccer_setup_screen.hpp"
#include "states_screens/state_manager.hpp"
#include "states_screens/tracks_and_gp_screen.hpp"
#include "utils/string_utils.hpp"
#include "utils/translation.hpp"

const int CONFIG_CODE_NORMAL    = 0;
const int CONFIG_CODE_TIMETRIAL = 1;
const int CONFIG_CODE_FTL       = 2;
const int CONFIG_CODE_3STRIKES  = 3;
const int CONFIG_CODE_EASTER    = 4;
const int CONFIG_CODE_SOCCER    = 5;
const int CONFIG_CODE_GHOST     = 6;
const int CONFIG_CODE_LAP_TRIAL = 7;

using namespace GUIEngine;

// -----------------------------------------------------------------------------

RaceSetupScreen::RaceSetupScreen() : Screen("race_setup.stkgui")
{
}   // RaceSetupScreen

// -----------------------------------------------------------------------------

void RaceSetupScreen::loadedFromFile()
{
}   // loadedFromFile

// -----------------------------------------------------------------------------

void RaceSetupScreen::init()
{
    Screen::init();
    input_manager->setMasterPlayerOnly(true);
    RibbonWidget* w = getWidget<RibbonWidget>("difficulty");
    assert( w != NULL );

    RaceManager::get()->setMajorMode(RaceManager::MAJOR_MODE_SINGLE);
    if (UserConfigParams::m_difficulty == RaceManager::DIFFICULTY_BEST &&
        PlayerManager::getCurrentPlayer()->isLocked("difficulty_best"))
    {
        w->setSelection(RaceManager::DIFFICULTY_HARD, PLAYER_ID_GAME_MASTER);
    }
    else
    {
        w->setSelection( UserConfigParams::m_difficulty, PLAYER_ID_GAME_MASTER );
    }

    DynamicRibbonWidget* w2 = getWidget<DynamicRibbonWidget>("gamemode");
    assert( w2 != NULL );
    w2->clearItems();

    // IGH EDUCATIVO uses only the standard race mode.
    // Keep the game-mode screen intentionally limited to one option while
    // the educational experience is under development.
    irr::core::stringw normal_name = irr::core::stringw(
        RaceManager::getNameOf(RaceManager::MINOR_MODE_NORMAL_RACE)) + L"\n";
    normal_name += _("Educational race with questions and power-ups.");

    w2->addItem(normal_name, IDENT_STD,
        RaceManager::getIconOf(RaceManager::MINOR_MODE_NORMAL_RACE));

    RaceManager::get()->setMinorMode(RaceManager::MINOR_MODE_NORMAL_RACE);
    UserConfigParams::m_game_mode = CONFIG_CODE_NORMAL;
    w2->setSelection(IDENT_STD, PLAYER_ID_GAME_MASTER, true);
    w2->setItemCountHint(1);
    w2->updateItemDisplay();

    {
        RibbonWidget* w = getWidget<RibbonWidget>("difficulty");
        assert(w != NULL);

        int index = w->findItemNamed("best");
        Widget* hardestWidget = &w->getChildren()[index];

        if (PlayerManager::getCurrentPlayer()->isLocked("difficulty_best"))
        {
            hardestWidget->setBadge(LOCKED_BADGE);
            hardestWidget->setActive(false);
        }
        else
        {
            hardestWidget->unsetBadge(LOCKED_BADGE);
            hardestWidget->setActive(true);
        }
    }
}   // init

// -----------------------------------------------------------------------------
void RaceSetupScreen::eventCallback(Widget* widget, const std::string& name,
                                    const int playerID)
{
    if (name == "difficulty")
    {
        assignDifficulty();
    }
    else if (name == "gamemode")
    {
        assignDifficulty();

        DynamicRibbonWidget* w = dynamic_cast<DynamicRibbonWidget*>(widget);
        const std::string& selectedMode = w->getSelectionIDString(PLAYER_ID_GAME_MASTER);

        if (selectedMode == IDENT_STD)
        {
            RaceManager::get()->setMinorMode(RaceManager::MINOR_MODE_NORMAL_RACE);
            UserConfigParams::m_game_mode = CONFIG_CODE_NORMAL;
            TracksAndGPScreen::getInstance()->push();
        }
    }
    else if (name == "back")
    {
        StateManager::get()->escapePressed();
    }

}   // eventCallback

// -----------------------------------------------------------------------------
/** Converts the difficulty string into a RaceManager::Difficulty value
 *  and sets this difficulty in the user config and in the race manager.
 */
void RaceSetupScreen::assignDifficulty()
{
    RibbonWidget* difficulty_widget = getWidget<RibbonWidget>("difficulty");
    assert(difficulty_widget != NULL);
    const std::string& difficulty =
        difficulty_widget->getSelectionIDString(PLAYER_ID_GAME_MASTER);
    
    RaceManager::Difficulty diff = RaceManager::convertDifficulty(difficulty);
    UserConfigParams::m_difficulty = diff;
    RaceManager::get()->setDifficulty(diff);
}   // assignDifficulty

// -----------------------------------------------------------------------------
