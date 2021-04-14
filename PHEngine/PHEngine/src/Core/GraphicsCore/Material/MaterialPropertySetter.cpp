#include "MaterialPropertySetter.h"
#include "IMaterial.h"
#include "DynamicMaterial.h"
#include "TextureMaterialProperty.h"
#include "FloatMaterialProperty.h"
#include "DeferredTextureMaterialProperty.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"

using namespace Resources;

namespace Graphics
{

   DynamicMaterial* MaterialPropertySetter::TryCastToDynamicMaterial(IMaterial* materialIsntance) {
      if (materialIsntance->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC)
         return static_cast<DynamicMaterial*>(materialIsntance);
      else
         return nullptr;
   }


   void MaterialPropertySetter::SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, ITexture* texture)
   {
      auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY);
      auto textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
      assert(textureProperty);
      textureProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, std::shared_ptr<ITexture> texture)
   {
      auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY);
      auto textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
      assert(textureProperty);
      textureProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetFloatValue(std::shared_ptr<MaterialProperty> materialProperty, const float value)
   {
      auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY);
      auto floatProperty = std::static_pointer_cast<FloatMaterialProperty>(materialProperty);
      assert(floatProperty);
      floatProperty->SetValue(value);
   }

   void MaterialPropertySetter::SetDeferredResourceValue(std::shared_ptr<MaterialProperty> materialProperty, IDeferredResourceCreator* deferredResourceCreator)
   {
      if (materialProperty->GetPropertyType() == MaterialProperty::MaterialPropertyType::DEFERRED_TEXTURE_PROPERTY)
      {
         auto deferredTextureProperty = std::static_pointer_cast<DeferredTextureMaterialProperty>(materialProperty);
         assert(deferredTextureProperty);
         auto textureResource = std::static_pointer_cast<IDeferredResource<std::shared_ptr<ITexture>, eResourceType::TEXTURE>>(deferredResourceCreator->GetDeferredResource());
         assert(textureResource);
         deferredTextureProperty->SetValue(textureResource);
      }
      else { assert(false); }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, ITexture* texture)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (DynamicMaterial* dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetTextureValue(property, texture);
         else
            assert(false);
      }
      else
         SetTextureValue(materialInstance->GetMaterialPropertyByName(propertyName), texture);
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (DynamicMaterial* dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetTextureValue(property, texture);
         else
            assert(false);
      }
      else
         SetTextureValue(materialInstance->GetMaterialPropertyByName(propertyName), texture);
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, float value)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (DynamicMaterial* dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetFloatValue(property, value);
         else
            assert(false);
      }
      else
         SetFloatValue(materialInstance->GetMaterialPropertyByName(propertyName), value);
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, IDeferredResourceCreator* deferredResourceCreator)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (DynamicMaterial* dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetDeferredResourceValue(property, deferredResourceCreator);
         else
            assert(false);
      }
      else
         SetDeferredResourceValue(materialInstance->GetMaterialPropertyByName(propertyName), deferredResourceCreator);
   }
}