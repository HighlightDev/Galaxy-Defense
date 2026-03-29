#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>

#include <memory>

namespace EnginePhysics {

class PhysicsComponent;

struct CollisionShapeBase {
protected:
    btCollisionShape* mCollisionShape = nullptr;

    std::weak_ptr<PhysicsComponent> mParentPhysicsComponent;

public:
    explicit CollisionShapeBase(btCollisionShape* shape);

    virtual ~CollisionShapeBase();

    btCollisionShape* GetCollisionShape() const;

    void SetParentPhysicsComponent(const std::weak_ptr<PhysicsComponent>& parentPhysicsComponent);

    std::weak_ptr<PhysicsComponent> GetParentPhysicsComponent() const;
};
} // namespace EnginePhysics
