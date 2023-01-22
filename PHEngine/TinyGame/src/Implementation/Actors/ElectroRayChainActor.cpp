#include "ElectroRayChainActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"

#include <utility>

using namespace EnginePhysics;
using namespace Event;

namespace Game
{
    ElectroRayChainActor::ElectroRayChainActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent),
          mLineComponent(),
          mElectroLineBegin(),
          mElectroLineEnd(),
          mStartLineSpaceship(),
          mEndLineSpaceship()
    {
        Initialize();
    }

    void ElectroRayChainActor::Initialize()
    {
    }

    void ElectroRayChainActor::Tick(const float deltaTime)
    {
        Actor::Tick(deltaTime);

        assert(mLineComponent);

        mLineComponent->SetLineBeginWorldSpacePosition(mElectroLineBegin);
        mLineComponent->SetLineEndWorldSpacePosition(mElectroLineEnd);
    }

    void ElectroRayChainActor::SetStartLineSpaceship(const std::weak_ptr<Actor> &startLineSpaceship)
    {
        mStartLineSpaceship = startLineSpaceship;
    }

    void ElectroRayChainActor::SetEndLineSpaceship(const std::weak_ptr<Actor> &endLineSpaceship)
    {
        mEndLineSpaceship = endLineSpaceship;
    }

    void ElectroRayChainActor::SetLineComponent(const std::shared_ptr<RuntimeGeneratedLineComponent> &lineComponent)
    {
        mLineComponent = lineComponent;
    }

    glm::vec3 ElectroRayChainActor::GetStartLinePosition()
    {
        if (const auto &startLineSpaceshipSp = mStartLineSpaceship.lock())
        {
        }
        return {};
    }

    glm::vec3 ElectroRayChainActor::GetEndLinePosition()
    {
        if (const auto &endLineSpaceshipSp = mEndLineSpaceship.lock())
        {
        }
        return {};
    }

    void ElectroRayChainActor::DropState()
    {
    }
}