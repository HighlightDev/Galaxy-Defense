#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/CommonCore/Timer.h"

#include "Implementation/Actors/MissileActor.h"

#include <functional>
#include <unordered_map>
#include <memory>
#include <glm/vec3.hpp>

using namespace EngineCore;

namespace EngineCore
{
    class RuntimeGeneratedLineComponent;
    class Actor;
}

namespace Game
{
    class MissileExplosionVisitorBase;

    class ElectroRayActor
        : public MissileActor
    {
        std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> mLineComponent;
        std::weak_ptr<::EngineCore::Actor> mSpaceshipWhoSpawnedMeWp;

        glm::vec3 mElectroLineBegin;
        glm::vec3 mElectroLineEnd;

        float mElectroLineOriginSpeed;
        float mElectroLineDestinationSpeed;

        GameThreadTimer mElectroLineOriginStartMovementDelayTimer;
        bool bLineOriginStartMovement;

        std::weak_ptr<::EngineCore::Actor> mCollidedSpaceship;
        bool bElectroLineCollided;
        
        std::shared_ptr<EngineGOProperty<float>> mOpacity;
        
    public:
        ElectroRayActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        bool IsInsideLevel(const BoundingBox3D &boundingBox) const override;

        void Tick(const float deltaTime) override;

        void TriggerSpawn(const glm::vec3 &position) override;

        void TriggerExplosion() override;

        void TriggerExplosionFinished() override;

        void TriggerDisabled() override;

        void SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> &lineComponent);

        void SetSpawnerSpaceship(const std::weak_ptr<::EngineCore::Actor> &spawnerSpaceship);

        void SetElectroLineOriginSpeed(const float speed);

        void SetElectroLineDestinationSpeed(const float speed);

        std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    private:

        void Initialize();

        void OnElectroLineOriginStartMovementDelayTimerTimeout();

        void OnElectroLineFadeoutTimerTimeout();

        void DropState();
    };
}