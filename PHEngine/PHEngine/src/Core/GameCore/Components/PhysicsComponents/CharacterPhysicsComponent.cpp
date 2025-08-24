#include "CharacterPhysicsComponent.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

using namespace EngineMath;

namespace EnginePhysics {
CharacterPhysicsComponent::CharacterPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data)
    : PhysicsComponent(data)
{
    characterController = std::dynamic_pointer_cast<DynamicCharacterController>(data->mPhysicsDescriptor);
    assert(characterController);
}

CharacterPhysicsComponent::~CharacterPhysicsComponent()
{
}

void CharacterPhysicsComponent::Tick(const float deltaTime)
{
    characterController->UpdateMotionWorldTransformLocalState(bIsTransformationDirty, deltaTime);

    if (bIsTransformationDirty) {
        if (const auto& spOwner = GetOwner().lock()) {
            spOwner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(characterController->GetTranslation()));
            Event::PhysicsComponentUpdatedGameThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, spOwner->GetName());
        }
    }
}

void CharacterPhysicsComponent::SetWalkVelocity(const glm::vec3& velocity)
{
    characterController->Walk(velocity);
}

void CharacterPhysicsComponent::SetJumpVelocity()
{
    characterController->Jump();
}
} // namespace EnginePhysics