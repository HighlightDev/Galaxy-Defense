#include "BlackHoleMissileActor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Implementation/MissileExplosionVisitors/BlackHoleExplosionVisitor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game
{
    BlackHoleMissileActor::BlackHoleMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent,
                                                 const std::shared_ptr<CombatActorsPoolHandler> &combatActorsPoolHandler)
        : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler),
          mCombatActivePhaseActor(),
          mExplosionSecondPhaseActor()
    {
        mMissileType = eMissileType::BLACK_HOLE;
    }

    void BlackHoleMissileActor::AttachTweener(std::shared_ptr<Tweener> tweener)
    {
        assert(tweener);
        LogInfo("BlackHoleMissileActor::AttachTweener => Path to tweener", tweener->GetRelPathTweener());

        Actor::AttachTweener(tweener);
        mBlackMissileTweener = tweener;
        InitTweenerSubscriptions();
    }

    void BlackHoleMissileActor::InitTweenerSubscriptions()
    {
        mBlackMissileTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<BlackHoleMissileActor>(shared_from_this()));
    }

    void BlackHoleMissileActor::AddCombatActivePhaseActor(const std::shared_ptr<Actor> &combatActivePhaseActor)
    {
        assert(!mCombatActivePhaseActor);
        mCombatActivePhaseActor = combatActivePhaseActor;
        AddChild(combatActivePhaseActor);
    }

    void BlackHoleMissileActor::AddExplosionSecondPhaseActor(const std::shared_ptr<MissileActor> &explosionSecondPhaseActor)
    {
        assert(!mExplosionSecondPhaseActor);
        mExplosionSecondPhaseActor = explosionSecondPhaseActor;
        AddChild(mExplosionSecondPhaseActor);
    }

    const std::shared_ptr<Actor> &BlackHoleMissileActor::GetCombatActivePhaseActor() const
    {
        return mCombatActivePhaseActor;
    }

    const std::shared_ptr<MissileActor> &BlackHoleMissileActor::GetExplosionPhaseActor() const
    {
        return mExplosionSecondPhaseActor;
    }

    void BlackHoleMissileActor::OnTweenStateChanged(const std::string &stateName)
    {
        if ("s_FirstPhasePreload" == stateName)
        {
        }
        else if ("s_FirstPhaseActiveCombat" == stateName)
        {
        }
        else if ("s_FirstPhaseExplosion" == stateName)
        {
            mExplosionSecondPhaseActor->TriggerSpawn(mCombatActivePhaseActor->GetRootComponent()->GetTranslation(),
                                                     {},
                                                     0.0f,
                                                     mDamageDealerType,
                                                     shared_from_this());
            const auto c_particle = mExplosionSecondPhaseActor->GetComponentsByType<ParticleSystemComponent>().back();
            c_particle->EmitParticles(100);
            TriggerLifecycle_SecondPhaseExplosion();
        }
        else if ("s_SecondPhaseExplosion" == stateName)
        {
            TriggerExplosionFinished();
        }
        else if ("s_BlackHoleSuckIn" == stateName)
        {
            TriggerDisabled();
        }
        else
        {
            assert(false);
        }
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhasePreload()
    {
        assert(mBlackMissileTweener);
        mBlackMissileTweener->ChangeState("s_FirstPhasePreload");
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhaseActiveCombat()
    {
        assert(mBlackMissileTweener);
        mBlackMissileTweener->ChangeState("s_FirstPhaseActiveCombat");
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhaseExplosion()
    {
        assert(mBlackMissileTweener);
        mBlackMissileTweener->ChangeState("s_FirstPhaseExplosion");
    }

    void BlackHoleMissileActor::TriggerLifecycle_SecondPhaseExplosion()
    {
        assert(mBlackMissileTweener);
        mBlackMissileTweener->ChangeState("s_SecondPhaseExplosion");
    }

    void BlackHoleMissileActor::TriggerLifecycle_BlackHoleSuckIn()
    {
        assert(mBlackMissileTweener);
        mBlackMissileTweener->ChangeState("s_BlackHoleSuckIn");
    }

    void BlackHoleMissileActor::TriggerSpawn(const glm::vec3 &position,
                                             const glm::vec3 &direction,
                                             const float yawDegrees,
                                             const eDamageDealerType ownerType,
                                             const std::shared_ptr<Actor> &spawnerActor)
    {
        mDamageDealerType = ownerType;
        mActivityState = eMissileActivityState::ACTIVE;
        SetIsEnabled(true);
        const auto &existingRotation = mCombatActivePhaseActor->GetRootComponent()->GetAdditionalRotation();
        mCombatActivePhaseActor->GetRootComponent()->SetAdditionalRotation(glm::vec3(existingRotation.x, yawDegrees, existingRotation.z));
        mCombatActivePhaseActor->GetMovementComponent()->SetDirection(direction);
        mCombatActivePhaseActor->GetMovementComponent()->Teleport(position);

        TriggerLifecycle_FirstPhaseActiveCombat();
    }

    void BlackHoleMissileActor::TriggerExplosion()
    {
        mActivityState = eMissileActivityState::EXPLOSION;
        const auto c_soundList = GetComponentsByType<SoundComponent>();
        assert(c_soundList.size());
        c_soundList.back()->PlayBuffer("explosion");
        TriggerLifecycle_FirstPhaseExplosion();
    }

    void BlackHoleMissileActor::TriggerExplosionFinished()
    {
        mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
        TriggerLifecycle_BlackHoleSuckIn();
    }

    void BlackHoleMissileActor::TriggerDisabled()
    {
        mActivityState = eMissileActivityState::IDLE;
        mBlackMissileTweener->InitRootState();
    }

    bool BlackHoleMissileActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(),
                                           boundingBox.GetMax(),
                                           mCombatActivePhaseActor->GetRootComponent()->GetTranslation());
    }

    std::shared_ptr<MissileExplosionVisitorBase> BlackHoleMissileActor::CreateMissileExplosionVisitor()
    {
        return std::make_shared<BlackHoleExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
    }
}