#include "MaterialPropertySetter.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "TextureMaterialProperty.h"
#include "FloatMaterialProperty.h"
#include "DeferredTextureMaterialProperty.h"
#include "BindingMaterialProperty.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"

using namespace Resources;
using namespace EngineCore;

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
      assert(propertyType == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY);
      auto textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
      assert(textureProperty);
      textureProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, std::shared_ptr<ITexture> texture)
   {
      auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY);
      auto textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
      assert(textureProperty);
      textureProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetFloatValue(std::shared_ptr<MaterialProperty> materialProperty, const float value)
   {
      auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY);
      auto floatProperty = std::static_pointer_cast<FloatMaterialProperty>(materialProperty);
      assert(floatProperty);
      floatProperty->SetValue(value);
   }

   void MaterialPropertySetter::SetDeferredResourceValue(std::shared_ptr<MaterialProperty> materialProperty, const std::shared_ptr<IDeferredResourceCreator>& deferredResourceCreator)
   {
      if (materialProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::DEFERRED_TEXTURE_PROPERTY)
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

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const std::string& propertyName, const std::shared_ptr<IDeferredResourceCreator>& deferredResourceCreator)
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

   bool MaterialPropertySetter::IsPropertyBindingType(std::shared_ptr<MaterialProperty> property, MaterialProperty::eMaterialPropertyType& outPropertyType)
   {
      outPropertyType = property->GetPropertyType();
      switch (outPropertyType)
      {
         case MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY:
            return true;
         default:
            return false;
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(IMaterial* materialInstance, const GameObject* gameObject,
      const std::string& gamePropertyName, const std::string& bindingName)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const DynamicMaterial* dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(bindingName))
         {
            MaterialProperty::eMaterialPropertyType outPropertyType;
            if (IsPropertyBindingType(property, outPropertyType))
            {
               auto bindingProperty = std::static_pointer_cast<BindingMaterialProperty>(property);
               BindingAttachmentBuilder::SetAttachment(gameObject, bindingProperty->GetMaterialBinding().get(), gamePropertyName);
            }
            else
               assert(false);
         }
         else
            assert(false);
      }
      else assert(false);
   }

}