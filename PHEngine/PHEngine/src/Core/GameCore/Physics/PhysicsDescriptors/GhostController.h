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
       : public PhysicsDescriptor,
         public btCollisionWorld::ContactResultCallback
   {
   private:
      btPairCachingGhostObject *mGhostObject;

      btTransform mMotionTransform;

      float mCollisionCooldown;
      static constexpr float sCollisionCooldownTimeout = 0.5f;

   public:
      GhostController(class PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape, const float mass);

      virtual ~GhostController();

      virtual void CompletePhysicsDescriptorConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation) override;

      virtual void PostPhysicsSimulationUpdate(const float deltaTime) override;

   private:
      void ParseGhostContacts();

      virtual btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0,
                                       int partId0, int index0,
                                       const btCollisionObjectWrapper *colObj1,
                                       int partId1,
                                       int index1) override;
   };
}