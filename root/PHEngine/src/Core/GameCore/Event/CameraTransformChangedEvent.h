#pragma once

#include "Core/GameCore/ACamera.h"
#include "Policy/Policies.h"
#include "TEvent.h"

using namespace EngineCore;

namespace Event {

class CameraTransformChangedGameThreadEvent
    : public TEvent<CameraTransformChangedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>> {
public:
    using Type_t
        = TEvent<CameraTransformChangedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>::Type_t;

    std::string ToString() const override
    {
        return "GameThreadCameraTransformChangedEvent";
    }
};

} // namespace Event