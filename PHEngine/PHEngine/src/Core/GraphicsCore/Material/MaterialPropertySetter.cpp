#include "MaterialPropertySetter.h"
#include "IMaterial.h"

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
}