#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/Modifiers/ModifiersHandler.h"

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace Game {
enum class eSpaceObjectActivityState { IDLE, ACTIVE };

class SpaceObjectActor : public Actor {
    std::unique_ptr<ModifiersHandler> mModifiersHandler;

protected:
    eSpaceObjectActivityState mActivityState{eSpaceObjectActivityState::IDLE};

public:
    SpaceObjectActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    virtual bool IsInsideLevel(const BoundingBox3D& boundingBox) const;

    void Tick(const float deltaTime) override;

    virtual void TriggerSpawn(const glm::vec3& position);

    virtual void TriggerDisabled();

    void AddModifier(const std::shared_ptr<IModifiable>& modifier);

    bool HasModifier(const eModifierType modifierType, const int32_t creatorObjectId) const;

    bool HasModifier(const eModifierType modifierType) const;

    std::shared_ptr<IModifiable> GetModifier(const eModifierType modifierType) const;

    void RemoveModifier(const eModifierType modifierType, const int32_t creatorObjectId);

    eSpaceObjectActivityState GetActivityState() const;

    glm::vec3 GetWorldPosition() const;
};
} // namespace Game