#pragma once
#include "PhysicsDescriptor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"

namespace EnginePhysics
{
   class PhysicsWorld;

   class RigidBodyController
       : public PhysicsDescriptor,
         public Event::KinematicBodyMovedGameThreadEvent
   {
      size_t mLastRayCastObjectResultId;

   public:
      RigidBodyController(const std::shared_ptr<PhysicsWorld> &pPhysicsWorld,
                          const std::shared_ptr<CollisionShapeBase> &shape,
                          const ePhysicsBodyType bodyType,
                          const float mass = 0.0f,
                          const MotionModifiers &motionModifier = MotionModifiers());

      ~RigidBodyController() override;

      void CleanUp() override;

      void CompletePhysicsDescriptorConstruction() override;

      void UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime) override;

      void SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation) override;

      void ProcessEvent(const KinematicBodyMovedGameThreadEvent* sender, const typename Event::KinematicBodyMovedGameThreadEvent::EventData_t &data) override;

      ePhysicsDescriptorType GetPhysicsDescriptorType() const override;

      std::vector<btCollisionObject *> GetCollisionObjects() const override;

   private:
      bool DoRayCastDown();
   };
}
