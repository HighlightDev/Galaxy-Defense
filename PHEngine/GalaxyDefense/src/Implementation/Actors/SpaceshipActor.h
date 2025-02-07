#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"
#include "Implementation/DamageDealerType.h"
#include "Implementation/Modifiers/IModifiable.h"
#include "Implementation/Modifiers/ModifiersHandler.h"

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
class UiComponent;
}

enum class eSpaceshipActivityState { IDLE, ACTIVE, PENDING_DISABLE };

namespace Game {
class SpaceshipActor : public Actor {
protected:
    eSpaceshipActivityState mActivityState{eSpaceshipActivityState::IDLE};

    std::unique_ptr<ModifiersHandler> mModifiersHandler;

    size_t mLifePoints;

    std::shared_ptr<::EngineCore::UiComponent> mUiComponent;

    int32_t mDamageTextFieldId{-1};

    GameThreadTimer mDamageMessageTimer;

    float mDamageEffectTimePassed;

    float mDamageEffectDuration;

    bool mIsDamageEffectActive;

    std::shared_ptr<EngineObjectProperty<float>> mDamageTimeProperty;
    std::shared_ptr<EngineObjectProperty<float>> mFreezingEffectProperty;

public:
    SpaceshipActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTime) override;

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

    void SetDamageDeltaTime(const float deltaTime);

    float GetDamageDeltaTime() const;

    void SetIsDamageReceived(const bool isDamageReceived);

    bool GetIsDamageReceived() const;

    void RestoreLife();

    eSpaceshipActivityState GetSpaceshipActivityState() const;

    void SetSpaceshipActivityState(const eSpaceshipActivityState activityState);

    void SetFreezingEffectValue(const float value);

    std::shared_ptr<OnRouteMovementComponent> GetOnRouteMovementComponent() const;

protected:
    virtual glm::vec2 CalculatePositionForDamageText() const;
};
} // namespace Game