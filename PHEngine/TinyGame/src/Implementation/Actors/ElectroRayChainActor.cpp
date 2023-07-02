#include "ElectroRayChainActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/LoggerExtension.h"

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
          mEndLineSpaceship(),
          mChainingAnimationTimeDuration(0.5f),
          mOpacity(std::make_shared<EngineObjectProperty<float>>(1.0f, "p_opacity"))
    {
        AddEngineProperty(mOpacity);
    }

    void ElectroRayChainActor::AttachTweener(std::shared_ptr<Tweener> tweener)
    {
        assert(tweener);
        LogInfo("ElectroRayChainActor::AttachTweener => Path to tweener", tweener->GetRelPathTweener());

        MissileActor::AttachTweener(tweener);
        mFadeoutTweener = tweener;
        InitTweenerSubscriptions();
    }

    void ElectroRayChainActor::Tick(const float deltaTime)
    {
        MissileActor::Tick(deltaTime);

        assert(mLineComponent);

        mElectroLineBegin = GetStartLinePosition();
        mElectroLineEnd = GetEndLinePosition();

        if (mIsChainingAnimationPlaying && mChainingAnimationTime >= mChainingAnimationTimeDuration)
        {
            mChainingAnimationTime = mChainingAnimationTimeDuration;
            TriggerLifecycle_OnFadeOutStarted();
        }
        else
        {
            mChainingAnimationTime += deltaTime;
        }

        const auto &toEndLineVec = mElectroLineEnd - mElectroLineBegin;
        const float distance = glm::length(toEndLineVec);
        const auto &nToEndLineVec = toEndLineVec / distance;
        const float t = std::clamp(EngineMath::LerpFloat(mChainingAnimationTime, 0.0f, mChainingAnimationTimeDuration, 0.0f, 1.0f), 0.0f, 1.0f);
        const float endLineDistance = t * distance;
        const auto &finalDestination = mElectroLineBegin + (nToEndLineVec * endLineDistance);

        mLineComponent->SetLineBeginWorldSpacePosition(mElectroLineBegin);
        mLineComponent->SetLineEndWorldSpacePosition(finalDestination);
    }

    bool ElectroRayChainActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return true;
    }

    void ElectroRayChainActor::TriggerSpawn(const glm::vec3 &position)
    {
        DropState();
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
        mFadeoutTweener->InitRootState();
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
            mElectroLineBegin = startLineSpaceshipSp->GetRootComponent()->GetTranslation();
        }

        return mElectroLineBegin;
    }

    glm::vec3 ElectroRayChainActor::GetEndLinePosition()
    {
        if (const auto &endLineSpaceshipSp = mEndLineSpaceship.lock())
        {
            mElectroLineEnd = endLineSpaceshipSp->GetRootComponent()->GetTranslation();
        }

        return mElectroLineEnd;
    }

    void ElectroRayChainActor::SetIsPendingDisable(const bool value)
    {
        mIsPendingDisable = false;
    }

    bool ElectroRayChainActor::IsPendingDisable() const
    {
        return mIsPendingDisable;
    }

    void ElectroRayChainActor::DropState()
    {
        mIsChainingAnimationPlaying = true;
        mIsPendingDisable = false;
        mChainingAnimationTime = 0.0f;
        mElectroLineBegin = mElectroLineEnd = glm::vec3();
    }

    void ElectroRayChainActor::OnTweenStateChanged(const std::string &stateName)
    {
        LogInfo("ElectroRayChainActor::OnTweenStateChanged => stateName: ", stateName);
        if ("s_OnFadeOutFinished" == stateName)
        {
            mIsPendingDisable = true;
        }
    }

    void ElectroRayChainActor::InitTweenerSubscriptions()
    {
        mFadeoutTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<ElectroRayChainActor>(shared_from_this()));
    }

    void ElectroRayChainActor::TriggerLifecycle_OnFadeOutStarted()
    {
        LogInfo("ElectroRayChainActor::TriggerLifecycle_OnFadeOutStarted");
        mIsChainingAnimationPlaying = false;
        mFadeoutTweener->ChangeState("s_OnFadeOutFinished");
    }
}