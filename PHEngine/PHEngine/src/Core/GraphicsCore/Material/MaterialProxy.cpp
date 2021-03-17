#include "MaterialProxy.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
   size_t MaterialProxy::MaterialProxyIdCounter = 0;

   MaterialProxy::MaterialProxy(const IMaterial* material)
      : MaterialName(material->MaterialName)
      , MaterialShaderName(material->MaterialShaderName)
      , MaterialShaderRelativePath(material->MaterialShaderRelativePath)
      , mMaterialProxyId(MaterialProxyIdCounter)
      , mProperties(material->GetProperties())
   {
      ++MaterialProxyIdCounter;
   }

   MaterialProxy::~MaterialProxy()
   {
   }

   const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& MaterialProxy::GetProperties() const
   {
      return mProperties;
   }

   size_t MaterialProxy::GetMaterialProxyId() const {

      return mMaterialProxyId;
   }

   void MaterialProxy::UpdateProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty> property)
   {
      assert(mProperties.count(propertyName));

      const auto propType = property->GetMaterialPropertyType();
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