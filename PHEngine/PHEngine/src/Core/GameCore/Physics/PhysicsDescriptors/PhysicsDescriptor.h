#pragma once

#include "Shapes/CollisionShapeBase.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"

#include <vector>
#include <memory>
#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace EnginePhysics
{
   struct MotionModifiers
   {
      btVector3 LinearFactor;
      btVector3 AngularFactor;

      MotionModifiers();

      MotionModifiers(const btVector3 &linearFactor, const btVector3 &angularFactor);
   };

   class PhysicsDescriptor
       : public std::enable_shared_from_this<PhysicsDescriptor>
   {
   protected:
      static int32_t mTotalIds;

      const ePhysicsBodyType mBodyType;

      int32_t mCurrentId;

      int32_t mOwnerComponentEngineObjectId;

      int32_t mOwnerActorEngineObjectId;

      class PhysicsWorld *mPhysicsWorld;

      std::shared_ptr<CollisionShapeBase> mShape;

      btMotionState *mMotionState;

      // default value is 0.0f which means that this physics body is completely STATIC
      float mMass;

      btVector3 mInertia;

      btRigidBody *mRigidBody;

      btQuaternion mRotator;

      btVector3 mTranslation;

      btVector3 mVelocity;

      btTransform mPrevTransform;

      MotionModifiers mMotionModifier;

      bool mIsCollisionEnabled;

   public:
      PhysicsDescriptor(class PhysicsWorld *pPhysicsWorld,
                        const std::shared_ptr<CollisionShapeBase> &shape,
                        const ePhysicsBodyType bodyType,
                        const float mass = 0.0f,
                        const MotionModifiers &motionModifier = MotionModifiers());

      virtual ~PhysicsDescriptor();

      virtual void CompletePhysicsDescriptorConstruction() = 0;

      virtual void UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime) = 0;

      virtual void SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation) = 0;

      virtual void PostPhysicsSimulationUpdate(const float deltaTime);

      virtual ePhysicsDescriptorType GetPhysicsDescriptorType() const = 0;

      virtual std::vector<btCollisionObject *> GetCollisionObjects() const;

      const std::shared_ptr<CollisionShapeBase> &GetShape() const;

      size_t GetId() const;

      float GetMass() const;

      ePhysicsBodyType GetPhysicsBodyType() const;

      MotionModifiers GetMotionModifiers() const;

      void SetLinearVelocity(const btVector3 &velocity);

      btRigidBody *GetRigidBody() const;

      btMotionState *GetMotionState() const;

      btVector3 GetTranslation() const;

      btQuaternion GetRotator() const;

      btVector3 GetVelocity() const;

      void SetTranslation(const btVector3 &translation);

      void SetRotator(const btQuaternion &rotator);

      virtual void SetIsCollisionEnabled(const bool isEnabled);

      bool GetIsCollisionEnabled() const;

      void SetOwnerComponentEngineObjectId(const int32_t ownerComponentEngineObjectId);

      int32_t GetOwnerComponentEngineObjectId() const;

      void SetOwnerActorEngineObjectId(const int32_t ownerActorEngineObjectId);

      int32_t GetOwnerActorEngineObjectId() const;
   };

}
