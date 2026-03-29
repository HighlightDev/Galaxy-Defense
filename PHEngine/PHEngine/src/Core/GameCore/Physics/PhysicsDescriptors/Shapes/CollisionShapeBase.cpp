#include "CollisionShapeBase.h"

#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"

namespace EnginePhysics {

CollisionShapeBase::CollisionShapeBase(btCollisionShape* shape)
    : mCollisionShape(shape)
{
}

CollisionShapeBase::~CollisionShapeBase()
{
    delete mCollisionShape;
}

btCollisionShape* CollisionShapeBase::GetCollisionShape() const
{
    return mCollisionShape;
}

void CollisionShapeBase::SetParentPhysicsComponent(const std::weak_ptr<PhysicsComponent>& parentPhysicsComponent)
{
    mParentPhysicsComponent = parentPhysicsComponent;
}

std::weak_ptr<PhysicsComponent> CollisionShapeBase::GetParentPhysicsComponent() const
{
    return mParentPhysicsComponent;
}

} // namespace EnginePhysics
