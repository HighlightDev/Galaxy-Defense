#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "PhysicsDescriptor.h"

namespace EnginePhysics
{

   class DynamicCharacterController
      : public PhysicsDescriptor
   {
   private:
      // Physics
      btPairCachingGhostObject* mGhostObject;

      bool mOnGround;
      bool mHittingWall;

      float mDeceleration;
      float mMaxSpeed;
      float mJumpImpulse;
      float mJumpRechargeTime;
      float mJumpRechargeTimer;
      float mBottomYOffset;
      float mBottomRoundedRegionYOffset;
      float mStepHeight;
      float mTimerMultiplier;

      btTransform mMotionTransform;
      btVector3 mPreviousPosition;

      btVector3 mManualVelocity;
      std::vector<btVector3> mSurfaceHitNormals;

   public:

      DynamicCharacterController(class PhysicsWorld* pPhysicsWorld, float capsuleRadius, float capsuleHeight, float mass, float stepHeight);

      virtual ~DynamicCharacterController();

      virtual void CompleteRigidBodyConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;

      // Acceleration vector in XZ plane
      void Walk(const glm::vec2& dir);

      // Ignores y
      void Walk(const glm::vec3& dir);

      void Jump();

      bool IsOnGround() const;

   private:

      void ParseGhostContacts();

      void UpdatePosition();

      void UpdateVelocity();
   };
}