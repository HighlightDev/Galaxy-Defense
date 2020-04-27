#include "PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EnginePhysics
{
   MotionModifiers::MotionModifiers()
      : LinearFactor(btVector3(1.0f, 1.0f, 1.0f))
      , AngularFactor(btVector3(1.0f, 1.0f, 1.0f))
   {
   }

   MotionModifiers::MotionModifiers(const btVector3& linearFactor, const btVector3& angularFactor)
      : LinearFactor(linearFactor)
      , AngularFactor(angularFactor)
   {
   }

   size_t PhysicsDescriptor::mTotalIds = 0;

   PhysicsDescriptor::PhysicsDescriptor(PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const float mass, const MotionModifiers& motionModifier)
      : mPhysicsWorld(pPhysicsWorld)
      , mCurrentId(PhysicsDescriptor::mTotalIds++)
      , mShape(shape)
      , mMotionState(new btDefaultMotionState())
      , mMass(mass)
      , mInertia()
      , mRigidBody(nullptr)
      , mRotator()
      , mTranslation()
      , mPrevTransform()
      , mMotionModifier(motionModifier)
   {
      if (!CompareFloats(mass, 0.0f))
      {
         mShape->GetCollisionShape()->calculateLocalInertia(mass, mInertia);
      }
   }

   PhysicsDescriptor::~PhysicsDescriptor()
   {
      mPhysicsWorld->GetWorld()->removeCollisionObject(GetRigidBody());

      delete mShape;
      delete mMotionState;
      delete mRigidBody;
   }

   PhyShapeBase* PhysicsDescriptor::GetShape() const {
      return mShape;
   }

   size_t PhysicsDescriptor::GetId() const {
      return mCurrentId;
   }

   btRigidBody* PhysicsDescriptor::GetRigidBody() const
   {
      return mRigidBody;
   }

   btMotionState* PhysicsDescriptor::GetMotionState() const
   {
      return mMotionState;
   }

   void PhysicsDescriptor::SetLinearVelocity(const btVector3& velocity)
   {
      if (mRigidBody)
      {
         mRigidBody->setLinearVelocity(velocity);
      }
   }

   void PhysicsDescriptor::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry)
   {
      btTransform transform;
      mMotionState->getWorldTransform(transform);

      bIsWorldTransformDiry = false;

      if (!(mPrevTransform == transform))
      {
         mPrevTransform = transform;
         bIsWorldTransformDiry = true;

         mRotator = transform.getRotation();
         mTranslation = transform.getOrigin();
      }
   }

   btQuaternion PhysicsDescriptor::GetRotator()  const
   {
      return mRotator;
   }

   btVector3 PhysicsDescriptor::GetTranslation() const
   {
      return mTranslation;
   }
}