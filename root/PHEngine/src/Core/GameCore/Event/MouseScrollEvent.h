#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <glm/vec2.hpp>

namespace Event {
struct MouseScrollRootEvent : public TEvent<
                                  MouseScrollRootEvent,
                                  eEventThreadType::GAME_THREAD,
                                  SingleDataEventPolicy<glm::ivec2, EngineCore::eMouseScrollDirection, float>> {
public:
    using Type_t = TEvent<
        MouseScrollRootEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<glm::ivec2, EngineCore::eMouseScrollDirection, float>>::Type_t;

    std::string ToString() const override
    {
        return "MouseScrollRootEvent";
    }
};

struct MouseScrollGameThreadEvent
    : public TEvent<
          MouseScrollGameThreadEvent,
          eEventThreadType::GAME_THREAD,
          SingleDataEventPolicy<EngineCore::eMouseEventTargetReceiverType, EngineCore::eMouseScrollDirection, float>> {
public:
    using Type_t = TEvent<
        MouseScrollGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<EngineCore::eMouseEventTargetReceiverType, EngineCore::eMouseScrollDirection, float>>::Type_t;

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
    using Type_t = TEvent<
        MouseScrollLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>>::Type_t;

    std::string ToString() const override
    {
        return "MouseScrollLuaThreadEvent";
    }
};
} // namespace Event
