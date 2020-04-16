#include "PhysicsDescriptor.h"
#include "Core/UtilityCore/EngineMath.h"
#include <glm/gtx/matrix_decompose.hpp>

using namespace EngineMath;

namespace Game
{
   size_t PhysicsDescriptor::mTotalIds = 0;

   PhysicsDescriptor::PhysicsDescriptor(PhyShapeBase* shape, const float mass)
      : mShape(shape)
      , mMotionState(new btDefaultMotionState())
      , mMass(mass)
      , mInertia()
      , mRigidBody(nullptr)
      , mCurrentId(PhysicsDescriptor::mTotalIds++)
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

   void PhysicsDescriptor::SetMotionStateWorldTransform(const float yaw, const float pitch, const float roll, const glm::vec3& translation)
   {
      btTransform worldTransform(btQuaternion(btScalar(yaw), btScalar(pitch), btScalar(roll)), btVector3(translation.x, translation.y, translation.z));
      mMotionState->setWorldTransform(worldTransform);
   }

   void PhysicsDescriptor::SetMotionStateWorldTransform(const btQuaternion& quat, const glm::vec3& translation)
   {
      btTransform worldTransform(quat, btVector3(translation.x, translation.y, translation.z));
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

      auto translation = transform.getOrigin();
      auto rotator = transform.getRotation();

      if (
         CompareFloats(rotator.getX(), mRotator.getX()) &&
         CompareFloats(rotator.getY(), mRotator.getY()) &&
         CompareFloats(rotator.getZ(), mRotator.getZ()) && 
         CompareFloats(rotator.getW(), mRotator.getW()) &&
         CompareFloats(translation.getX(), mTranslation.x()) &&
         CompareFloats(translation.getY(), mTranslation.y()) &&
         CompareFloats(translation.getZ(), mTranslation.z()))
      {
         bIsWorldTransformDiry = false;
      }
      else
      {
         bIsWorldTransformDiry = true;

         mRotator = rotator;
         mTranslation = translation;
      }
   }

   btQuaternion PhysicsDescriptor::GetRotator()  const
   {
      return mRotator;
   }

   glm::vec3 PhysicsDescriptor::GetTranslation() const
   {
      return glm::vec3(mTranslation.getX(), mTranslation.getY(), mTranslation.getZ());
   }
}