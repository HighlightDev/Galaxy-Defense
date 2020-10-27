#include "SerializeHelper.h"

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"

using namespace Graphics;
using namespace EnginePhysics;

namespace Game {

   std::shared_ptr<SerializeDataPhysicsComponent> SerializeHelper::GetSerializeDataPhysicsComponent(PhysicsComponent* component)
   {
      std::shared_ptr<SerializeDataPhysicsComponent> physCompData = std::make_shared<SerializeDataPhysicsComponent>();

      std::shared_ptr<SerializeDataPhysicsShape> PhysicsShape;

      PhyShapeBase* physShape = component->GetDescriptor()->GetShape();
      int32_t shapeType = physShape->GetCollisionShape()->getShapeType();
      
      if (SPHERE_SHAPE_PROXYTYPE == shapeType)
      {
         PhySphereShape* sphere = static_cast<PhySphereShape*>(physShape);
         auto shape = std::make_shared<SerializeDataSpherePhysicsShape>();
         shape->Radius = sphere->GetRadius();
         PhysicsShape = shape;
      }
      else if (BOX_SHAPE_PROXYTYPE == shapeType)
      {
         PhyBoxShape* box = static_cast<PhyBoxShape*>(physShape);
         auto shape = std::make_shared<SerializeDataBoxPhysicsShape>();
         shape->HalfExtent = box->GetHalfExtent();
         PhysicsShape = shape;
      }
      else if (CAPSULE_SHAPE_PROXYTYPE == shapeType)
      {
         PhyCapsuleShape* capsule = static_cast<PhyCapsuleShape*>(physShape);
         auto shape = std::make_shared<SerializeDataCapsulePhysicsShape>();
         shape->Height = capsule->GetHeight();
         shape->Radius = capsule->GetRadius();
         PhysicsShape = shape;
      }

      physCompData->PhysicsShape = PhysicsShape;
      physCompData->Mass = component->GetDescriptor()->GetMass();
      auto motionModifiers = component->GetDescriptor()->GetMotionModifiers();
      physCompData->AngularFactor = Converter::bulletToGlm(motionModifiers.AngularFactor);
      physCompData->LinearFactor = Converter::bulletToGlm(motionModifiers.LinearFactor);
      physCompData->BodyType = component->GetDescriptor()->GetPhysicsBodyType();

      return physCompData;
   }

   SerializeDataMaterial SerializeHelper::GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance)
   {
      SerializeDataMaterial materialData;

      materialData.MaterialName = materialInstance->MaterialName;
      materialData.MaterialShaderRelPath = materialInstance->RelativeMaterialShaderPath;

      const auto& properties = materialInstance->GetProperties();

      for (const auto& propertyItem : properties)
      {
         const std::string& uniformName = propertyItem.first;
         std::shared_ptr<MaterialProperty> prop = propertyItem.second;
         auto type = prop->GetMaterialPropertyType();

         std::string uniformValue, propertyType;

         if (type == MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY)
         {
            std::shared_ptr<TextureMaterialProperty> texProp = std::static_pointer_cast<TextureMaterialProperty>(prop);
            propertyType = "texture";
            uniformValue = Resources::TexturePool::GetInstance()->GetKey(texProp->GetValue());
         }
         else if (type == MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY)
         {
            std::shared_ptr<FloatMaterialProperty> floatProp = std::static_pointer_cast<FloatMaterialProperty>(prop);
            propertyType = "float";
            uniformValue = std::to_string(floatProp->GetValue());
         }

         SerializeDataMaterial::SerializeDataMaterialProperty property;
         property.PropertyType = propertyType;
         property.Value = uniformValue;
         property.UniformName = uniformName;

         materialData.Properties.emplace_back(property);

      }

      return materialData;
   }
}
