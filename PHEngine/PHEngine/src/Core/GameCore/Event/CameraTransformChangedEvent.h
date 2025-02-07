#pragma once

#include "Core/GameCore/ACamera.h"
#include "Policy/Policies.h"
#include "TEvent.h"

using namespace EngineCore;

namespace Event {

class CameraTransformChangedGameThreadEvent
    : public TEvent<CameraTransformChangedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>> {
public:
    using Event_t
        = TEvent<CameraTransformChangedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>::Event_t;

    std::string ToString() const override
    {
        return "GameThreadCameraTransformChangedEvent";
    }
};

} // namespace Event