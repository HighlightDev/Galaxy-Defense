#include "PhysicsDescriptor.h"
#include "Core/UtilityCore/EngineMath.h"
#include <glm/gtx/matrix_decompose.hpp>

using namespace EngineMath;

namespace Game
{
   size_t PhysicsDescriptor::mTotalIds = 0;

   PhysicsDescriptor::PhysicsDescriptor(PhyShapeBase* shape, const float mass)
      : mCurrentId(PhysicsDescriptor::mTotalIds++)
      , mShape(shape)
      , mMotionState(new btDefaultMotionState())
      , mMass(mass)
      , mInertia()
      , mRigidBody(nullptr)
      , mRotator()
      , mTranslation()
      , mPrevTransform()
   {
      if (!CompareFloats(mass, 0.0f))
      {
         mShape->GetCollisionShape()->calculateLocalInertia(mass, mInertia);
      }
   }

   PhysicsDescriptor::~PhysicsDescriptor()
   {
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

   void PhysicsDescriptor::SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation)
   {
      btTransform worldTransform(quat, translation);
      mMotionState->setWorldTransform(worldTransform);
   }

   btTransform PhysicsDescriptor::GetMotionWorldTransform() const {
      btTransform resultTransform;
      mMotionState->getWorldTransform(resultTransform);
      return resultTransform;
   }

   btRigidBody* PhysicsDescriptor::GetRigidBody() const
   {
      return mRigidBody;
   }

   btMotionState* PhysicsDescriptor::GetMotionState() const
   {
      return mMotionState;
   }

   void PhysicsDescriptor::CompleteRigidBodyConstruction()
   {
      btRigidBody::btRigidBodyConstructionInfo info(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);
      mRigidBody = new btRigidBody(info);
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