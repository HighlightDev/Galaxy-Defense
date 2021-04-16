#pragma once

#include "MaterialProperty.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResource.h"

using namespace Resources;

namespace Graphics {

   struct DeferredTextureMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = IDeferredResource<std::shared_ptr<ITexture>, eResourceType::TEXTURE>;

   private:

      std::shared_ptr<MaterialPropertyValueType> m_value;

   public:

      DeferredTextureMaterialProperty(std::shared_ptr<MaterialPropertyValueType> propertyValue, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(propertyValue)
      {
      }

      DeferredTextureMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
      {
      }

      virtual eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::DEFERRED_TEXTURE_PROPERTY;
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
      {
         if (m_value)
         {
            std::shared_ptr<ITexture> outResource = nullptr;
            const bool bHasResource = m_value->TryGetResource(outResource);
            if (bHasResource)
            {
               int32_t slot = 10 + propertyIndex;
               outResource->BindTexture(slot);
               uniform.LoadUniform(slot);
            }
         }
      }

      void SetValue(std::shared_ptr<MaterialPropertyValueType> value) {
         m_value = value;
      }

      typename MaterialPropertyValueType::arg_t GetValue() const {
         std::shared_ptr<ITexture> outResource = nullptr;
         if (m_value)
         {
            const bool bHasResource = m_value->TryGetResource(outResource);
         }
         return outResource;
      }
   };
}