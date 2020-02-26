#include "CollisionComponent.h"

namespace Game
{

   CollisionComponent::CollisionComponent(float mass, bool IsStaticMesh)
      : SceneComponent()
      , bStaticMesh(IsStaticMesh)
      , m_mass(mass)
      , m_collisionShape(new btSphereShape(5))
      , m_motionState(nullptr)
   {
   }

   CollisionComponent::~CollisionComponent()
   {
   }

   void CollisionComponent::Tick(const float deltaTime)
   {
      SceneComponent::Tick(deltaTime);

      if (!bStaticMesh && std::abs(m_mass) != 0.0001f)
      {
         m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
      }

      btTransform worldTransform;
      worldTransform.setOrigin(btVector3(m_translation.x, m_translation.y, m_translation.z));

      worldTransform.setBasis(btMatrix3x3(m_relativeMatrix[0][0], m_relativeMatrix[1][0], m_relativeMatrix[2][0],
                             m_relativeMatrix[0][1], m_relativeMatrix[1][1], m_relativeMatrix[2][1],
                             m_relativeMatrix[0][2], m_relativeMatrix[1][2], m_relativeMatrix[2][2]));

      m_rigidBody->setWorldTransform(worldTransform);

      if (m_motionState)
      {
         delete m_motionState;
         m_motionState = nullptr;
      }

      m_motionState = new btDefaultMotionState(worldTransform);
      btRigidBody::btRigidBodyConstructionInfo info(m_mass, m_motionState, m_collisionShape, m_inertia);
      m_rigidBody = new btRigidBody(info);
   }
}
