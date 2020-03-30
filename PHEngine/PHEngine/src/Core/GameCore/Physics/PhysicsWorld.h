#pragma once


#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "BulletPhys/btBulletDynamicsCommon.h"

using namespace Graphics::Mesh;
using namespace Resources;

namespace Game
{
  /*   class btBroadphaseInterface;
     class btDefaultCollisionConfiguration;
     class btCollisionDispatcher;
     class btSequentialImpulseConstraintSolver;
     class btDiscreteDynamicsWorld;
     class btRigidBody;
     class btCollisionShape;*/

   template <typename Model>
   struct PhysicsPoolAllocationPolicy
   {
      PhysicsPoolAllocationPolicy()
      {

      }

      ~PhysicsPoolAllocationPolicy()
      {

      }
      static std::shared_ptr<Skin> AllocateMemory(std::string arg);
      static void DeallocateMemory(std::shared_ptr<Skin> arg);
   };

   struct PhysicsPool : public PoolBase<Skin, std::string, PhysicsPoolAllocationPolicy>
   {
      static std::unique_ptr<PhysicsPool> m_instance;

   public:

      using poolType_t = PoolBase<Skin, std::string, PhysicsPoolAllocationPolicy>;

      static std::unique_ptr<PhysicsPool>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<PhysicsPool>();

         return m_instance;
      }

      static void ReloadInstance()
      {
         if (m_instance)
            m_instance.reset();
      }
   };

   class PhysicsWorld
   {
   public:
      PhysicsWorld();

      ~PhysicsWorld();

      btBroadphaseInterface* mBroadphase;
      btDefaultCollisionConfiguration*        mCollisionConfiguration;
      btCollisionDispatcher*                  mDispatcher;
      btSequentialImpulseConstraintSolver*    mSolver;
      btDiscreteDynamicsWorld*                mWorld;

      btRigidBody*                            mBody;
      btRigidBody*                            mFloor;

      void Tick(const float deltaTime);

      void InitPhysics();

      glm::vec3 GetBodyWorldTransform();

      std::tuple<std::shared_ptr<Skin>, btCollisionShape*>  LoadSimpleSkinWithPhysics(const std::string& pathToObject);

      btCollisionShape* LoadFloor();

      btRigidBody* CreateBodyWithMass(float mass, btCollisionShape* shape, bool bFall, float yPos);
   };
}