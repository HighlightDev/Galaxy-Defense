#include "GhostController.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Event/PhysicsCollisionOccuredEvent.h"

#include <glm/gtx/projection.hpp>
#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;
using namespace Event;

namespace EnginePhysics
{
   GhostController::GhostController(
       PhysicsWorld *pPhysicsWorld, PhysicsShapeBase *shape,
       const float mass,
       const int32_t collisionFilterGroup,
       const int32_t collisionFilterMask)
       : PhysicsDescriptor(pPhysicsWorld, shape, ePhysicsBodyType::GHOST, mass),
         btCollisionWorld::ContactResultCallback(),
         mGhostObject(nullptr),
         mMotionTransform(),
         mCollisionCooldown(0.0f)
   {
      m_collisionFilterGroup = collisionFilterGroup;
      m_collisionFilterMask = collisionFilterMask;
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
      mGhostObject = new btPairCachingGhostObject();

      mGhostObject->setCollisionShape(mShape->GetCollisionShape());
      mGhostObject->setUserPointer(static_cast<PhysicsDescriptor *>(this));
      mGhostObject->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);

      mPhysicsWorld->GetWorld()->addCollisionObject(mGhostObject, m_collisionFilterGroup, m_collisionFilterMask);
   }

   void GhostController::SetIsCollisionEnabled(const bool isCollisionEnabled)
   {
      assert(mGhostObject);
      const int collisionMask = isCollisionEnabled
                                    ? mGhostObject->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE
                                    : mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE;
      mGhostObject->setCollisionFlags(collisionMask);
   }

   void GhostController::UpdateMotionWorldTransformLocalState(bool &bIsWorldTransformDiry, const float deltaTime)
   {
      // Sync ghost with actually object
      mGhostObject->setWorldTransform(mMotionTransform);
      bIsWorldTransformDiry = false;
   }

   void GhostController::PostPhysicsSimulationUpdate(const float deltaTime)
   {
      mCollisionCooldown += deltaTime;
      ParseGhostContacts();
   }

   btScalar GhostController::addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0,
                                             int partId0, int index0,
                                             const btCollisionObjectWrapper *colObj1,
                                             int partId1,
                                             int index1)
   {
      const auto &collidedObject = colObj1->getCollisionObject();
      if (mGhostObject == collidedObject)
         return 1.0f;

      const auto &collidedObjDescriptor = reinterpret_cast<PhysicsDescriptor *>(collidedObject->getUserPointer());

      PhysicsCollisionOccuredEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                             ePhysicsBodyType::GHOST,
                                                             mCurrentId, collidedObjDescriptor->GetId());

      Logger::Out("Collision detected; my descriptor = ", mCurrentId, " collided object descriptor = ", collidedObjDescriptor->GetId());
      return 0;
   }

   void GhostController::ParseGhostContacts()
   {
      if (mCollisionCooldown > sCollisionCooldownTimeout)
      {
         mPhysicsWorld->GetWorld()->contactTest(mGhostObject, *this);
         mCollisionCooldown = fmod(mCollisionCooldown, sCollisionCooldownTimeout);
      }
   }
}