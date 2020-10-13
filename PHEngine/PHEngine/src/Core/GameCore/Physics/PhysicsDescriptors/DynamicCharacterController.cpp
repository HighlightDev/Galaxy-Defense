#include "DynamicCharacterController.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Components/Transform.h"

#include <glm/gtx/projection.hpp>

#include <iostream>

using namespace Converter;

namespace EnginePhysics
{

   class IgnoreBodyAndGhostCast :
      public btCollisionWorld::ClosestRayResultCallback
   {
   private:
      btRigidBody* m_pBody;
      btPairCachingGhostObject* mGhostObject;

   public:
      IgnoreBodyAndGhostCast(btRigidBody* pBody, btPairCachingGhostObject* pGhostObject)
         : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
         m_pBody(pBody), mGhostObject(pGhostObject)
      {
      }

      btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
      {
         if (rayResult.m_collisionObject == m_pBody || rayResult.m_collisionObject == mGhostObject)
            return 1.0f;

         return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
      }
   };

   DynamicCharacterController::DynamicCharacterController(
      PhysicsWorld* pPhysicsWorld, float radius, float height, float mass, float stepHeight)
      : PhysicsDescriptor(pPhysicsWorld, new PhyCapsuleShape(radius, height), PhysicsBodyType::DYNAMIC, mass)
      , mGhostObject(nullptr)
      , mOnGround(false)
      , mHittingWall(false)
      , mDeceleration(0.1f)
      , mMaxSpeed(15.0f)
      , mJumpImpulse(150)
      , mJumpRechargeTime(10.0f)
      , mJumpRechargeTimer(0.0f)
      , mBottomYOffset(height / 3.0f + radius)
      , mBottomRoundedRegionYOffset((height + radius) / 3.0f)
      , mStepHeight(stepHeight)
      , mTimerMultiplier(0.0f)
      , mMotionTransform()
      , mPreviousPosition()
      , mManualVelocity(0.0f, 0.0f, 0.0f)
      , mSurfaceHitNormals()
      , mLastRayCastObjectResult(nullptr)
   {
      KinematicBodyMovedEvent::GetInstance()->AddListener(this);
   }

   DynamicCharacterController::~DynamicCharacterController()
   {
      KinematicBodyMovedEvent::GetInstance()->RemoveListener(this);

      mPhysicsWorld->GetWorld()->removeRigidBody(mRigidBody);
      mPhysicsWorld->GetWorld()->removeCollisionObject(mGhostObject);

      delete mGhostObject;
   }

   void DynamicCharacterController::SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation)
   {
      btTransform worldTransform(btQuaternion(1.0f, 0.0f, 0.0f, 0.0f), translation);
      mMotionState->setWorldTransform(worldTransform);
   }

   void DynamicCharacterController::CompleteRigidBodyConstruction() 
   {
      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);

      // No friction, this is done manually
      rigidBodyCI.m_friction = 0.0f;
      //rigidBodyCI.m_additionalDamping = true;
      //rigidBodyCI.m_additionalLinearDampingThresholdSqr= 1.0f;
      //rigidBodyCI.m_additionalLinearDampingThresholdSqr = 0.5f;
      rigidBodyCI.m_restitution = 0.0f;

      rigidBodyCI.m_linearDamping = 0.0f;

      mRigidBody = new btRigidBody(rigidBodyCI);

      // Keep upright
      mRigidBody->setAngularFactor(0.0f);
      mRigidBody->setUserPointer(static_cast<PhysicsDescriptor*>(this));

      // No sleeping (or else setLinearVelocity won't work)
      mRigidBody->setActivationState(DISABLE_DEACTIVATION);

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);

      // Ghost object that is synchronized with rigid body
      mGhostObject = new btPairCachingGhostObject();

      mGhostObject->setCollisionShape(mShape->GetCollisionShape());
      mGhostObject->setUserPointer(static_cast<PhysicsDescriptor*>(this));
      mGhostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

     

      // Specify filters manually, otherwise ghost doesn't collide with statics for some reason
   }

   void DynamicCharacterController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTime)
   {
      mTimerMultiplier = deltaTime * 1000; // from ms to sec

      // Sync ghost with actually object
      mGhostObject->setWorldTransform(mRigidBody->getWorldTransform());
      // Update transform
      mMotionState->getWorldTransform(mMotionTransform);

      mOnGround = false;

      ParseGhostContacts();

      UpdatePosition();
      UpdateVelocity();

      // Update jump timer
      if (mJumpRechargeTimer < mJumpRechargeTime)
         mJumpRechargeTimer += mTimerMultiplier;

      if (bIsWorldTransformDiry = !(isEqual(mMotionTransform, mPrevTransform)))
      {
         // Update data
         mPrevTransform = mMotionTransform;
         mTranslation = mMotionTransform.getOrigin();
         mVelocity = mRigidBody->getLinearVelocity();
      }
   }

   void DynamicCharacterController::Walk(const glm::vec2& dir)
   {
      glm::vec2 velocityXZ(dir + glm::vec2(mManualVelocity.getX(), mManualVelocity.getZ()));

      // Prevent from going over maximum speed
      float speedXZ = glm::length(velocityXZ);

      if (speedXZ > mMaxSpeed)
         velocityXZ = velocityXZ / speedXZ * mMaxSpeed;

      mManualVelocity.setX(velocityXZ.x);
      mManualVelocity.setZ(velocityXZ.y);
   }

   void DynamicCharacterController::Walk(const glm::vec3& dir)
   {
      Walk(glm::vec2(dir.x, dir.z));
   }

   void DynamicCharacterController::ParseGhostContacts()
   {
      btManifoldArray manifoldArray;
      btBroadphasePairArray &pairArray = mGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
      int numPairs = pairArray.size();

      // Set false now, may be set true in test
      mHittingWall = false;

      mSurfaceHitNormals.clear();

      for (int i = 0; i < numPairs; i++)
      {
         manifoldArray.clear();

         const btBroadphasePair &pair = pairArray[i];

         btBroadphasePair* collisionPair = mPhysicsWorld->GetWorld()->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

         if (collisionPair == NULL)
            continue;

         if (collisionPair->m_algorithm != NULL)
            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

         for (int j = 0; j < manifoldArray.size(); j++)
         {
            btPersistentManifold* pManifold = manifoldArray[j];

            // Skip the rigid body the ghost monitors
            if (pManifold->getBody0() == mRigidBody)
               continue;

            for (int p = 0; p < pManifold->getNumContacts(); p++)
            {
               const btManifoldPoint &point = pManifold->getContactPoint(p);

               if (point.getDistance() < 0.0f)
               {
                  //const btVector3 &ptA = point.getPositionWorldOnA();
                  const btVector3 &ptB = point.getPositionWorldOnB();

                  //const btVector3 &normalOnB = point.m_normalWorldOnB;

                  // If point is in rounded bottom region of capsule shape, it is on the ground
                  if (ptB.getY() < mMotionTransform.getOrigin().getY() - mBottomRoundedRegionYOffset)
                     mOnGround = true;
                  else
                  {
                     mHittingWall = true;

                     mSurfaceHitNormals.push_back(point.m_normalWorldOnB);
                  }
               }
            }
         }
      }
   }

   void DynamicCharacterController::UpdateVelocity()
   {
      // Adjust only xz velocity
      mManualVelocity.setY(mRigidBody->getLinearVelocity().getY());

      mRigidBody->setLinearVelocity(mManualVelocity);

      // Decelerate
      mManualVelocity -= mManualVelocity * mDeceleration * mTimerMultiplier;

      if (mHittingWall)
      {
         for (unsigned int i = 0, size = mSurfaceHitNormals.size(); i < size; i++)
         {
            // Cancel velocity across normal
            glm::vec3 surfaceNormal = Converter::bulletToGlm(mSurfaceHitNormals[i]);
            glm::vec3 velocity = Converter::bulletToGlm(mManualVelocity);
            glm::vec3 projection = glm::proj(velocity, surfaceNormal);

            btVector3 velInNormalDir(Converter::glmToBullet(projection));

            // Apply correction
            mManualVelocity -= velInNormalDir * 1.05f;
         }

         // Do not adjust rigid body velocity manually (so bodies can still be pushed by character)
         return;
      }
   }

   void DynamicCharacterController::ProcessEvent(const Event::KinematicBodyMovedEvent::EventData_t& data)
   {
      PhysicsDescriptor* kinematicObjDesc = std::get<0>(data);
      const btVector3& offsetTranslation = Converter::glmToBullet(std::get<1>(data).Translation);
      
      if (mLastRayCastObjectResult && kinematicObjDesc == mLastRayCastObjectResult)
      {
         // Collision
         auto& worldTransform = mRigidBody->getWorldTransform();
         const auto& offsetedTranslation = worldTransform.getOrigin() + offsetTranslation;
         worldTransform.setOrigin(offsetedTranslation);
      }
   }

   void DynamicCharacterController::UpdatePosition()
   {
      // Ray cast, ignore rigid body
      
      auto rayCastResult = IgnoreBodyAndGhostCast(mRigidBody, mGhostObject);
      auto& worldTransform = mRigidBody->getWorldTransform();

      mPhysicsWorld->GetWorld()->rayTest(worldTransform.getOrigin(),
         worldTransform.getOrigin() - btVector3(0.0f, mBottomYOffset + mStepHeight, 0.0f), rayCastResult);

      // Bump up if hit
      if (rayCastResult.hasHit())
      {
         if (auto collidedUserPtr = rayCastResult.m_collisionObject->getUserPointer())
         {
            mLastRayCastObjectResult = static_cast<PhysicsDescriptor*>(collidedUserPtr);
         }

         float previousY = worldTransform.getOrigin().getY();

         worldTransform.getOrigin().setY(previousY + (mBottomYOffset + mStepHeight) * (1.0f - rayCastResult.m_closestHitFraction));

         btVector3 vel(mRigidBody->getLinearVelocity());

         vel.setY(0.0f);

         mRigidBody->setLinearVelocity(vel);

         mOnGround = true;
      }

      float testOffset = 0.07f;

      // Ray cast, ignore rigid body
      IgnoreBodyAndGhostCast rayCallBack_top(mRigidBody, mGhostObject);

      mPhysicsWorld->GetWorld()->rayTest(worldTransform.getOrigin(), worldTransform.getOrigin() + btVector3(0.0f, mBottomYOffset + testOffset, 0.0f), rayCallBack_top);

      // Bump up if hit
      if (rayCallBack_top.hasHit())
      {
         worldTransform.setOrigin(mPreviousPosition);

         btVector3 vel(mRigidBody->getLinearVelocity());

         vel.setY(0.0f);

         mRigidBody->setLinearVelocity(vel);
      }

      mPreviousPosition = worldTransform.getOrigin();
   }

   void DynamicCharacterController::Jump()
   {
      if (mOnGround && mJumpRechargeTimer >= mJumpRechargeTime)
      {
         mJumpRechargeTimer = 0.0f;
         mRigidBody->applyCentralImpulse(btVector3(0.0f, mJumpImpulse, 0.0f));

         // Move upwards slightly so velocity isn't immediately canceled when it detects it as on ground next frame
         const float jumpYOffset = 0.01f;

         auto& worldTransform = mRigidBody->getWorldTransform();

         float previousY = worldTransform.getOrigin().getY();

         worldTransform.getOrigin().setY(previousY + jumpYOffset);
      }
   }

   bool DynamicCharacterController::IsOnGround() const
   {
      return mOnGround;
   }

}