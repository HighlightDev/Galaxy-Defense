#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <glm/vec2.hpp>

#include <vector>

using namespace EngineCore;

namespace Event {
struct MouseButtonDownRootEvent : public TEvent<
                                      MouseButtonDownRootEvent,
                                      eEventThreadType::GAME_THREAD,
                                      MultipleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>> {
    using Type_t = TEvent<
        MouseButtonDownRootEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>::Type_t;

    std::string ToString() const override
    {
        return "MouseButtonDownRootEvent";
    }
};

struct MouseButtonDownGameThreadEvent : public TEvent<
                                            MouseButtonDownGameThreadEvent,
                                            eEventThreadType::GAME_THREAD,
                                            MultipleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>> {
public:
    using Type_t = TEvent<
        MouseButtonDownRootEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>::Type_t;

    std::string ToString() const override
    {
        return "MouseButtonDownGameThreadEvent";
    }
};

struct MouseButtonDownLuaThreadEvent : public TEvent<
                                           MouseButtonDownLuaThreadEvent,
                                           eEventThreadType::LUA_THREAD,
                                           MultipleDataEventPolicy<std::vector<MouseKeysData>>> {
public:
    using Type_t
        = TEvent<MouseButtonDownLuaThreadEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::vector<MouseKeysData>>>::
            Type_t;

    std::string ToString() const override
    {
        return "MouseButtonDownLuaThreadEvent";
    }
};
} // namespace Event
