#pragma once

#include "Shapes/PhyShapeBase.h"

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace Game
{

   struct PhysicsDescriptor
   {
   private:

      static size_t mTotalIds;

      size_t mCurrentId;

      PhyShapeBase* mShape;

      btMotionState* mMotionState;

      // default value is 0.0f which means that this physics body is completely STATIC
      float mMass;

      btVector3 mInertia;

      btRigidBody* mRigidBody;

      btQuaternion mRotator;

      btVector3 mTranslation;

      btTransform mPrevTransform;

   public:

      PhysicsDescriptor(PhyShapeBase* shape, const float mass = 0.0f);

      ~PhysicsDescriptor();

      PhyShapeBase* GetShape() const;

      size_t GetId() const;

      void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation);

      btRigidBody* GetRigidBody() const;

      btMotionState* GetMotionState() const;

      void CompleteRigidBodyConstruction();

      void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry);

      btVector3 GetTranslation() const;

      btQuaternion GetRotator() const;

   private:

      btTransform GetMotionWorldTransform() const;
   };

}
