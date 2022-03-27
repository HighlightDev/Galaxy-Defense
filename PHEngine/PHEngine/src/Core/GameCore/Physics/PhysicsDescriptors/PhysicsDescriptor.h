#pragma once

#include "Shapes/PhysicsShapeBase.h"

#include <BulletPhys/btBulletDynamicsCommon.h>

namespace EnginePhysics
{

   enum class PhysicsBodyType
   {
      STATIC,  // deactivates when velocity is 0
      DYNAMIC, // never deactivates
      KINEMATIC // deactivates + cannot be applied linear velocity to body
   };

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

      const PhysicsBodyType mBodyType;

      size_t mCurrentId;

      class PhysicsWorld* mPhysicsWorld;

      PhysicsShapeBase* mShape;

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

      PhysicsDescriptor(class PhysicsWorld* pPhysicsWorld, PhysicsShapeBase* shape, const PhysicsBodyType bodyType, const float mass = 0.0f, const MotionModifiers& motionModifier = MotionModifiers());

      virtual ~PhysicsDescriptor();

      virtual void CompletePhysicsDescriptorConstruction() = 0;

      virtual void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime) = 0;

      virtual void SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation) = 0;

      virtual void PostPhysicsSimulationUpdate();

      PhysicsShapeBase* GetShape() const;

      size_t GetId() const;

      float GetMass() const;

      PhysicsBodyType GetPhysicsBodyType() const;

      MotionModifiers GetMotionModifiers() const;

      void SetLinearVelocity(const btVector3& velocity);

      btRigidBody* GetRigidBody() const;

      btMotionState* GetMotionState() const;

      btVector3 GetTranslation() const;

      btQuaternion GetRotator() const;

      btVector3 GetVelocity() const;

      void SetTranslation(const btVector3& translation);

      void SetRotator(const btQuaternion& rotator);
   };

}
