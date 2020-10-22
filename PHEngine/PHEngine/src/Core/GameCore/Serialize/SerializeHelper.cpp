#include "SerializeHelper.h"

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace Graphics;

namespace Game {

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
