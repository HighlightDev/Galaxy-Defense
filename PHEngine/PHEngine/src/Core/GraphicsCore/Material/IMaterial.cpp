#include "IMaterial.h"
#include "Core/IoCore/FolderManager.h"
#include "MaterialPropertySetter.h"

#include <algorithm>

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
      auto propertyIt = std::find_if(mProperties.begin(), mProperties.end(), [&](const auto& property) {return property->GetPropertyName() == propertyName; });
      assert(propertyIt != mProperties.end());

      return *propertyIt;
   }

   IMaterial::eMaterialType IMaterial::GetMaterialType() const {
      return IMaterial::eMaterialType::STATIC;
   }

   void IMaterial::PushMaterialProperty(std::shared_ptr<MaterialProperty> propertyValue) {
      mProperties.emplace_back(std::move(propertyValue));
   }

   const std::vector<std::shared_ptr<MaterialProperty>>& IMaterial::GetProperties() const
   {
      return mProperties;
   }

   std::shared_ptr<MaterialProxy> IMaterial::CreateMaterialProxy() const
   {
      return std::make_shared<MaterialProxy>(this);
   }
}
