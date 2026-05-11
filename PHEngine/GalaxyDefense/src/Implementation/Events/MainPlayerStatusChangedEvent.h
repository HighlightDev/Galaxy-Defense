#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Implementation/StatusTypes.h"

using namespace Game;

namespace Event {
class MainPlayerStatusChangedEvent
    : public TEvent<MainPlayerStatusChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*args*/>> {
public:
    using Type_t
        = TEvent<MainPlayerStatusChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*args*/>>::
            Type_t;

    std::string ToString() const override
    {
        return "GameThreadMainPlayerStatusChangedEvent";
    }
};

class LuaMainPlayerStatusChangedEvent : public TEvent<
                                            LuaMainPlayerStatusChangedEvent,
                                            eEventThreadType::LUA_THREAD,
                                            MultipleDataEventPolicy<std::string /*args*/>> {
public:
    using Type_t
        = TEvent<LuaMainPlayerStatusChangedEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::string /*args*/>>::
            Type_t;

    std::string ToString() const override
    {
        return "LuaThreadMainPlayerStatusChangedEvent";
    }
};

} // namespace Event