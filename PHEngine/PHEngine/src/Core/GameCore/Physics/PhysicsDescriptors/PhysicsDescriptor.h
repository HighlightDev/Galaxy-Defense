#pragma once

#include "Shapes/PhyShapeBase.h"

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace EnginePhysics
{

   struct MotionModifiers
   {
      btVector3 LinearFactor;
      btVector3 AngularFactor;

      MotionModifiers();

      MotionModifiers(const btVector3& linearFactor, const btVector3& angularFactor);
   };

   class PhysicsDescriptor
   {
   protected:

      static size_t mTotalIds;

      size_t mCurrentId;

      class PhysicsWorld* mPhysicsWorld;

      PhyShapeBase* mShape;

      btMotionState* mMotionState;

      // default value is 0.0f which means that this physics body is completely STATIC
      float mMass;

      btVector3 mInertia;

      btRigidBody* mRigidBody;

      btQuaternion mRotator;

      btVector3 mTranslation;

      btVector3 mVelocity;

      btTransform mPrevTransform;
      
      MotionModifiers mMotionModifier;

   public:

      PhysicsDescriptor(class PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const float mass = 0.0f, const MotionModifiers& motionModifier = MotionModifiers());

      virtual ~PhysicsDescriptor();

      virtual void CompleteRigidBodyConstruction() = 0;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry) = 0;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) = 0;

      PhyShapeBase* GetShape() const;

      size_t GetId() const;

      void SetLinearVelocity(const btVector3& velocity);

      btRigidBody* GetRigidBody() const;

      btMotionState* GetMotionState() const;

      btVector3 GetTranslation() const;

      btQuaternion GetRotator() const;

      btVector3 GetVelocity() const;

   };

}
