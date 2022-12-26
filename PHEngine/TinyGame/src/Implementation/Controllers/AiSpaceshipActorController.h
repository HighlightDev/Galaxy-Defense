#pragma once

#include "AiActorController.h"

#include <memory>
#include <string>

using namespace EngineCore;

namespace Game
{
    class AiSpaceshipActorController : public AiActorController
    {
        std::string mCurrentState;

    public:
        AiSpaceshipActorController(const std::shared_ptr<Actor>& actor);

        void Tick(const float deltaTime) override;

        void InitActorController() override;
    };
}