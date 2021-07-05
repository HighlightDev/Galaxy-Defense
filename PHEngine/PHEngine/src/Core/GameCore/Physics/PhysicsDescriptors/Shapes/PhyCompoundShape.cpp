#include "PhyCompoundShape.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EnginePhysics
{

   PhyCompoundShape::PhyCompoundShape()
      : PhysicsShapeBase(new btCompoundShape())
      , mChildShapes()
   {
   }

   PhyCompoundShape::~PhyCompoundShape()
   {
      for (auto& childShapePtr : mChildShapes)
      {
         delete childShapePtr.first;
      }
   }

   void PhyCompoundShape::AddChildShape(const NoScaleEulerRotationTransform& childTransform, PhysicsShapeBase* childShape)
   {
      btTransform localTransform;
      localTransform.setIdentity();
      localTransform.setOrigin(Converter::glmToBullet(childTransform.Translation));
      localTransform.setRotation(Converter::glmToBullet(
         glm::quat(glm::vec3(DEG_TO_RAD(childTransform.RotationEulerAngles.x),
            DEG_TO_RAD(childTransform.RotationEulerAngles.y),
            DEG_TO_RAD(childTransform.RotationEulerAngles.z)))));

      auto compoundShape = static_cast<btCompoundShape*>(mCollisionShape);
      compoundShape->addChildShape(localTransform, childShape->GetCollisionShape());
      mChildShapes.emplace_back(childShape);
   }

   const std::vector<std::pair<PhysicsShapeBase*, NoScaleEulerRotationTransform>>& PhyCompoundShape::GetChildShapes() const
   {
      return mChildShapes;
   }
   
}