#include "MaterialProxy.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"

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

   const std::vector<std::shared_ptr<MaterialProperty>>& MaterialProxy::GetProperties() const
   {
      return mProperties;
   }

   std::vector<std::string> MaterialProxy::GetUniformNames() const
   {
      std::vector<std::string> result;
      std::for_each(mProperties.begin(), mProperties.end(), [&](const auto& property) { result.push_back(property->GetPropertyName()); });
      return result;
   }

   void MaterialProxy::UpdateProperty(std::shared_ptr<MaterialProperty> property)
   {
      const std::string& propertyName = property->GetPropertyName();
      auto propertyIt = std::find_if(mProperties.begin(), mProperties.end(), [&](const auto& property) { return property->GetPropertyName() == propertyName; });
      assert(propertyIt != mProperties.end());

      const auto propType = property->GetPropertyType();
      if (propType == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY)
      {
         auto renderThreadProperty = std::static_pointer_cast<FloatMaterialProperty>(*propertyIt);
         auto gameThreadProperty = std::static_pointer_cast<FloatMaterialProperty>(property);
         renderThreadProperty->SetValue(gameThreadProperty->GetValue());
      }
      else if (propType == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY)
      {
         auto renderThreadProperty = std::static_pointer_cast<TextureMaterialProperty>(*propertyIt);
         auto gameThreadProperty = std::static_pointer_cast<TextureMaterialProperty>(property);
         renderThreadProperty->SetValue(gameThreadProperty->GetValue());
      }
   }

   void MaterialProxy::UpdateProperties(std::vector<std::shared_ptr<MaterialProperty>>&& updatedProperties)
   {
      std::vector<std::shared_ptr<MaterialProperty>> properties = updatedProperties;
      for (const auto& property : properties)
      {
         UpdateProperty(property);
      }
   }
}