#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/ActorLeveling/SpaceshipLevel.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"
#include "Implementation/DamageDealerType.h"
#include "Implementation/Modifiers/IModifiable.h"
#include "Implementation/Modifiers/ModifiersHandler.h"

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

enum class eSpaceshipActivityState { IDLE, ACTIVE, PENDING_DISABLE };

namespace Game {

class SpaceObjectUiComponent;

class SpaceshipActor : public Actor {
protected:
    eSpaceshipActivityState mActivityState{eSpaceshipActivityState::IDLE};

    std::unique_ptr<ModifiersHandler> mModifiersHandler;

    SpaceshipLevel mSpaceshipLevel;

    std::shared_ptr<SpaceObjectUiComponent> mUiComponent;

    std::shared_ptr<GameThreadTimer> mDamageMessageTimer;

    std::shared_ptr<GameThreadTimer> mDmgShakeTimer;

    float mShakeTimePassed{0.0f};

    float mDamageEffectTimePassed;

    float mDamageEffectDuration;

    bool mIsDamageEffectActive;

    std::shared_ptr<EngineObjectProperty<float>> mDamageTimeProperty;
    std::shared_ptr<EngineObjectProperty<float>> mFreezingEffectProperty;

public:
    SpaceshipActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const SpaceshipLevel& spaceshipLevel);

    void Tick(const float deltaTimeSec) override;

    void OnSceneOwnerInitialized() override;

    virtual bool IsInsideLevel(const BoundingBox3D& boundingBox) const;

    virtual void TriggerDamageReceived(const size_t damage, const eDamageDealerType damageDealerType);

    virtual void TriggerSpawn(const glm::vec3& position);

    virtual void TriggerExplosion();

    virtual void TriggerDisabled();

    glm::vec3 GetWorldPosition() const;

    void AddModifier(const std::shared_ptr<IModifiable>& modifier);

    bool HasModifier(const eModifierType modifierType, const int32_t creatorObjectId) const;

    bool HasModifier(const eModifierType modifierType) const;

    std::shared_ptr<IModifiable> GetModifier(const eModifierType modifierType) const;

    void RemoveModifier(const eModifierType modifierType, const int32_t creatorObjectId);

    bool CheckIsAliveAfterDamage(const size_t dmg);

    bool IsAlive() const;

    void SetDamageDeltaTime(const float deltaTimeSec);

    float GetDamageDeltaTime() const;

    void SetIsDamageReceived(const bool isDamageReceived);

    bool GetIsDamageReceived() const;

    eSpaceshipActivityState GetSpaceshipActivityState() const;

    void SetSpaceshipActivityState(const eSpaceshipActivityState activityState);

    void SetFreezingEffectValue(const float value);

    std::shared_ptr<OnRouteMovementComponent> GetOnRouteMovementComponent() const;
};
} // namespace Game