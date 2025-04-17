#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

namespace Event {
struct MouseScrollGameThreadEvent : public TEvent<
                                        MouseScrollGameThreadEvent,
                                        eEventThreadType::GAME_THREAD,
                                        SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>> {
public:
    using Event_t = TEvent<
        MouseScrollGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>>::Event_t;

    std::string ToString() const override
    {
        return "MouseScrollGameThreadEvent";
    }
};

struct MouseScrollLuaThreadEvent : public TEvent<
                                       MouseScrollLuaThreadEvent,
                                       eEventThreadType::LUA_THREAD,
                                       SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>> {
public:
    using Event_t = TEvent<
        MouseScrollLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>::Event_t;

    std::string ToString() const override
    {
        return "MouseScrollLuaThreadEvent";
    }
};
} // namespace Event
