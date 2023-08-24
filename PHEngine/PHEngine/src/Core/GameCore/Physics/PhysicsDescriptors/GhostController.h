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
      int32_t mSavedCollisionFilterGroup;
      int32_t mSavedCollisionFilterMask;

      btTransform mMotionTransform;

      float mCollisionCooldown;
      static constexpr float sCollisionCooldownTimeout = 0.1f;

   public:
      GhostController(class PhysicsWorld *pPhysicsWorld,
                      const std::shared_ptr<CollisionShapeBase> &shape,
                      const float mass,
                      const int32_t collisionFilterGroup = btBroadphaseProxy::DefaultFilter,
                      const int32_t collisionFilterMask = btBroadphaseProxy::AllFilter);

      ~GhostController() override;

      void CleanUp() override;

      void CompletePhysicsDescriptorConstruction() override;

      void UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime) override;

      void SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation) override;

      void PostPhysicsSimulationUpdate(const float deltaTime) override;

      void SetIsCollisionEnabled(const bool isCollisionEnabled) override;

      ePhysicsDescriptorType GetPhysicsDescriptorType() const override;

      std::vector<btCollisionObject *> GetCollisionObjects() const override;

   private:
      void ParseGhostContacts();

      btScalar addSingleResult(btManifoldPoint &cp,
                               const btCollisionObjectWrapper *colObj0,
                               int partId0,
                               int index0,
                               const btCollisionObjectWrapper *colObj1,
                               int partId1,
                               int index1) override;
   };
}