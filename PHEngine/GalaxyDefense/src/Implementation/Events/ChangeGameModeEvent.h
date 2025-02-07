#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Implementation/GameModeTypeEnum.h"

#include <string>

using namespace Game;

namespace Event {
class ChangeGameModeEvent
    : public TEvent<ChangeGameModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eGameModeType>> {
public:
    using Event_t = TEvent<ChangeGameModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eGameModeType>>::Event_t;

    std::string ToString() const override
    {
        return "ChangeGameModeEvent";
    }
};

class LuaChangeGameModeEvent
    : public TEvent<LuaChangeGameModeEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<eGameModeType>> {
public:
    using Event_t = TEvent<LuaChangeGameModeEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<eGameModeType>>::Event_t;

    std::string ToString() const override
    {
        return "LuaChangeGameModeEvent";
    }
};

} // namespace Event