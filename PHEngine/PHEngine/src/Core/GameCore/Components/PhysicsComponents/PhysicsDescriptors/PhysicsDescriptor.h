#pragma once

#include "Shapes/PhyShapeBase.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

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

      float mPrevTransformMatrix[16];

      glm::vec3 mRotation;

      glm::vec3 mTranslation;

   public:

      PhysicsDescriptor(PhyShapeBase* shape, const float mass = 0.0f);

      ~PhysicsDescriptor();

      PhyShapeBase* GetShape() const;

      size_t GetId() const;

      void SetMotionStateWorldTransform(const float yaw, const float pitch, const float roll, const glm::vec3& translation);

      btRigidBody* GetRigidBody() const;

      btMotionState* GetMotionState() const;

      void CompleteRigidBodyConstruction();

      void UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry);

      float* GetTrasformMatrix4x4();
      
      glm::vec3 GetEulerRotationDegrees() const;
      
      glm::vec3 GetTranslation() const;

   private:

      btTransform GetMotionWorldTransform() const;
   };

}
