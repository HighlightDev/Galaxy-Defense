#pragma once
#include "PhysicsDescriptor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"

namespace EnginePhysics
{
   class RigidBodyController
      : public PhysicsDescriptor
      , public Event::KinematicBodyMovedEvent
   {
      PhysicsDescriptor* mLastRayCastObjectResult;

   public:

      RigidBodyController(class PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const PhysicsBodyType bodyType, const float mass = 0.0f, const MotionModifiers& motionModifier = MotionModifiers());

      virtual ~RigidBodyController();

      virtual void CompleteRigidBodyConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;

      virtual void ProcessEvent(const Event::KinematicBodyMovedEvent::EventData_t& data) override;

   private:

      bool DoRayCastDown();
   };
}

