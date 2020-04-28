#pragma once
#include "PhysicsDescriptor.h"

namespace EnginePhysics
{
   class RigidBodyController :
      public PhysicsDescriptor
   {
   public:

      RigidBodyController(class PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const float mass = 0.0f, const MotionModifiers& motionModifier = MotionModifiers());

      virtual ~RigidBodyController();

      virtual void CompleteRigidBodyConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;
   };
}

