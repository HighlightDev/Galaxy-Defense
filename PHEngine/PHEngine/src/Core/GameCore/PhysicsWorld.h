#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace Game
{

   class PhysicsWorld
   {
   public:
      PhysicsWorld();
      ~PhysicsWorld();

      btBroadphaseInterface* mBroadphase;
      btDefaultCollisionConfiguration*        mCollisionConfiguration;
      btCollisionDispatcher*                  mDispatcher;
      btSequentialImpulseConstraintSolver*    mSolver;
      btDiscreteDynamicsWorld*                mWorld;
   };

}

