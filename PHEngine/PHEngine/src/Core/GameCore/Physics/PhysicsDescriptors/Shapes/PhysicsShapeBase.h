#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace EnginePhysics
{

   struct PhysicsShapeBase
   {
   protected:

      btCollisionShape* mCollisionShape = nullptr;

   public:

      PhysicsShapeBase(btCollisionShape* shape)
         : mCollisionShape(shape)
      {
      }

      virtual ~PhysicsShapeBase()
      {
         delete mCollisionShape;
      }

      inline btCollisionShape* GetCollisionShape() const {

         return mCollisionShape;
      }
   };
}
