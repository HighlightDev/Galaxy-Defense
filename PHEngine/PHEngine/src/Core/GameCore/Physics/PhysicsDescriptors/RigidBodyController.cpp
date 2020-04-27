#include "RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"

namespace EnginePhysics
{
   RigidBodyController::RigidBodyController(PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const float mass, const MotionModifiers& motionModifier)
      : PhysicsDescriptor(pPhysicsWorld, shape, mass, motionModifier)
   {
   }

   RigidBodyController::~RigidBodyController()
   {
   }

   void RigidBodyController::SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation)
   {
      btTransform worldTransform(quat, translation);
      mMotionState->setWorldTransform(worldTransform);
   }

   void RigidBodyController::CompleteRigidBodyConstruction()
   {
      btRigidBody::btRigidBodyConstructionInfo info(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);
      mRigidBody = new btRigidBody(info);

      // apply motion modifiers
      mRigidBody->setLinearFactor(mMotionModifier.LinearFactor);
      mRigidBody->setAngularFactor(mMotionModifier.AngularFactor);

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);
   }

   void RigidBodyController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry)
   {
      btTransform transform;
      mMotionState->getWorldTransform(transform);

      if (bIsWorldTransformDiry = !(isEqual(transform, mPrevTransform)))
      {
         mPrevTransform = transform;

         mRotator = transform.getRotation();
         mTranslation = transform.getOrigin();
         mVelocity = mRigidBody->getLinearVelocity();
      }
   }
}