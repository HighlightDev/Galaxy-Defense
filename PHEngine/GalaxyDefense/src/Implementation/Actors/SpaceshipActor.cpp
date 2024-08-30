#include "SpaceshipActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"

namespace Game
{
    SpaceshipActor::SpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent),
          mModifiersHandler(std::make_unique<ModifiersHandler>()),
          mLifePoints(30),
          mDamageEffectTimePassed(0.0f),
          mDamageEffectDuration(0.5f),
          mDamageTimeProperty(std::make_shared<EngineObjectProperty<float>>(0.0f, "p_damageEffect")),
          mFreezingEffectProperty(std::make_shared<EngineObjectProperty<float>>(0.0f, "p_freezingEffect")),
          mIsDamageEffectActive(false)
    {
        AddEngineProperty(mDamageTimeProperty);
        AddEngineProperty(mFreezingEffectProperty);

        static constexpr size_t s_dmgTextShowDuration = 1500;
        mDamageMessageTimer.SetIntervalMs(s_dmgTextShowDuration);
        mDamageMessageTimer.SetIsRepeat(false);
        mDamageMessageTimer.SetIsPausable(true);
        mDamageMessageTimer.SetCallback([this]()
                                        { mUiComponent->SetVisibility(mDamageTextFieldId, false); });
    }

    void SpaceshipActor::OnSceneOwnerInitialized()
    {
        mUiComponent = GetComponentsByType<UiComponent>().back();
        mDamageTextFieldId = mUiComponent->CreateEmptyTextField("nimbus_mono",
                                                                10,
                                                                glm::vec3(1.0f, 0.0f, 0.0f),
                                                                true,
                                                                0.3f,
                                                                1,
                                                                eTextHorizontalAlignmentType::LEFT);
    }

    void SpaceshipActor::TriggerSpawn(const glm::vec3 &position)
    {
        SetIsEnabled(true);
        mActivityState = eSpaceshipActivityState::ACTIVE;
        const auto &onRouteMovementComponent = GetOnRouteMovementComponent();
        onRouteMovementComponent->ResetStates();
        onRouteMovementComponent->Teleport(position);
        RestoreLife();
    }

    void SpaceshipActor::TriggerExplosion()
    {
        SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
    }

    void SpaceshipActor::TriggerDisabled()
    {
        mModifiersHandler->RemoveAllModifiers();
        mActivityState = eSpaceshipActivityState::IDLE;
        SetIsEnabled(false);
    }

    void SpaceshipActor::Tick(const float deltaTime)
    {
        Actor::Tick(deltaTime);

        mModifiersHandler->Tick(deltaTime);

        if (mIsDamageEffectActive)
        {
            const float normDmgEffectTime = glm::clamp(mDamageEffectTimePassed / mDamageEffectDuration, 0.0f, 1.0f);
            mDamageTimeProperty->SetValue(normDmgEffectTime);

            if (mDamageEffectTimePassed < mDamageEffectDuration)
            {
                mDamageEffectTimePassed += deltaTime;
            }
            else
            {
                mIsDamageEffectActive = false;
                mDamageEffectTimePassed = 0.0f;
                mDamageTimeProperty->SetValue(0.0f);
            }
        }

        if (mDamageMessageTimer.IsRunning())
        {
            if (const auto &sceneSp = mSceneOwner.lock())
            {
                const auto &spaceShipTranslation = GetRootComponent()->GetTranslation();
                const auto &mainCameraSp = sceneSp->GetMainCamera();
                const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.z / clippedSpaceTranslation.w);

                const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                if (const auto &dmgTextFieldSp = mUiComponent->GetTextFieldById(mDamageTextFieldId))
                {
                    mUiComponent->SetPosition(mDamageTextFieldId,
                                              textureSpaceTranslation - (dmgTextFieldSp->GetScreenSpaceSize().x * 0.5f) + glm::vec2(0.0f, -0.2f));
                }
            }
        }
    }

    void SpaceshipActor::TriggerDamageReceived(const size_t dmg, const eDamageDealerType damageDealerType)
    {
        if (CheckIsAliveAfterDamage(dmg))
        {
            mIsDamageEffectActive = true;
            mDamageEffectTimePassed = 0.0f;

            const auto c_particle = GetComponentsByType<ParticleSystemComponent>().back();
            c_particle->EmitParticles();

            mUiComponent->SetPosition(mDamageTextFieldId, CalculatePositionForDamageText());
            mUiComponent->SetText(mDamageTextFieldId, std::to_string(dmg));
            mUiComponent->SetVisibility(mDamageTextFieldId, true);
        }
        else if (eDamageDealerType::MAIN_PLAYER == damageDealerType)
        {
            const auto &playerDataProvider = PlayerDataProvider::GetInstance();
            playerDataProvider->SetDestroyedEnemySpaceshipsCount(playerDataProvider->GetDestroyedEnemySpaceshipsCount() + 1);
        }

        mDamageMessageTimer.RestartTimer();
    }

    glm::vec2 SpaceshipActor::CalculatePositionForDamageText() const
    {
        const auto &spaceShipTranslation = GetRootComponent()->GetTranslation();

        if (const auto &sceneSp = mSceneOwner.lock())
        {
            if (const auto &dmgTextFieldSp = mUiComponent->GetTextFieldById(mDamageTextFieldId))
            {
                const auto &mainCameraSp = sceneSp->GetMainCamera();
                const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.z / clippedSpaceTranslation.w);

                const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                return (textureSpaceTranslation + glm::vec2(dmgTextFieldSp->GetScreenSpaceSize().x * -0.5f, -0.2f));
            }
        }

        return glm::vec2(spaceShipTranslation.x, spaceShipTranslation.y);
    }

    glm::vec3 SpaceshipActor::GetWorldPosition() const
    {
        return GetRootComponent()->GetTranslation();
    }

    bool SpaceshipActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
    }

    void SpaceshipActor::AddModifier(const std::shared_ptr<IModifiable> &modifier)
    {
        mModifiersHandler->AddModifier(modifier);
        if (modifier->GetModifierType() == eModifierType::Gravity)
        {
            GetOnRouteMovementComponent()->SetIsMovementOnRouteAllowed(false); // Spaceship is under gravity effect. Movement on the route is not allowed.
        }
    }

    bool SpaceshipActor::HasModifier(const eModifierType modifierType, const int32_t creatorObjectId) const
    {
        return mModifiersHandler->HasModifier(modifierType, creatorObjectId);
    }

    bool SpaceshipActor::HasModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->HasModifier(modifierType);
    }

    std::shared_ptr<IModifiable> SpaceshipActor::GetModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->GetModifier(modifierType);
    }

    void SpaceshipActor::RemoveModifier(const eModifierType modifierType, const int32_t creatorObjectId)
    {
        mModifiersHandler->RemoveModifier(modifierType, creatorObjectId);
    }

    bool SpaceshipActor::CheckIsAliveAfterDamage(const size_t dmg)
    {
        mLifePoints = mLifePoints >= dmg ? mLifePoints - dmg : 0;
        return 0 != mLifePoints;
    }

    bool SpaceshipActor::IsAlive() const
    {
        return 0 != mLifePoints;
    }

    void SpaceshipActor::SetDamageDeltaTime(const float deltaTime)
    {
        mDamageEffectTimePassed = deltaTime;
    }

    float SpaceshipActor::GetDamageDeltaTime() const
    {
        return mDamageEffectTimePassed;
    }

    void SpaceshipActor::SetIsDamageReceived(const bool isDamageReceived)
    {
        mIsDamageEffectActive = isDamageReceived;
        mDamageEffectTimePassed = 0.0f;
    }

    bool SpaceshipActor::GetIsDamageReceived() const
    {
        return mIsDamageEffectActive;
    }

    void SpaceshipActor::RestoreLife()
    {
        mLifePoints = 10;
    }

    eSpaceshipActivityState SpaceshipActor::GetSpaceshipActivityState() const
    {
        return mActivityState;
    }

    void SpaceshipActor::SetSpaceshipActivityState(const eSpaceshipActivityState activityState)
    {
        mActivityState = activityState;
    }

    void SpaceshipActor::SetFreezingEffectValue(const float value)
    {
        mFreezingEffectProperty->SetValue(value);
    }

    std::shared_ptr<OnRouteMovementComponent> SpaceshipActor::GetOnRouteMovementComponent() const
    {
        return std::dynamic_pointer_cast<OnRouteMovementComponent>(GetMovementComponent());
    }
}