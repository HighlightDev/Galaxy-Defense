#include "MaterialPropertySetter.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicInstancedFloatMaterialProperty.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/MaterialInstanceDataProvider.h"
#include "TextureMaterialProperty.h"
#include "FloatMaterialProperty.h"
#include "iVec2MaterialProperty.h"
#include "Vec2MaterialProperty.h"
#include "Vec3MaterialProperty.h"
#include "DeferredTextureMaterialProperty.h"
#include "BindingMaterialProperty.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics::Texture;

namespace Graphics
{
   std::shared_ptr<DynamicMaterial> MaterialPropertySetter::TryCastToDynamicMaterial(const std::shared_ptr<IMaterial> &materialInstance)
   {
      return materialInstance->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC ? std::static_pointer_cast<DynamicMaterial>(materialInstance) : nullptr;
   }

   void MaterialPropertySetter::SetTextureValue(const std::shared_ptr<MaterialProperty> &materialProperty, const std::shared_ptr<ITexture> &texture)
   {
      const auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY);
      const auto &textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
      assert(textureProperty);
      textureProperty->SetValue(texture);
   }

   void MaterialPropertySetter::SetFloatValue(const std::shared_ptr<MaterialProperty> &materialProperty, const float value)
   {
      const auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY);
      const auto &floatProperty = std::static_pointer_cast<FloatMaterialProperty>(materialProperty);
      assert(floatProperty);
      floatProperty->SetValue(value);
   }

   void MaterialPropertySetter::SetIVec2Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::ivec2 &value)
   {
      const auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::IVEC2_PROPERTY);
      const auto &ivec2Property = std::static_pointer_cast<iVec2MaterialProperty>(materialProperty);
      assert(ivec2Property);
      ivec2Property->SetValue(value);
   }

   void MaterialPropertySetter::SetVec2Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::vec2 &value)
   {
      const auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY);
      const auto &vec2Property = std::static_pointer_cast<Vec2MaterialProperty>(materialProperty);
      assert(vec2Property);
      vec2Property->SetValue(value);
   }

   void MaterialPropertySetter::SetVec3Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::vec3 &value)
   {
      const auto propertyType = materialProperty->GetPropertyType();
      assert(propertyType == MaterialProperty::eMaterialPropertyType::VEC3_PROPERTY);
      const auto &vec3Property = std::static_pointer_cast<Vec3MaterialProperty>(materialProperty);
      assert(vec3Property);
      vec3Property->SetValue(value);
   }

   void MaterialPropertySetter::SetDeferredResourceValue(const std::shared_ptr<MaterialProperty> &materialProperty, const std::shared_ptr<IDeferredResourceCreator> &deferredResourceCreator)
   {
      if (materialProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::DEFERRED_TEXTURE_PROPERTY)
      {
         auto deferredTextureProperty = std::static_pointer_cast<DeferredTextureMaterialProperty>(materialProperty);
         assert(deferredTextureProperty);
         const auto &textureResource = std::static_pointer_cast<DeferredResource<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>(deferredResourceCreator->GetDeferredResource());
         assert(textureResource);
         deferredTextureProperty->SetValue(textureResource);
      }
      else
      {
         assert(false);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const std::shared_ptr<ITexture> &texture)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetTextureValue(property, texture);
         else
            assert(false);
      }
      else
      {
         SetTextureValue(materialInstance->GetMaterialPropertyByName(propertyName), texture);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const float value)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetFloatValue(property, value);
         else
            assert(false);
      }
      else
      {
         SetFloatValue(materialInstance->GetMaterialPropertyByName(propertyName), value);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::ivec2 &value)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetIVec2Value(property, value);
         else
            assert(false);
      }
      else
      {
         SetIVec2Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::vec2 &value)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetVec2Value(property, value);
         else
            assert(false);
      }
      else
      {
         SetVec2Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::vec3 &value)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetVec3Value(property, value);
         else
            assert(false);
      }
      else
      {
         SetVec3Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
      }
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance,
                                                         const std::string &propertyName,
                                                         const std::shared_ptr<IDeferredResourceCreator> &deferredResourceCreator)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetDeferredResourceValue(property, deferredResourceCreator);
         else
            assert(false);
      }
      else
      {
         SetDeferredResourceValue(materialInstance->GetMaterialPropertyByName(propertyName), deferredResourceCreator);
      }
   }

   bool MaterialPropertySetter::IsPropertyBindingType(const std::shared_ptr<MaterialProperty> &property)
   {
      const auto propertyType = property->GetPropertyType();
      return propertyType == MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY ||
             propertyType == MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY ||
             propertyType == MaterialProperty::eMaterialPropertyType::VEC2_BINDING_PROPERTY ||
             propertyType == MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY;
   }

   void MaterialPropertySetter::SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::shared_ptr<EngineObject> &gameObjectSp,
                                                         const std::string &gamePropertyName, const std::string &bindingName)
   {
      assert(materialInstance);

      // first try to find material property among related to dynamic property
      if (const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance))
      {
         if (const auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(bindingName))
         {
            if (IsPropertyBindingType(property))
            {
               const auto bindingProperty = std::static_pointer_cast<BindingMaterialProperty>(property);
               BindingAttachmentBuilder::SetAttachment(gameObjectSp, bindingProperty->GetMaterialBinding(), gamePropertyName);
            }
            else
               assert(false);
         }
         else
            assert(false);
      }
      else
      {
         assert(false);
      }
   }

   void MaterialPropertySetter::SetMaterialInstancedPropertyValue(const std::shared_ptr<IMaterial> &materialInstance,
                                                                  const std::shared_ptr<MaterialInstanceDataProvider> &instanceDataProvider,
                                                                  const std::shared_ptr<EngineObject> &gameObjectSp,
                                                                  const std::string &gamePropertyName,
                                                                  const std::string &bindingName)
   {
      assert(materialInstance);

      const auto &dynamicMaterial = TryCastToDynamicMaterial(materialInstance);
      assert(dynamicMaterial);
      const auto property = std::dynamic_pointer_cast<DynamicInstancedFloatMaterialProperty>(dynamicMaterial->TryGetDynamicPropertyByName(bindingName));
      assert(property);
      auto floatBinding = std::make_shared<FloatPropertyBinding>(bindingName);
      property->AddInstancedBinding(floatBinding, instanceDataProvider);
      BindingAttachmentBuilder::SetAttachment(gameObjectSp, floatBinding, gamePropertyName);
   }

}