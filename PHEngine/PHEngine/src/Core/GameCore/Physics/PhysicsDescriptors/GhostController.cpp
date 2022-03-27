#include "GhostController.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Components/Transform.h"

#include <glm/gtx/projection.hpp>
#include <iostream>

namespace EnginePhysics
{
   GhostController::GhostController(
       PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape, const float mass)
       : PhysicsDescriptor(pPhysicsWorld, shape, PhysicsBodyType::KINEMATIC, mass), mGhostObject(nullptr), mMotionTransform()
   {
   }

   GhostController::~GhostController()
   {
      mPhysicsWorld->GetWorld()->removeCollisionObject(mGhostObject);
      delete mGhostObject;
   }

   void GhostController::SetMotionStateWorldTransform(const btQuaternion &quat, const btVector3 &translation)
   {
      mMotionTransform = btTransform(btQuaternion(1.0f, 0.0f, 0.0f, 0.0f), translation);
   }

   void GhostController::CompletePhysicsDescriptorConstruction()
   {
      /*btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);
      // No friction, this is done manually
      rigidBodyCI.m_friction = 0.0f;
      rigidBodyCI.m_restitution = 0.0f;
      rigidBodyCI.m_linearDamping = 0.0f;
      mRigidBody = new btRigidBody(rigidBodyCI);

      // Keep upright
      mRigidBody->setAngularFactor(0.0f);
      mRigidBody->setUserPointer(static_cast<PhysicsDescriptor*>(this));

      mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);*/

      // Ghost object that is synchronized with rigid body
      mGhostObject = new btPairCachingGhostObject();

      mGhostObject->setCollisionShape(mShape->GetCollisionShape());
      mGhostObject->setUserPointer(static_cast<PhysicsDescriptor *>(this));
      mGhostObject->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);

      mPhysicsWorld->GetWorld()->addCollisionObject(mGhostObject);
   }

   void GhostController::UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime)
   {
      // Sync ghost with actually object
      //mRigidBody->setWorldTransform(mMotionTransform);
      mGhostObject->setWorldTransform(mMotionTransform);
      bIsWorldTransformDiry = false;
   }

   void GhostController::PostPhysicsSimulationUpdate()
   {
      ParseGhostContacts();
   }

   void GhostController::ParseGhostContacts()
   {
      btManifoldArray manifoldArray;
      btBroadphasePairArray &pairArray = mGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
      const auto overlappingObjects = mGhostObject->getNumOverlappingObjects();
      if (overlappingObjects)
      {
         std::cout << "getNumOverlappingObjects = " << overlappingObjects << std::endl;
      }
      const auto numPairs = pairArray.size();

      for (int i = 0; i < numPairs; ++i)
      {
         manifoldArray.clear();

         const btBroadphasePair &pair = pairArray[i];

         btBroadphasePair *collisionPair = mPhysicsWorld->GetWorld()->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

         if (nullptr == collisionPair)
            continue;

         std::cout << "Collision !" << std::endl;

         // if (collisionPair->m_algorithm != NULL)
         //    collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

         // for (int j = 0; j < manifoldArray.size(); j++)
         // {
         //    btPersistentManifold* pManifold = manifoldArray[j];

         //    // Skip the rigid body the ghost monitors
         //    if (pManifold->getBody0() == mRigidBody)
         //       continue;

         //    for (int p = 0; p < pManifold->getNumContacts(); p++)
         //    {
         //       const btManifoldPoint &point = pManifold->getContactPoint(p);

         //       if (point.getDistance() < 0.0f)
         //       {
         //          //const btVector3 &ptA = point.getPositionWorldOnA();
         //          const btVector3 &ptB = point.getPositionWorldOnB();

         //          //const btVector3 &normalOnB = point.m_normalWorldOnB;

         //          // If point is in rounded bottom region of capsule shape, it is on the ground
         //          if (ptB.getY() < mMotionTransform.getOrigin().getY() - mBottomRoundedRegionYOffset)
         //             mOnGround = true;
         //          else
         //          {
         //             mHittingWall = true;

         //             mSurfaceHitNormals.push_back(point.m_normalWorldOnB);
         //          }
         //       }
         //    }
         // }
      }
   }
}