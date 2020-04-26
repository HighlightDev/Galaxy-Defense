#include "PhysicsWorld.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpMeshLoader.h"

#include <iostream>

namespace Game
{

   PhysicsWorld::PhysicsWorld()
      : Event::PhysicsDescriptorRemovedEvent()
      , mBroadphase(nullptr)
      , mCollisionConfiguration(nullptr)
      , mDispatcher(nullptr)
      , mSolver(nullptr)
      , mWorld(nullptr)
#if DEBUG
      , mDebugRenderer(new BulletDebugRenderer())
#endif
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->AddListener(this);
   }

   PhysicsWorld::~PhysicsWorld()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->RemoveListener(this);

      for (size_t i = 0; i < mPhysicsDescriptors.size(); ++i)
      {
         RemovePhysDescriptorFromSimulation(mPhysicsDescriptors[i]);
      }
      mPhysicsDescriptors.clear();

      delete mBroadphase;
      delete mCollisionConfiguration;
      delete mDispatcher;
      delete mSolver;
#if DEBUG
      delete mDebugRenderer;
#endif
      delete mWorld;
   }

   void PhysicsWorld::InitPhysicsWorld()
   {
      mBroadphase = new btDbvtBroadphase();

      mCollisionConfiguration = new btDefaultCollisionConfiguration();
      mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

      mSolver = new btSequentialImpulseConstraintSolver();

      mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

      mWorld->setGravity(btVector3(btScalar(0.0f), btScalar(-9.8f), btScalar(0.0f)));

#if DEBUG
      mWorld->setDebugDrawer(mDebugRenderer);
#endif
   }

   void PhysicsWorld::AddPhysDescriptor(PhysicsDescriptor* inDescriptor)
   {
      mPhysicsDescriptors.push_back(inDescriptor);
   }

   void PhysicsWorld::RemovePhysDescriptorFromSimulation(PhysicsDescriptor* descriptor)
   {
      mWorld->removeCollisionObject(descriptor->GetRigidBody());

      // !!! ATTENTION !!!
      // this is the only place,
      // where descriptor could be deleted.
      delete descriptor;
   }

   void PhysicsWorld::JoinPhysDescriptorsForSimulation()
   {
      for (int32_t i = 0; i < mPhysicsDescriptors.size(); ++i)
      {
         btRigidBody* body = mPhysicsDescriptors[i]->GetRigidBody();

         if (body)
         {
            mWorld->addRigidBody(body);
         }
      }

      mPhysicsDescriptors[0]->GetRigidBody()->setLinearVelocity(btVector3(-2.5f, 0, 0));
   }

   void PhysicsWorld::Tick(const float deltaTime)
   {
      mWorld->stepSimulation(deltaTime);

#if DEBUG
      mDebugRenderer->ClearLinesBuffer();
      mWorld->debugDrawWorld();
#endif
   }

#if DEBUG
   const DebugPhysicsRenderData& PhysicsWorld::GetDebugPhysicsRenderData() const
   {
      return mDebugRenderer->GetRenderData();
   }
#endif

   void PhysicsWorld::ProcessEvent(const Event::PhysicsDescriptorRemovedEvent::EventData_t& data)
   {
      auto removedDescriptorIt 
         = std::find_if(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [=](const PhysicsDescriptor* physDesc) { return physDesc->GetId() == std::get<0>(data); });

      mPhysicsDescriptors.erase(removedDescriptorIt);
      RemovePhysDescriptorFromSimulation(*removedDescriptorIt);
   }
}
