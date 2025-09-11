#pragma once

#include "Core/GameCore/Components/Transform.h"
#include "Policy/Policies.h"
#include "TEvent.h"

namespace Event {

class PlayerMovedGameThreadEvent : public TEvent<
                                       PlayerMovedGameThreadEvent,
                                       eEventThreadType::GAME_THREAD,
                                       SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>> {
public:
    using Type_t = TEvent<
        PlayerMovedGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>::Type_t;

    std::string ToString() const override
    {
        return "GameThreadPlayerMovedEvent";
    }
};

} // namespace Event