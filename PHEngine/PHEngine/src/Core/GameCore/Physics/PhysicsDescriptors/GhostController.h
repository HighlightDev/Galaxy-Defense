#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "PhysicsDescriptor.h"

namespace EnginePhysics
{
   class GhostController
       : public PhysicsDescriptor
   {
   private:
      btPairCachingGhostObject *mGhostObject;

      btTransform mMotionTransform;

   public:
      GhostController(class PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape, const float mass);

      virtual ~GhostController();

      virtual void CompletePhysicsDescriptorConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation) override;

      virtual void PostPhysicsSimulationUpdate() override;

   private:

      void ParseGhostContacts();
   };
}