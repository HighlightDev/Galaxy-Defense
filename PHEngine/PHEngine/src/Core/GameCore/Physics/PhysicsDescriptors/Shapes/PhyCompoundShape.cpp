#include "PhyCompoundShape.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EnginePhysics
{

   PhyCompoundShape::PhyCompoundShape()
      : PhysicsShapeBase(new btCompoundShape())
   {
   }

   PhyCompoundShape::~PhyCompoundShape()
   {
      auto compoundShape = static_cast<btCompoundShape*>(mCollisionShape);
      const auto shapeCount = compoundShape->getNumChildShapes();
      for (int32_t shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
      {
         delete compoundShape->getChildShape(shapeIndex);
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
   }

   
}