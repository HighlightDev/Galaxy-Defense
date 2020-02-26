#pragma once
#include "SceneComponent.h"
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/btBulletDynamicsCommon.h>

namespace Game
{

   class CollisionComponent :
      public SceneComponent
   {
      
      bool bStaticMesh;

      float m_mass;
      btVector3 m_inertia;
      btCollisionShape* m_collisionShape;
      btMotionState* m_motionState;

      btRigidBody* m_rigidBody = nullptr;

   public:

      CollisionComponent(float mass, bool IsStaticMesh);

      virtual ~CollisionComponent();

      virtual void Tick(const float deltaTime) override;
   };

}

