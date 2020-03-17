#pragma once

#include "BulletPhys/btBulletDynamicsCommon.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <memory>
#include <string>

using namespace Graphics::Mesh;
using namespace Resources;

namespace Game
{
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

      void Tick(const float deltaTime);

      void InitPhysics();

      std::tuple<std::shared_ptr<Skin>, btCollisionShape*>  LoadSimpleSkinWithPhysics(const std::string& pathToObject);

      btRigidBody* CreateBodyWithMass(float mass, btCollisionShape* shape);
   };
}

