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

      RigidBodyController(class PhysicsWorld* pPhysicsWorld, PhysicsShapeBase* shape, const ePhysicsBodyType bodyType, const float mass = 0.0f, const MotionModifiers& motionModifier = MotionModifiers());

      virtual ~RigidBodyController();

      virtual void CompletePhysicsDescriptorConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;

      virtual void ProcessEvent(const typename Event::KinematicBodyMovedEvent::EventData_t& data) override;

      virtual ePhysicsDescriptorType GetPhysicsDescriptorType() const override;

   private:

      bool DoRayCastDown();
   };
}

