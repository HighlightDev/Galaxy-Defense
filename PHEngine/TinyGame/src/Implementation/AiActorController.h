#pragma once

#include "Core/GameCore/ActorController.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class AiActorController : public ActorController
    {
    public:
        AiActorController(const std::shared_ptr<Actor>& actor);

        virtual void Tick(const float deltaTime) override;

        virtual void InitActorController() override;
    };
}