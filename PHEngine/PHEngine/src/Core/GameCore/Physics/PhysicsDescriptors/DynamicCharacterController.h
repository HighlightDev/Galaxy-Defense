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
      btPairCachingGhostObject* m_pGhostObject;

      bool m_onGround;
      //bool m_onJumpableGround; // A bit lower contact than just onGround
      bool m_hittingWall;

      float m_bottomYOffset;
      float m_bottomRoundedRegionYOffset;

      float m_stepHeight;

      const float timerMultiplier = 0.15f;

      btTransform m_motionTransform;

      btVector3 m_manualVelocity;
      std::vector<btVector3> m_surfaceHitNormals;

      btVector3 m_previousPosition;

      float m_jumpRechargeTimer;

      void ParseGhostContacts();

      void UpdatePosition();
      void UpdateVelocity();

   public:

      float m_deceleration;
      float m_maxSpeed;
      float m_jumpImpulse;

      float m_jumpRechargeTime;

      DynamicCharacterController(class PhysicsWorld* pPhysicsWorld, float capsuleRadius, float capsuleHeight, float mass, float stepHeight);

      virtual ~DynamicCharacterController();

      virtual void CompleteRigidBodyConstruction() override;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry) override;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) override;

      // Acceleration vector in XZ plane
      void Walk(const glm::vec2& dir);

      // Ignores y
      void Walk(const glm::vec3& dir);

      void Jump();

      bool IsOnGround() const;
   };
}