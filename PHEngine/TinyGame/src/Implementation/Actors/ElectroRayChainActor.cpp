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
        : MissileActor(gameObjectName, rootComponent),
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
        MissileActor::Tick(deltaTime);

        assert(mLineComponent);

        // todo: temporary
        mElectroLineBegin = GetStartLinePosition();
        mElectroLineEnd = GetEndLinePosition();
        mLineComponent->SetLineBeginWorldSpacePosition(mElectroLineBegin);
        mLineComponent->SetLineEndWorldSpacePosition(mElectroLineEnd);
    }

    bool ElectroRayChainActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return true;
    }

    void ElectroRayChainActor::TriggerSpawn(const glm::vec3 &position)
    {
        SetIsEnabled(true);
        mActivityState = eMissileActivityState::ACTIVE;
    }

    void ElectroRayChainActor::TriggerExplosion()
    {
        mActivityState = eMissileActivityState::EXPLOSION;
        TriggerExplosionFinished();
    }

    void ElectroRayChainActor::TriggerExplosionFinished()
    {
        mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
        TriggerDisabled();
    }

    void ElectroRayChainActor::TriggerDisabled()
    {
        mActivityState = eMissileActivityState::IDLE;
        SetIsEnabled(false);
    }

    std::shared_ptr<MissileExplosionVisitorBase> ElectroRayChainActor::CreateMissileExplosionVisitor()
    {
        return nullptr;
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
            return startLineSpaceshipSp->GetRootComponent()->GetTranslation();
        }
        return mElectroLineBegin;
    }

    glm::vec3 ElectroRayChainActor::GetEndLinePosition()
    {
        if (const auto &endLineSpaceshipSp = mEndLineSpaceship.lock())
        {
            return endLineSpaceshipSp->GetRootComponent()->GetTranslation();
        }
        return mElectroLineEnd;
    }

    void ElectroRayChainActor::DropState()
    {
    }
}