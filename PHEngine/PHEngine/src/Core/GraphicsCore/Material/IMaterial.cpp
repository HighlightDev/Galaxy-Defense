#include "IMaterial.h"
#include "Core/IoCore/FolderManager.h"
#include "MaterialPropertySetter.h"

namespace Graphics
{

   IMaterial::IMaterial(const std::string& materialName, const std::string& materialShaderName)
      : MaterialProxyId(0)
      , MaterialName(materialName)
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

   IMaterial::eMaterialType IMaterial::GetMaterialType() const {
      return IMaterial::eMaterialType::STATIC;
   }

   void IMaterial::PushMaterialProperty(std::shared_ptr<MaterialProperty> propertyValue) {
      mProperties.emplace(std::make_pair(propertyValue->GetPropertyName(), std::move(propertyValue)));
   }

   const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& IMaterial::GetProperties() const
   {
      return mProperties;
   }

   std::shared_ptr<MaterialProxy> IMaterial::CreateMaterialProxy() const
   {
      return std::make_shared<MaterialProxy>(this);
   }
}
