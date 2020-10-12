#include "RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EnginePhysics
{
   RigidBodyController::RigidBodyController(PhysicsWorld* pPhysicsWorld, PhyShapeBase* shape, const PhysicsBodyType bodyType, const float mass, const MotionModifiers& motionModifier)
      : PhysicsDescriptor(pPhysicsWorld, shape, bodyType, mass, motionModifier)
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

      switch (mBodyType)
      {
         case PhysicsBodyType::DYNAMIC: mRigidBody->setActivationState(DISABLE_DEACTIVATION); break;
         case PhysicsBodyType::KINEMATIC: mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); break;
         case PhysicsBodyType::STATIC: assert(EngineMath::CompareFloats(mMass, 0.0f)); break;
      }

      mRigidBody->setUserPointer(static_cast<PhysicsDescriptor*>(this));

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);
   }

   void RigidBodyController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime)
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