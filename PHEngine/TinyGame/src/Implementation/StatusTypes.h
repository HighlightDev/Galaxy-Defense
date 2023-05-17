#pragma once

namespace Game
{
    enum class eMainPlayerStatusType
    {
        NONE,
        LIFE_POINTS_CHANGED,
        ACTIVE_WEAPON_CHANGED, // changed active type of missiles
        MISSILES_COUNT_CHANGED, // amount of missiles changed
        AVAILABLE_MISSILES_CHANGED // missiles become unlocked or locked
    };
}
