#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"
#include "Core/GameCore/Input/Keys.h"

#include <vector>

using namespace EngineCore;

namespace Event {
struct KeyboardButtonDownGameThreadEvent : public TEvent<
                                               KeyboardButtonDownGameThreadEvent,
                                               eEventThreadType::GAME_THREAD,
                                               SingleDataEventPolicy<std::vector<KeyboardKeysData>>> {
public:
    using Type_t = TEvent<
        KeyboardButtonDownGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<std::vector<KeyboardKeysData>>>::Type_t;

    std::string ToString() const override
    {
        return "KeyboardButtonDownGameThreadEvent";
    }
};

struct KeyboardButtonDownLuaThreadEvent : public TEvent<
                                              KeyboardButtonDownLuaThreadEvent,
                                              eEventThreadType::LUA_THREAD,
                                              SingleDataEventPolicy<std::vector<KeyboardKeysData>>> {
public:
    using Type_t = TEvent<
        KeyboardButtonDownLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        SingleDataEventPolicy<std::vector<KeyboardKeysData>>>::Type_t;

    std::string ToString() const override
    {
        return "KeyboardButtonDownLuaThreadEvent";
    }
};
} // namespace Event
