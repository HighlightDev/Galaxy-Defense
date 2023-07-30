#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace EnginePhysics
{

   struct CollisionShapeBase
   {
   protected:

      btCollisionShape* mCollisionShape = nullptr;

   public:

      CollisionShapeBase(btCollisionShape* shape)
         : mCollisionShape(shape)
      {
      }

      virtual ~CollisionShapeBase()
      {
         delete mCollisionShape;
      }

      inline btCollisionShape* GetCollisionShape() const {

         return mCollisionShape;
      }
   };
}
