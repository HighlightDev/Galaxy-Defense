#include "RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EnginePhysics
{

   class IgnoreBodyCast :
      public btCollisionWorld::ClosestRayResultCallback
   {
   private:
      btRigidBody* m_pBody;

   public:
      IgnoreBodyCast(btRigidBody* pBody)
         : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
         m_pBody(pBody)
      {
      }

      btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
      {
         if (rayResult.m_collisionObject == m_pBody)
            return 1.0f;

         return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
      }
   };

   RigidBodyController::RigidBodyController(PhysicsWorld* pPhysicsWorld, PhysicsShapeBase* shape, const PhysicsBodyType bodyType, const float mass, const MotionModifiers& motionModifier)
      : PhysicsDescriptor(pPhysicsWorld, shape, bodyType, mass, motionModifier)
      , mLastRayCastObjectResult(nullptr)
   {
   }

   RigidBodyController::~RigidBodyController()
   {
      if (PhysicsBodyType::DYNAMIC == mBodyType)
         Event::KinematicBodyMovedEvent::GetInstance()->RemoveListener(this);
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
         case PhysicsBodyType::DYNAMIC: 
         {
            mRigidBody->setActivationState(DISABLE_DEACTIVATION); 
            Event::KinematicBodyMovedEvent::GetInstance()->AddListener(this);
            break;
         }
         case PhysicsBodyType::KINEMATIC: mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); break;
         case PhysicsBodyType::STATIC: assert(EngineMath::CompareFloats(mMass, 0.0f)); break;
      }

      mRigidBody->setUserPointer(static_cast<PhysicsDescriptor*>(this));

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);
   }

   bool RigidBodyController::DoRayCastDown()
   {
      auto rayCastResult = IgnoreBodyCast(mRigidBody);
      auto& worldTransform = mRigidBody->getWorldTransform();

      const btVector3& downDir = worldTransform.getOrigin() - btVector3(0, 5.0f, 0);

      mPhysicsWorld->GetWorld()->rayTest(worldTransform.getOrigin(), downDir, rayCastResult);

      const bool bResult = rayCastResult.hasHit();

      if (bResult)
      {
         if (auto collidedUserPtr = rayCastResult.m_collisionObject->getUserPointer())
         {
            mLastRayCastObjectResult = static_cast<PhysicsDescriptor*>(collidedUserPtr);
         }
      }

      return bResult;
   }

   void RigidBodyController::ProcessEvent(const Event::KinematicBodyMovedEvent::EventData_t& data)
   {
      PhysicsDescriptor* kinematicObjDesc = std::get<0>(data);
      const btVector3& offsetTranslation = Converter::glmToBullet(std::get<1>(data).Translation);

      if (DoRayCastDown())
      {
         if (mLastRayCastObjectResult == kinematicObjDesc)
         {
            // Collision
            auto& worldTransform = mRigidBody->getWorldTransform();
            const auto& offsetedTranslation = worldTransform.getOrigin() + offsetTranslation;
            worldTransform.setOrigin(offsetedTranslation);
         }
      }
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