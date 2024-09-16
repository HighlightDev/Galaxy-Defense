#pragma once
#include "DynamicMaterialProperty.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/InstancedFloatMaterialProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/PropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/MaterialInstanceDataProvider.h"

#include <algorithm>
#include <limits>
#include <any>
#include <vector>
#include <utility>
#include <memory>

#undef max
#undef min

using namespace EngineCore;
using namespace Resources;

namespace Graphics
{

   struct DynamicInstancedFloatMaterialProperty
       : public DynamicMaterialProperty
   {
   private:
      std::vector<std::pair<std::shared_ptr<PropertyBinding>, std::shared_ptr<MaterialInstanceDataProvider>>> mInstancedBindings;

   public:
      explicit DynamicInstancedFloatMaterialProperty(const std::string &propertyName)
          : DynamicMaterialProperty(propertyName)
      {
      }

      eDynamicMaterialPropertyType GetPropertyType() const override
      {
         return DynamicMaterialProperty::eDynamicMaterialPropertyType::InstancedFloatProperty;
      }

      void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty> &staticProperty) override
      {
         assert(MaterialProperty::eMaterialPropertyType::FLOAT_INSTANCED_PROPERTY == staticProperty->GetPropertyType());
         const auto &staticInstancedFloatMaterialProperty = std::static_pointer_cast<InstancedFloatMaterialProperty>(staticProperty);
         staticInstancedFloatMaterialProperty->SetValue(GetValue());
      }

      void AddInstancedBinding(const std::shared_ptr<PropertyBinding> &propertyBinding,
                               const std::shared_ptr<MaterialInstanceDataProvider> &instanceDataProvider)
      {
         mInstancedBindings.emplace_back(std::make_pair(propertyBinding, instanceDataProvider));
      }

      std::vector<float> GetValue()
      {
         std::vector<float> result;
         result.reserve(mInstancedBindings.size());
         std::sort(mInstancedBindings.begin(), mInstancedBindings.end(), [](const auto& pairLeft, const auto& pairRight) {
            return pairLeft.second->GetInstanceId() < pairRight.second->GetInstanceId();
         });
         for (const auto &[binding, instanceData] : mInstancedBindings)
         {
            if (instanceData->IsInstanceActive() && instanceData->GetInstanceId() >= 0)
            {
               const auto &floatBinding = std::static_pointer_cast<FloatPropertyBinding>(binding);
               result.emplace_back(floatBinding->GetValue());
            }
         }
         return result;
      }
   };
}
