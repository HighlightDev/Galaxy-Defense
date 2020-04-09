#include "PhysicsDescriptor.h"
#include "Core/UtilityCore/EngineMath.h"

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
      if (!EngineUtility::CMP::Process(mass, 0.0f))
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
      btScalar btMatrix[16];
      transform.getOpenGLMatrix(btMatrix);

      bIsWorldTransformDiry = memcmp(btMatrix, mPrevTransformMatrix, 16 * sizeof(float)) != 0;
      memcpy(mPrevTransformMatrix, btMatrix, 16 * sizeof(float));

      if (bIsWorldTransformDiry)
      {
         float yaw, pitch, roll;

         transform.getBasis().getEulerZYX(yaw, pitch, roll);
         mRotation = std::move(glm::vec3(yaw, pitch, roll));
         mTranslation = std::move(glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z()));
      }
   }


   float* PhysicsDescriptor::GetTrasformMatrix4x4()
   {
      return mPrevTransformMatrix;
   }

   glm::vec3 PhysicsDescriptor::GetRotation() const
   {
      return mRotation;
   }

   glm::vec3 PhysicsDescriptor::GetTranslation() const
   {
      return mTranslation;
   }
}