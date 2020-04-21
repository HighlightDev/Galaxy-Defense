#pragma once
#include "PhysicsDescriptor.h"

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

      virtual void setWalkDirection(const btVector3& walkDirection) { }
      virtual void setVelocityForTimeInterval(const btVector3& velocity, btScalar timeInterval) {}


      virtual void setUpInterpolate(bool value) { }

      void setup(btScalar height = 2.0, btScalar width = 0.25, btScalar stepHeight = 0.25);
      void destroy();


      virtual void reset(btCollisionWorld* collisionWorld);
      virtual void warp(const btVector3& origin);
      virtual void registerPairCacheAndDispatcher(btOverlappingPairCache* pairCache, btCollisionDispatcher* dispatcher);

      btCollisionObject* getCollisionObject();

      virtual void preStep(btCollisionWorld* collisionWorld);
      virtual void playerStep1(const btCollisionWorld* collisionWorld, btScalar dt,
         int forward,
         int backward,
         int left,
         int right,
         int jump);

      virtual void playerStep(btCollisionWorld* collisionWorld, btScalar dt) {}
      virtual bool canJump() const;
      virtual void jump(const btVector3& dir = btVector3(0, 0, 0));

      virtual bool onGround() const;

      // this method is invoking every time
      virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) {

      }

      virtual void debugDraw(btIDebugDraw* debugDrawer) {

      }
   };

}

