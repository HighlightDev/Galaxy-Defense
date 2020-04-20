#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace Game
{

   struct PhyShapeBase
   {
   protected:

      btCollisionShape* mCollisionShape = nullptr;

   public:

      PhyShapeBase(btCollisionShape* shape)
         : mCollisionShape(shape)
      {
      }

      virtual ~PhyShapeBase()
      {
         delete mCollisionShape;
      }

      inline btCollisionShape* GetCollisionShape() const {

         return mCollisionShape;
      }
   };
}
