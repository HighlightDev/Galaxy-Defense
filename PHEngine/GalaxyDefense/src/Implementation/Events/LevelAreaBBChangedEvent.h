#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <string>

using namespace EngineCore::GUI;

namespace Event {
class LevelAreaBBChangedGameThreadEvent : public TEvent<
                                              LevelAreaBBChangedGameThreadEvent,
                                              eEventThreadType::GAME_THREAD,
                                              MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>> {
public:
    using Type_t = TEvent<
        LevelAreaBBChangedGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>>::Type_t;

    std::string ToString() const override
    {
        return "LevelAreaBBChangedGameThreadEvent";
    }
};

class LevelAreaBBChangedLuaThreadEvent : public TEvent<
                                             LevelAreaBBChangedLuaThreadEvent,
                                             eEventThreadType::LUA_THREAD,
                                             MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>> {
public:
    using Type_t = TEvent<
        LevelAreaBBChangedLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>>::Type_t;

    std::string ToString() const override
    {
        return "LevelAreaBBChangedLuaThreadEvent";
    }
};

} // namespace Event