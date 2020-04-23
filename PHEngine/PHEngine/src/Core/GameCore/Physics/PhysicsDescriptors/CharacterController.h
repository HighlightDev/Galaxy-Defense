#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace Game
{

   class DynamicCharacterController
   {
   private:
      // Physics
      btDiscreteDynamicsWorld* m_pPhysicsWorld;

      btCollisionShape* m_pCollisionShape;
      btDefaultMotionState* m_pMotionState;
      btRigidBody* m_pRigidBody;
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

      DynamicCharacterController(btDiscreteDynamicsWorld* pPhysicsWorld, const btVector3 spawnPos, float radius, float height, float mass, float stepHeight);
      ~DynamicCharacterController();

      // Acceleration vector in XZ plane
      void Walk(const glm::vec2& dir);

      // Ignores y
      void Walk(const glm::vec3& dir);

      void Update();

      void Jump();

      btVector3 GetPosition() const;
      btVector3 GetVelocity() const;

      bool IsOnGround() const;
   };
}