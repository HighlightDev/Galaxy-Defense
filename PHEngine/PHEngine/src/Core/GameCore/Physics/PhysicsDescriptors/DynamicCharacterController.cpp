#include "DynamicCharacterController.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

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
      btPairCachingGhostObject* m_pGhostObject;

   public:
      IgnoreBodyAndGhostCast(btRigidBody* pBody, btPairCachingGhostObject* pGhostObject)
         : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
         m_pBody(pBody), m_pGhostObject(pGhostObject)
      {
      }

      btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
      {
         if (rayResult.m_collisionObject == m_pBody || rayResult.m_collisionObject == m_pGhostObject)
            return 1.0f;

         return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
      }
   };

   DynamicCharacterController::DynamicCharacterController(
      PhysicsWorld* pPhysicsWorld
      , float radius
      , float height
      , float mass
      , float stepHeight)
      : PhysicsDescriptor(pPhysicsWorld, new PhyCapsuleShape(radius, height), mass)
      , m_bottomYOffset(height / 3.0f + radius)
      , m_bottomRoundedRegionYOffset((height + radius) / 3.0f)
      , m_deceleration(0.1f)
      , m_maxSpeed(15.0f)
      , m_jumpImpulse(150)
      , m_manualVelocity(0.0f, 0.0f, 0.0f)
      , m_onGround(false)
      , m_hittingWall(false)
      , m_jumpRechargeTimer(0.0f)
      , m_jumpRechargeTime(10.0f)
      , m_stepHeight(stepHeight)
   {
   }

   DynamicCharacterController::~DynamicCharacterController()
   {
      mPhysicsWorld->GetWorld()->removeRigidBody(mRigidBody);
      mPhysicsWorld->GetWorld()->removeCollisionObject(m_pGhostObject);

      delete m_pGhostObject;
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

      // No sleeping (or else setLinearVelocity won't work)
      mRigidBody->setActivationState(DISABLE_DEACTIVATION);

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);

      // Ghost object that is synchronized with rigid body
      m_pGhostObject = new btPairCachingGhostObject();

      m_pGhostObject->setCollisionShape(mShape->GetCollisionShape());
      m_pGhostObject->setUserPointer(this);
      m_pGhostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

      // Specify filters manually, otherwise ghost doesn't collide with statics for some reason
      mPhysicsWorld->GetWorld()->addCollisionObject(m_pGhostObject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
   }

   void DynamicCharacterController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry)
   {
      // Sync ghost with actually object
      m_pGhostObject->setWorldTransform(mRigidBody->getWorldTransform());
      // Update transform
      mMotionState->getWorldTransform(m_motionTransform);

      m_onGround = false;

      ParseGhostContacts();

      UpdatePosition();
      UpdateVelocity();

      // Update jump timer
      if (m_jumpRechargeTimer < m_jumpRechargeTime)
         m_jumpRechargeTimer += timerMultiplier;

      if (bIsWorldTransformDiry = !(isEqual(m_motionTransform, mPrevTransform)))
      {
         // Update data
         mPrevTransform = m_motionTransform;
         mTranslation = m_motionTransform.getOrigin();
         mVelocity = mRigidBody->getLinearVelocity();
      }
   }

   void DynamicCharacterController::Walk(const glm::vec2& dir)
   {
      glm::vec2 velocityXZ(dir + glm::vec2(m_manualVelocity.getX(), m_manualVelocity.getZ()));

      // Prevent from going over maximum speed
      float speedXZ = glm::length(velocityXZ);

      if (speedXZ > m_maxSpeed)
         velocityXZ = velocityXZ / speedXZ * m_maxSpeed;

      m_manualVelocity.setX(velocityXZ.x);
      m_manualVelocity.setZ(velocityXZ.y);
   }

   void DynamicCharacterController::Walk(const glm::vec3& dir)
   {
      Walk(glm::vec2(dir.x, dir.z));
   }

   void DynamicCharacterController::ParseGhostContacts()
   {
      btManifoldArray manifoldArray;
      btBroadphasePairArray &pairArray = m_pGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
      int numPairs = pairArray.size();

      // Set false now, may be set true in test
      m_hittingWall = false;

      m_surfaceHitNormals.clear();

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
                  if (ptB.getY() < m_motionTransform.getOrigin().getY() - m_bottomRoundedRegionYOffset)
                     m_onGround = true;
                  else
                  {
                     m_hittingWall = true;

                     m_surfaceHitNormals.push_back(point.m_normalWorldOnB);
                  }
               }
            }
         }
      }
   }

   void DynamicCharacterController::UpdateVelocity()
   {
      // Adjust only xz velocity
      m_manualVelocity.setY(mRigidBody->getLinearVelocity().getY());

      mRigidBody->setLinearVelocity(m_manualVelocity);

      // Decelerate
      m_manualVelocity -= m_manualVelocity * m_deceleration * timerMultiplier;

      if (m_hittingWall)
      {
         for (unsigned int i = 0, size = m_surfaceHitNormals.size(); i < size; i++)
         {
            // Cancel velocity across normal
            glm::vec3 surfaceNormal = Converter::bulletToGlm(m_surfaceHitNormals[i]);
            glm::vec3 velocity = Converter::bulletToGlm(m_manualVelocity);
            glm::vec3 projection = glm::proj(velocity, surfaceNormal);

            btVector3 velInNormalDir(Converter::glmToBullet(projection));

            // Apply correction
            m_manualVelocity -= velInNormalDir * 1.05f;
         }

         // Do not adjust rigid body velocity manually (so bodies can still be pushed by character)
         return;
      }
   }

   void DynamicCharacterController::UpdatePosition()
   {
      // Ray cast, ignore rigid body
      IgnoreBodyAndGhostCast rayCallBack_bottom(mRigidBody, m_pGhostObject);

      mPhysicsWorld->GetWorld()->rayTest(mRigidBody->getWorldTransform().getOrigin(),
         mRigidBody->getWorldTransform().getOrigin() - btVector3(0.0f, m_bottomYOffset + m_stepHeight, 0.0f), rayCallBack_bottom);

      // Bump up if hit
      if (rayCallBack_bottom.hasHit())
      {
         float previousY = mRigidBody->getWorldTransform().getOrigin().getY();

         mRigidBody->getWorldTransform().getOrigin().setY(previousY + (m_bottomYOffset + m_stepHeight) * (1.0f - rayCallBack_bottom.m_closestHitFraction));

         btVector3 vel(mRigidBody->getLinearVelocity());

         vel.setY(0.0f);

         mRigidBody->setLinearVelocity(vel);

         m_onGround = true;
      }

      float testOffset = 0.07f;

      // Ray cast, ignore rigid body
      IgnoreBodyAndGhostCast rayCallBack_top(mRigidBody, m_pGhostObject);

      mPhysicsWorld->GetWorld()->rayTest(mRigidBody->getWorldTransform().getOrigin(),
         mRigidBody->getWorldTransform().getOrigin() + btVector3(0.0f, m_bottomYOffset + testOffset, 0.0f), rayCallBack_top);

      // Bump up if hit
      if (rayCallBack_top.hasHit())
      {
         mRigidBody->getWorldTransform().setOrigin(m_previousPosition);

         btVector3 vel(mRigidBody->getLinearVelocity());

         vel.setY(0.0f);

         mRigidBody->setLinearVelocity(vel);
      }

      m_previousPosition = mRigidBody->getWorldTransform().getOrigin();
   }

   void DynamicCharacterController::Jump()
   {
      if (m_onGround && m_jumpRechargeTimer >= m_jumpRechargeTime)
      {
         m_jumpRechargeTimer = 0.0f;
         mRigidBody->applyCentralImpulse(btVector3(0.0f, m_jumpImpulse, 0.0f));

         // Move upwards slightly so velocity isn't immediately canceled when it detects it as on ground next frame
         const float jumpYOffset = 0.01f;

         float previousY = mRigidBody->getWorldTransform().getOrigin().getY();

         mRigidBody->getWorldTransform().getOrigin().setY(previousY + jumpYOffset);
      }
   }

   bool DynamicCharacterController::IsOnGround() const
   {
      return m_onGround;
   }

}