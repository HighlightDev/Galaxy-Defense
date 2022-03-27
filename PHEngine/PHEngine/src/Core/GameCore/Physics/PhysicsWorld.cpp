#include "PhysicsWorld.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace EnginePhysics
{

   PhysicsWorld::PhysicsWorld()
       : Event::PhysicsDescriptorRemovedEvent(),
         mBroadphase(nullptr),
         mCollisionConfiguration(nullptr),
         mDispatcher(nullptr),
         mSolver(nullptr),
         mWorld(nullptr)
#if DEBUG
         ,
         mDebugRenderer(new BulletDebugRenderer())
#endif
   {
      Logger::Out("PhysicsWorld::ctor");

      Event::PhysicsDescriptorRemovedEvent::GetInstance()->AddListener(this);
   }

   PhysicsWorld::~PhysicsWorld()
   {
      Logger::Out("PhysicsWorld::dctor");

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

   btDiscreteDynamicsWorld *PhysicsWorld::GetWorld() const
   {
      return mWorld;
   }

   void PhysicsWorld::AddPhysDescriptor(PhysicsDescriptor *inDescriptor)
   {
      Logger::Out("PhysicsWorld::AddPhysDescriptor; descriptor id = ", inDescriptor->GetId());

      mPhysicsDescriptors.push_back(inDescriptor);
   }

   void PhysicsWorld::RemovePhysDescriptorFromSimulation(PhysicsDescriptor *descriptor)
   {
      Logger::Out("PhysicsWorld::RemovePhysDescriptorFromSimulation; descriptor id = ", descriptor->GetId());
      // !!! ATTENTION !!!
      // this is the only place,
      // where descriptor could be deleted.
      delete descriptor;
   }

   void PhysicsWorld::Tick(const float deltaTime)
   {
      if (mPhysicsDescriptors.size())
      {
         mWorld->stepSimulation(deltaTime);

         PostPhysicsSimulationUpdate();

#if DEBUG
         mDebugRenderer->ClearLinesBuffer();
         mWorld->debugDrawWorld();
#endif
      }
   }

   void PhysicsWorld::PostPhysicsSimulationUpdate()
   {
      std::for_each(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [](const auto &descriptor)
                    { descriptor->PostPhysicsSimulationUpdate(); });
   }

#if DEBUG
   const DebugPhysicsRenderData &PhysicsWorld::GetDebugPhysicsRenderData() const
   {
      return mDebugRenderer->GetRenderData();
   }
#endif

   void PhysicsWorld::ProcessEvent(const Event::PhysicsDescriptorRemovedEvent::EventData_t &data)
   {
      auto removedDescriptorIt = std::find_if(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [&](const PhysicsDescriptor *physDesc)
                                              { return physDesc->GetId() == std::get<0>(data); });

      mPhysicsDescriptors.erase(removedDescriptorIt);
      RemovePhysDescriptorFromSimulation(*removedDescriptorIt);
   }
}
