#include "PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

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

   PhysicsDescriptor::PhysicsDescriptor(PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape, const PhysicsBodyType bodyType, const float mass, const MotionModifiers &motionModifier)
       : mBodyType(bodyType), mPhysicsWorld(pPhysicsWorld), mCurrentId(PhysicsDescriptor::mTotalIds++), mShape(shape), mMotionState(new btDefaultMotionState()), mMass(mass), mInertia(), mRigidBody(nullptr), mRotator(), mTranslation(), mVelocity(), mPrevTransform(), mMotionModifier(motionModifier)
   {
      if (!CompareFloats(mass, 0.0f))
      {
         mShape->GetCollisionShape()->calculateLocalInertia(mass, mInertia);
      }
   }

   PhysicsDescriptor::~PhysicsDescriptor()
   {
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

   float PhysicsDescriptor::GetMass() const
   {
      return mMass;
   }

   PhysicsBodyType PhysicsDescriptor::GetPhysicsBodyType() const
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
}