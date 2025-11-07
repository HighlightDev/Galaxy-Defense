#pragma once

#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "PhysicsComponent.h"

namespace EnginePhysics {

class CharacterPhysicsComponent : public PhysicsComponent {
    std::shared_ptr<DynamicCharacterController> characterController;

public:
    CharacterPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data);

    ~CharacterPhysicsComponent() override;

    void Tick(const float deltaTimeSec) override;

    void SetWalkVelocity(const glm::vec3& velocity);

    void SetJumpVelocity();
};
} // namespace EnginePhysics
