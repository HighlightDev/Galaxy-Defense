#include "RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/BulletRayCastWithFilter.h"

namespace EnginePhysics
{
   RigidBodyController::RigidBodyController(PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape, const ePhysicsBodyType bodyType, const float mass, const MotionModifiers &motionModifier)
       : PhysicsDescriptor(pPhysicsWorld, shape, bodyType, mass, motionModifier), mLastRayCastObjectResult(nullptr)
   {
   }

   RigidBodyController::~RigidBodyController()
   {
      if (ePhysicsBodyType::DYNAMIC == mBodyType)
         Event::KinematicBodyMovedEvent::GetInstance()->RemoveListener(this);
   }

   ePhysicsDescriptorType RigidBodyController::GetPhysicsDescriptorType() const
   {
      return ePhysicsDescriptorType::RIGID_BODY_CONTROLLER;
   }

   std::vector<btCollisionObject*> RigidBodyController::GetCollisionObjects() const
   {
      return {mRigidBody};
   }

   void RigidBodyController::SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation)
   {
      btTransform worldTransform(quat, translation);
      mMotionState->setWorldTransform(worldTransform);
   }

   void RigidBodyController::CompletePhysicsDescriptorConstruction()
   {
      btRigidBody::btRigidBodyConstructionInfo info(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);
      mRigidBody = new btRigidBody(info);

      // apply motion modifiers
      mRigidBody->setLinearFactor(mMotionModifier.LinearFactor);
      mRigidBody->setAngularFactor(mMotionModifier.AngularFactor);

      switch (mBodyType)
      {
      case ePhysicsBodyType::DYNAMIC:
      {
         mRigidBody->setActivationState(DISABLE_DEACTIVATION);
         Event::KinematicBodyMovedEvent::GetInstance()->AddListener(this);
         break;
      }
      case ePhysicsBodyType::KINEMATIC:
         mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
         break;
      case ePhysicsBodyType::STATIC:
         assert(EngineMath::FloatsNearEqual(mMass, 0.0f));
         break;
      }

      mRigidBody->setUserPointer(static_cast<PhysicsDescriptor *>(this));

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);
   }

   bool RigidBodyController::DoRayCastDown()
   {
      auto ignoreMeRayCast = BulletRayCastWithFilter({mRigidBody});
      auto &worldTransform = mRigidBody->getWorldTransform();

      const btVector3 &downDir = worldTransform.getOrigin() - btVector3(0, 5.0f, 0);

      ignoreMeRayCast.RayTest(mPhysicsWorld->GetWorld(), worldTransform.getOrigin(), downDir);

      const bool bResult = ignoreMeRayCast.IsRayHitCollision();

      if (bResult)
      {
         if (auto collidedUserPtr = ignoreMeRayCast.GetCollisionHitObject()->getUserPointer())
         {
            mLastRayCastObjectResult = static_cast<PhysicsDescriptor *>(collidedUserPtr);
         }
      }

      return bResult;
   }

   void RigidBodyController::ProcessEvent(const Event::KinematicBodyMovedEvent::EventData_t &data)
   {
      PhysicsDescriptor *kinematicObjDesc = std::get<0>(data);
      const btVector3 &offsetTranslation = Converter::glmToBullet(std::get<1>(data).Translation);

      if (DoRayCastDown())
      {
         if (mLastRayCastObjectResult == kinematicObjDesc)
         {
            // Collision
            auto &worldTransform = mRigidBody->getWorldTransform();
            const auto &offsetedTranslation = worldTransform.getOrigin() + offsetTranslation;
            worldTransform.setOrigin(offsetedTranslation);
         }
      }
   }

   void RigidBodyController::UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime)
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