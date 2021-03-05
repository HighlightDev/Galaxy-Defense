#include "IMaterial.h"
#include "Core/IoCore/FolderManager.h"
#include "MaterialPropertySetter.h"

namespace Graphics
{

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

   std::shared_ptr<MaterialProxy> IMaterial::GetMaterialProxy() const
   {
      return std::make_shared<MaterialProxy>(this);
   }
}
