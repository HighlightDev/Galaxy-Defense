#include "PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"
#include "Core/GameCore/LoggerExtension.h"

#include <limits>

using namespace EngineMath;
using namespace EngineCore;

namespace EnginePhysics
{
   MotionModifiers::MotionModifiers()
       : LinearFactor(btVector3(1.0f, 1.0f, 1.0f)), AngularFactor(btVector3(1.0f, 1.0f, 1.0f))
   {
   }

   MotionModifiers::MotionModifiers(const btVector3 &linearFactor, const btVector3 &angularFactor)
       : LinearFactor(linearFactor), AngularFactor(angularFactor)
   {
   }

   size_t PhysicsDescriptor::mTotalIds = 0;

   PhysicsDescriptor::PhysicsDescriptor(PhysicsWorld *pPhysicsWorld,
                                        PhysicsShapeBase *shape,
                                        const ePhysicsBodyType bodyType,
                                        const float mass,
                                        const MotionModifiers &motionModifier)
       : mBodyType(bodyType),
         mPhysicsWorld(pPhysicsWorld),
         mCurrentId(PhysicsDescriptor::mTotalIds++),
         mOwnerComponentGameObjectId(std::numeric_limits<uint64_t>::max()),
         mOwnerActorGameObjectId(std::numeric_limits<uint64_t>::max()),
         mShape(shape),
         mMotionState(new btDefaultMotionState()),
         mMass(mass),
         mInertia(),
         mRigidBody(nullptr),
         mRotator(),
         mTranslation(),
         mVelocity(),
         mPrevTransform(),
         mMotionModifier(motionModifier),
         mIsCollisionEnabled(true)
   {
      LogInfo("PhysicsDescriptor::ctor => my descriptor id=", mCurrentId);

      if (!FloatsNearEqual(mass, 0.0f))
      {
         mShape->GetCollisionShape()->calculateLocalInertia(mass, mInertia);
      }
   }

   PhysicsDescriptor::~PhysicsDescriptor()
   {
      LogInfo("PhysicsDescriptor::dctor => my descriptor id=", mCurrentId);

      if (mRigidBody)
      {
         mPhysicsWorld->GetWorld()->removeCollisionObject(mRigidBody);
         delete mRigidBody;
      }

      delete mShape;
      delete mMotionState;
   }

   void PhysicsDescriptor::PostPhysicsSimulationUpdate(const float deltaTime)
   {
   }

   std::vector<btCollisionObject*> PhysicsDescriptor::GetCollisionObjects() const
   {
      return {mRigidBody};
   }

   float PhysicsDescriptor::GetMass() const
   {
      return mMass;
   }

   ePhysicsBodyType PhysicsDescriptor::GetPhysicsBodyType() const
   {
      return mBodyType;
   }

   MotionModifiers PhysicsDescriptor::GetMotionModifiers() const
   {
      return mMotionModifier;
   }

   PhysicsShapeBase *PhysicsDescriptor::GetShape() const
   {
      return mShape;
   }

   size_t PhysicsDescriptor::GetId() const
   {
      return mCurrentId;
   }

   btRigidBody *PhysicsDescriptor::GetRigidBody() const
   {
      return mRigidBody;
   }

   btMotionState *PhysicsDescriptor::GetMotionState() const
   {
      return mMotionState;
   }

   void PhysicsDescriptor::SetLinearVelocity(const btVector3 &velocity)
   {
      if (mRigidBody)
      {
         mRigidBody->setLinearVelocity(velocity);
      }
   }

   btQuaternion PhysicsDescriptor::GetRotator() const
   {
      return mRotator;
   }

   btVector3 PhysicsDescriptor::GetTranslation() const
   {
      return mTranslation;
   }

   btVector3 PhysicsDescriptor::GetVelocity() const
   {

      return mVelocity;
   }

   void PhysicsDescriptor::SetTranslation(const btVector3 &translation)
   {
      assert(mRigidBody);
      btTransform &worldTransform = mRigidBody->getWorldTransform();
      worldTransform.setOrigin(translation);
   }

   void PhysicsDescriptor::SetRotator(const btQuaternion &rotator)
   {
      assert(mRigidBody);
      btTransform &worldTransform = mRigidBody->getWorldTransform();
      worldTransform.setRotation(rotator);
   }

   void PhysicsDescriptor::SetIsCollisionEnabled(const bool isCollisionEnabled)
   {
      assert(mRigidBody);
      if (mIsCollisionEnabled != isCollisionEnabled)
      {
         if (isCollisionEnabled)
         {
            mPhysicsWorld->GetWorld()->addCollisionObject(mRigidBody);
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
         }
         else
         {
            mPhysicsWorld->GetWorld()->removeCollisionObject(mRigidBody);
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
         }
         mIsCollisionEnabled = isCollisionEnabled;
      }
   }

   bool PhysicsDescriptor::GetIsCollisionEnabled() const
   {
      return mIsCollisionEnabled;
   }

   void PhysicsDescriptor::SetOwnerComponentGameObjectId(const uint64_t ownerComponentGameObjectId)
   {
      LogInfo("PhysicsDescriptor::SetOwnerComponentGameObjectId => my descriptor id=", mCurrentId, "owner id=", ownerComponentGameObjectId);
      mOwnerComponentGameObjectId = ownerComponentGameObjectId;
   }

   uint64_t PhysicsDescriptor::GetOwnerComponentGameObjectId() const
   {
      return mOwnerComponentGameObjectId;
   }

   void PhysicsDescriptor::SetOwnerActorGameObjectId(const uint64_t ownerActorGameObjectId)
   {
      LogInfo("PhysicsDescriptor::SetOwnerActorGameObjectId => my descriptor id=", mCurrentId, "owner id=", ownerActorGameObjectId);
      mOwnerActorGameObjectId = ownerActorGameObjectId;
   }

   uint64_t PhysicsDescriptor::GetOwnerActorGameObjectId() const
   {
      return mOwnerActorGameObjectId;
   }
}