#include "MaterialProxy.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/FloatMaterialProperty.h"

#include <algorithm>

namespace Graphics
{
   MaterialProxy::MaterialProxy(const IMaterial* material)
      : SceneProxyBase()
      , MaterialName(material->MaterialName)
      , MaterialShaderName(material->MaterialShaderName)
      , MaterialShaderRelativePath(material->MaterialShaderRelativePath)
      , mProperties(material->GetProperties())
   {
   }

   MaterialProxy::~MaterialProxy()
   {
   }

   const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& MaterialProxy::GetProperties() const
   {
      return mProperties;
   }

   std::vector<std::string> MaterialProxy::GetUniformNames() const
   {
      std::vector<std::string> result;
      std::for_each(mProperties.begin(), mProperties.end(), [&](const auto& propPair) { result.push_back(propPair.first); });
      return result;
   }

   void MaterialProxy::UpdateProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty> property)
   {
      assert(mProperties.count(propertyName));

      const auto propType = property->GetPropertyType();
      if (propType == MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY)
      {
         std::static_pointer_cast<FloatMaterialProperty>(mProperties[propertyName])->SetValue(std::static_pointer_cast<FloatMaterialProperty>(property)->GetValue());
      }
      else if (propType == MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY)
      {
         std::static_pointer_cast<TextureMaterialProperty>(mProperties[propertyName])->SetValue(std::static_pointer_cast<TextureMaterialProperty>(property)->GetValue());
      }
   }

   void MaterialProxy::UpdateProperties(const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& updatedProperties)
   {
      for (const auto& propPair : updatedProperties)
      {
         UpdateProperty(propPair.first, propPair.second);
      }
   }
}