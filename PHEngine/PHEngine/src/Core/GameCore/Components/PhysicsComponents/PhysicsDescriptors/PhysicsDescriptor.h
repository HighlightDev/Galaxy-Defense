#pragma once

#include "Shapes/PhyShapeBase.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <BulletPhys/btBulletDynamicsCommon.h>

namespace Game
{

   struct PhysicsDescriptor
   {
   private:

      static size_t mTotalIds;

      size_t mCurrentId;

      PhyShapeBase* mShape;

      btMotionState* mMotionState;

      // default value is 0.0f which means that this physics body is completely STATIC
      float mMass;

      btVector3 mInertia;

      btRigidBody* mRigidBody;

      btQuaternion mRotator;

      btVector3 mTranslation;

   public:

      PhysicsDescriptor(PhyShapeBase* shape, const float mass = 0.0f);

      ~PhysicsDescriptor();

      PhyShapeBase* GetShape() const;

      size_t GetId() const;

      void SetMotionStateWorldTransform(const float yaw, const float pitch, const float roll, const glm::vec3& translation);
      void SetMotionStateWorldTransform(const btQuaternion& quat, const glm::vec3& translation);

      btRigidBody* GetRigidBody() const;

      btMotionState* GetMotionState() const;

      void CompleteRigidBodyConstruction();

      void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry);

      glm::vec3 GetTranslation() const;

      btQuaternion GetRotator() const;

   private:

      btTransform GetMotionWorldTransform() const;
   };

}
