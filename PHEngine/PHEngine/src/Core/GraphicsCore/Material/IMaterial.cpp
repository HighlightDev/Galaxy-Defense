#include "IMaterial.h"
#include "Core/IoCore/FolderManager.h"

namespace Graphics
{

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, ITexture* texture)
   {
      assert(materialInstance);

      auto property = materialInstance->GetMaterialPropertyByName(propertyName);
      auto texProperty = std::static_pointer_cast<TextureMaterialProperty>(property);

      assert(texProperty);

      texProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture)
   {
      assert(materialInstance);

      auto property = materialInstance->GetMaterialPropertyByName(propertyName);
      auto texProperty = std::static_pointer_cast<TextureMaterialProperty>(property);

      assert(texProperty);

      texProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, float value)
   {
      assert(materialInstance);

      auto property = materialInstance->GetMaterialPropertyByName(propertyName);
      auto floatProperty = std::static_pointer_cast<FloatMaterialProperty>(property);

      assert(floatProperty);

      floatProperty->SetValue(value);
   }

   IMaterial::IMaterial(const std::string& materialName, const std::string& materialShaderName)
      : MaterialName(materialName)
      , MaterialShaderName(materialShaderName)
      , MaterialShaderRelativePath(IO::FolderManager::GetInstance()->GetShadersPath() + "\\material_shaders\\" + materialShaderName)
   {
   }

   IMaterial::~IMaterial()
   {
   }

   std::shared_ptr<MaterialProperty> IMaterial::GetMaterialPropertyByName(const std::string& propertyName) const
   {
      assert(mProperties.count(propertyName));

      return mProperties.at(propertyName);
   }

   void IMaterial::PushMaterialProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty>&& propertyValue) {
      mProperties.emplace(std::make_pair(propertyName, std::forward<std::shared_ptr<MaterialProperty>>(propertyValue)));
   }

   const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& IMaterial::GetProperties() const
   {
      return mProperties;
   }
}
