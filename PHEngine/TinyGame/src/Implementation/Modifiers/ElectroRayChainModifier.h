#pragma once

#include "IModifiable.h"
#include "Core/CommonCore/Timer.h"
#include "Implementation/Actors/ElectroRayChainActor.h"

#include <memory>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Actor;
}

namespace Game
{
    class SpaceshipActor;
    class ElectroRayChainActorPool;

    class ElectroRayChainModifier : public IModifiable
    {
        std::weak_ptr<Actor> mChainDst;

        std::weak_ptr<Actor> mChainSrc;

        std::shared_ptr<ElectroRayChainActor> mElectroRayChainActor;

        GameThreadTimer mDisposeTimer;

    public:
        ElectroRayChainModifier(const std::weak_ptr<Actor> &chainDst, const std::weak_ptr<Actor> &chainSrc);

        eModifierType GetModifierType() const override;

        uint64_t CreatorObjectId() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override{};

        bool IsExpired() const override;

        void OnPreRemoved() override;

        void Initialize(const std::shared_ptr<::Game::ElectroRayChainActorPool> &mElectroRayChainActorPool);

    private:
        void OnDisposeTimerTimeout();
    };
}