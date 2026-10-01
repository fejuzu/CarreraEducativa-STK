//
//  Carrera Educativa STK - access rules
//
//  GPL v3 or later.
//

#ifndef HEADER_EDUCATION_RULES_HPP
#define HEADER_EDUCATION_RULES_HPP

namespace Education
{

class Rules
{
public:
    enum
    {
        DAILY_MAX_ENERGY = 5,
        GAME_OPEN_HOUR = 7,
        GAME_CLOSE_HOUR = 21
    };

    /**
     * The game accepts new races from 07:00 through 20:59.
     * A race that already started before 21:00 may finish normally.
     */
    static bool canStartRaceAtHour(int hour)
    {
        return hour >= GAME_OPEN_HOUR && hour < GAME_CLOSE_HOUR;
    }

    static bool hasEnergy(int energy)
    {
        return energy > 0;
    }

    static int consumeEnergy(int energy)
    {
        return energy > 0 ? energy - 1 : 0;
    }
};

} // namespace Education

#endif
