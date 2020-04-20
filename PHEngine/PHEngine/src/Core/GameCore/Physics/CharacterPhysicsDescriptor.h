#pragma once
#include "PhysicsDescriptors/PhysicsDescriptor.h"

#include <BulletPhys/LinearMath/btVector3.h>
#include <BulletPhys/BulletDynamics/Character/btCharacterControllerInterface.h>

class btCollisionShape;
class btRigidBody;
class btCollisionWorld;

namespace Game
{

   class CharacterPhysicsDescriptor
      : public PhysicsDescriptor
      , public btCharacterControllerInterface

   {
   public:

      CharacterPhysicsDescriptor();
      virtual ~CharacterPhysicsDescriptor();


   protected:
      btScalar m_halfHeight;
      btCollisionShape* m_shape;
      btRigidBody* m_rigidBody;

      btVector3 m_raySource[2];
      btVector3 m_rayTarget[2];
      btScalar m_rayLambda[2];
      btVector3 m_rayNormal[2];

      btScalar m_turnAngle;

      btScalar m_maxLinearVelocity;
      btScalar m_walkVelocity;
      btScalar m_turnVelocity;
   public:

      void setup(btScalar height = 2.0, btScalar width = 0.25, btScalar stepHeight = 0.25);
      void destroy();

      virtual void reset();
      virtual void warp(const btVector3& origin);
      virtual void registerPairCacheAndDispatcher(btOverlappingPairCache* pairCache, btCollisionDispatcher* dispatcher);

      btCollisionObject* getCollisionObject();

      void preStep(const btCollisionWorld* collisionWorld);
      void playerStep(const btCollisionWorld* collisionWorld, btScalar dt,
         int forward,
         int backward,
         int left,
         int right,
         int jump);
      bool canJump() const;
      void jump();

      bool onGround() const;
   };

}

