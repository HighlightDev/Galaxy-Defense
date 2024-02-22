#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "PhysicsDescriptor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"

namespace EnginePhysics
{
   class PhysicsWorld;

   class DynamicCharacterController
      : public PhysicsDescriptor
      , public Event::KinematicBodyMovedGameThreadEvent
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

      PhysicsDescriptor* mLastRayCastObjectResult;

   public:

      DynamicCharacterController(const std::shared_ptr<PhysicsWorld>& pPhysicsWorld, float capsuleRadius, float capsuleHeight, float mass, float stepHeight);

      ~DynamicCharacterController() override;

      void Initialize() override;

      void CleanUp() override;

      void CompletePhysicsDescriptorConstruction() override;

      void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) override;

      void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;

      void ProcessEvent(const typename Event::KinematicBodyMovedGameThreadEvent::EventData_t& data) override;

      ePhysicsDescriptorType GetPhysicsDescriptorType() const override;

      std::vector<btCollisionObject*> GetCollisionObjects() const override;

      // Acceleration vector in XZ plane
      void Walk(const glm::vec2& dir);

      // Ignores y
      void Walk(const glm::vec3& dir);

      void Jump();

      bool IsOnGround() const;

      float GetStepHeight() const;

      float GetCapsuleHeight() const;

      float GetCapsuleRadius() const;

   private:

      void ParseGhostContacts();

      void UpdatePosition();

      void UpdateVelocity();
   };
}