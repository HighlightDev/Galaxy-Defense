#include "MaterialPropertySetter.h"
#include "IMaterial.h"
#include "TextureMaterialProperty.h"
#include "FloatMaterialProperty.h"
#include "DeferredTextureMaterialProperty.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"

using namespace Resources;

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

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, IDeferredResourceCreator* deferredResourceCreator)
   {
      assert(materialInstance);

      auto property = materialInstance->GetMaterialPropertyByName(propertyName);

      if (property->GetMaterialPropertyType() == MaterialProperty::MaterialPropertyType::DEFERRED_TEXTURE_PROPERTY)
      {
         auto deferredTextureProperty = std::static_pointer_cast<DeferredTextureMaterialProperty>(property);
         assert(deferredTextureProperty);
         auto textureResource = std::static_pointer_cast<IDeferredResource<std::shared_ptr<ITexture>, eResourceType::TEXTURE>>(deferredResourceCreator->GetDeferredResource());
         assert(textureResource);
         deferredTextureProperty->SetValue(textureResource);
      }
   }
}