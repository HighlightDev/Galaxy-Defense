#include "SpaceshipActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/ComponentData/UiComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"
#include "Implementation/Components/UiComponents/SpaceObjectUiComponent.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

using namespace EngineCore::DataProviders;

namespace Game {
SpaceshipActor::SpaceshipActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const SpaceshipLevel& spaceshipLevel)
    : Actor(gameObjectName, rootComponent)
    , mModifiersHandler(std::make_unique<ModifiersHandler>())
    , mSpaceshipLevel(spaceshipLevel)
    , mDamageMessageTimer(std::make_shared<GameThreadTimer>())
    , mDmgShakeTimer(std::make_shared<GameThreadTimer>())
    , mDamageEffectTimePassed(0.0f)
    , mDamageEffectDuration(0.5f)
    , mDamageTimeProperty(std::make_shared<EngineObjectProperty<float>>(0.0f, "p_damageEffect"))
    , mFreezingEffectProperty(std::make_shared<EngineObjectProperty<float>>(0.0f, "p_freezingEffect"))
    , mIsDamageEffectActive(false)
{
    AddEngineProperty(mDamageTimeProperty);
    AddEngineProperty(mFreezingEffectProperty);

    mDamageMessageTimer->Initialize();
    mDamageMessageTimer->SetIntervalMs(Game::Constants::c_dmgTextShowDuration);
    mDamageMessageTimer->SetIsRepeat(false);
    mDamageMessageTimer->SetIsPausable(true);
    mDamageMessageTimer->SetCallback([this]() { mUiComponent->FadeOut(); });

    mDmgShakeTimer->Initialize();
    mDmgShakeTimer->SetIntervalMs(Game::Constants::c_shakeDurationMs);
    mDmgShakeTimer->SetIsRepeat(false);
    mDmgShakeTimer->SetIsPausable(true);
    mDmgShakeTimer->SetCallback([this]() {
        const auto& rootComponent = GetRootComponent();
        ext_assert(rootComponent, "SpaceshipActor root component is null in damage shake callback");
        rootComponent->SetRotator(glm::quat()); // reset rotation
        mShakeTimePassed = 0.0f;
    });
}

void SpaceshipActor::OnSceneOwnerInitialized()
{
    mUiComponent = GetComponentsByType<SpaceObjectUiComponent>().back();
}

void SpaceshipActor::TriggerSpawn(const glm::vec3& position)
{
    SetIsEnabled(true);
    mActivityState = eSpaceshipActivityState::ACTIVE;
    const auto& onRouteMovementComponent = GetOnRouteMovementComponent();
    onRouteMovementComponent->ResetStates();
    onRouteMovementComponent->Teleport(position);
    mSpaceshipLevel.RestoreHealth();
}

void SpaceshipActor::TriggerExplosion()
{
    SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
}

void SpaceshipActor::TriggerDisabled()
{
    mModifiersHandler->RemoveAllModifiers();
    mActivityState = eSpaceshipActivityState::IDLE;
    mShakeTimePassed = 0.0f;
    mDmgShakeTimer->StopTimer();
    mDamageMessageTimer->StopTimer();
    SetIsEnabled(false);
    mDamageTimeProperty->SetValue(0.0f);
    mFreezingEffectProperty->SetValue(0.0f);
    mUiComponent->SetHealthBarVisibility(false);
    mUiComponent->SetLabelVisibility(false);
}

void SpaceshipActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);

    mModifiersHandler->Tick(deltaTimeSec);

    if (mIsDamageEffectActive) {
        const float normDmgEffectTime = glm::clamp(mDamageEffectTimePassed / mDamageEffectDuration, 0.0f, 1.0f);
        mDamageTimeProperty->SetValue(normDmgEffectTime);

        if (mDamageEffectTimePassed < mDamageEffectDuration) {
            mDamageEffectTimePassed += deltaTimeSec;
        } else {
            mIsDamageEffectActive = false;
            mDamageEffectTimePassed = 0.0f;
            mDamageTimeProperty->SetValue(0.0f);
        }
    }

    // Shake Effect
    if (mDmgShakeTimer->IsRunning()) {
        mShakeTimePassed += deltaTimeSec;
        const auto& rootComponent = GetRootComponent();
        constexpr float c_shakeSpeed = 30.0f;
        constexpr float c_shakeAmplitudeDegrees = 10.0f;
        const float shakeAmountMs = std::sin(mShakeTimePassed * c_shakeSpeed);
        const float shakeRollClampRadians = DEG_TO_RAD(c_shakeAmplitudeDegrees);
        const float resultNormalizedRollRadians = EngineMath::NormalizeAngleRadians(shakeAmountMs * shakeRollClampRadians);
        rootComponent->SetRotator(glm::quat(glm::vec3(0.0f, 0.0f, resultNormalizedRollRadians)));
    }
}

void SpaceshipActor::TriggerDamageReceived(const size_t dmg, const eDamageDealerType damageDealerType)
{
    if (CheckIsAliveAfterDamage(dmg)) {
        mIsDamageEffectActive = true;
        mDamageEffectTimePassed = 0.0f;

        const auto c_particle = GetComponentsByType<CpuParticleSystemComponent>().back();
        c_particle->EmitParticles();

        mUiComponent->SetLabelText(std::to_string(dmg));
        mUiComponent->FadeIn();
    } else if (eDamageDealerType::MAIN_PLAYER == damageDealerType) {
        const auto& playerDataProvider = PlayerDataProvider::GetInstance();
        playerDataProvider->SetDestroyedEnemySpaceshipsCount(playerDataProvider->GetDestroyedEnemySpaceshipsCount() + 1);
    }

    mDamageMessageTimer->RestartTimer();
    mDmgShakeTimer->RestartTimer();
}

glm::vec3 SpaceshipActor::GetWorldPosition() const
{
    return GetRootComponent()->GetTranslation();
}

bool SpaceshipActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
}

void SpaceshipActor::AddModifier(const std::shared_ptr<IModifiable>& modifier)
{
    mModifiersHandler->AddModifier(modifier);
    if (modifier->GetModifierType() == eModifierType::Gravity) {
        GetOnRouteMovementComponent()->SetIsMovementOnRouteAllowed(
            false); // Spaceship is under gravity effect. Movement on the route is not allowed.
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
    mSpaceshipLevel.DecreaseHealth(dmg);
    return IsAlive();
}

bool SpaceshipActor::IsAlive() const
{
    return mSpaceshipLevel.GetHealth() > 0;
}

void SpaceshipActor::SetDamageDeltaTime(const float deltaTimeSec)
{
    mDamageEffectTimePassed = deltaTimeSec;
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

} // namespace Game