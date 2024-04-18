#pragma once

#include "IModifiable.h"

#include "Implementation/GameObjectsType.h"

#include <memory>
#include <glm/vec3.hpp>
#include <utility>

namespace EngineCore
{
    class Actor;
}

namespace Game
{
    class SpaceshipActor;
    class ElectroRayChainActor;

    class ElectroRayChainModifier : public IModifiable
    {
        std::pair<eGameObjectsType, const std::weak_ptr<::EngineCore::Actor>> mChainDst;

        std::pair<eGameObjectsType, const std::weak_ptr<::EngineCore::Actor>> mChainSrc;

        std::shared_ptr<ElectroRayChainActor> mElectroRayChainActor;

        bool mIsPendingRemoval{false};

    public:
        ElectroRayChainModifier(const std::pair<eGameObjectsType, const std::weak_ptr<::EngineCore::Actor>> &chainDstActor,
                                const std::pair<eGameObjectsType, const std::weak_ptr<::EngineCore::Actor>> &chainSrc);

        eModifierType GetModifierType() const override;

        int32_t CreatorObjectId() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override{};

        bool IsExpired() const override;

        void OnPreRemoved() override;

        void Initialize(const std::shared_ptr<::Game::ElectroRayChainActor> &electroRayChainActor);
    };
}