#include "RigidBodyPhysicsComponent.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

using namespace EngineMath;

namespace EnginePhysics {
RigidBodyPhysicsComponent::RigidBodyPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data)
    : PhysicsComponent(data)
{
}

RigidBodyPhysicsComponent::~RigidBodyPhysicsComponent()
{
}

void RigidBodyPhysicsComponent::Tick(const float deltaTimeSec)
{
    Component::Tick(deltaTimeSec);

    if (mDescriptor->GetMotionState()) {
        bool bIsDirty;

        mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty, deltaTimeSec);

        bIsTransformationDirty = bIsDirty;

        if (bIsDirty) {
            if (const auto& spOwner = GetOwner().lock()) {
                spOwner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(mDescriptor->GetTranslation()));
                spOwner->GetRootComponent()->SetRotator(Converter::bulletToGlm(mDescriptor->GetRotator()));

                Event::PhysicsComponentUpdatedGameThreadEvent::GetInstance()->SendEvent(
                    Event::eExecutionOrder::PRE_EXECUTION, spOwner->GetName());
            }
        }
    }
}

} // namespace EnginePhysics