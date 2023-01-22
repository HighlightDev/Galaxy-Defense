#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Actor.h"
#include "Core/CommonCore/Timer.h"

#include <functional>
#include <unordered_map>
#include <memory>
#include <glm/vec3.hpp>

using namespace EngineCore;

namespace EngineCore
{
    class RuntimeGeneratedLineComponent;
}

namespace Game
{
    class MissileExplosionVisitorBase;

    class ElectroRayChainActor
        : public Actor
    {
        std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> mLineComponent;

        glm::vec3 mElectroLineBegin;
        glm::vec3 mElectroLineEnd;

        std::weak_ptr<::EngineCore::Actor> mStartLineSpaceship;
        std::weak_ptr<::EngineCore::Actor> mEndLineSpaceship;

    public:
        ElectroRayChainActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        void Tick(const float deltaTime) override;

        void SetStartLineSpaceship(const std::weak_ptr<Actor> &startLineSpaceship);

        void SetEndLineSpaceship(const std::weak_ptr<Actor> &endLineSpaceship);

        void SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> &lineComponent);

    private:
        void Initialize();

        void DropState();

        glm::vec3 GetStartLinePosition();

        glm::vec3 GetEndLinePosition();
    };
}