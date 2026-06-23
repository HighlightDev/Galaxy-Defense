#pragma once

#include "Core/GameCore/ActorController.h"

#include <memory>
#include <string>

using namespace EngineCore;

namespace Game {
class AiActorController : public ActorController {
public:
    AiActorController(const std::shared_ptr<Actor>& actor);

    void Tick(const float deltaTimeSec, const float playSpeed) override;
};
} // namespace Game