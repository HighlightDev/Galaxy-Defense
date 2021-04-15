#include "DynamicMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace Game;

namespace Graphics
{
   DynamicMaterial::DynamicMaterial(const std::string& materialName, const std::string& materialShaderName)
      : IMaterial(materialName, materialShaderName)
   {
   }

   DynamicMaterial::~DynamicMaterial()
   {
   }

   void DynamicMaterial::SetScene(std::weak_ptr<Scene> scene)
   {
      mScene = scene;
   }

   IMaterial::eMaterialType DynamicMaterial::GetMaterialType() const {
      return IMaterial::eMaterialType::DYNAMIC;
   }

   void DynamicMaterial::SyncDataWithRenderThread() {

      if (auto sceneSP = mScene.lock())
      {
         sceneSP->MaterialPropertiesUpdated_OnRenderThread(MaterialProxyId, mDirtyProperties);
      }
   }

   void DynamicMaterial::Tick(const float deltaTime)
   {
      for (auto dynProp : mDynamicProperties)
      {
         // Get proxy of dynamic property
         auto proxyProperty = GetMaterialPropertyByName(dynProp->GetPropertyName());
         assert(proxyProperty);

         auto floatProperty = std::static_pointer_cast<FloatMaterialProperty>(proxyProperty);
         const float value = dynProp->GetValue();
         floatProperty->SetValue(value);
         mDirtyProperties.push_back(floatProperty);
      }

      if (mDirtyProperties.size())
      {
         SyncDataWithRenderThread();
      }

      mDirtyProperties.clear();
   }

   void DynamicMaterial::PushDynamicProperty(std::shared_ptr<DynamicFloatMaterialProperty> dynamicProperty) {

      const auto& propertyName = dynamicProperty->GetPropertyName();

      auto propertyIt = std::find_if(mDynamicProperties.begin(), mDynamicProperties.end(),
         [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });
      assert(propertyIt == mDynamicProperties.end());

      // Add proxy of dynamic property 
      std::shared_ptr<FloatMaterialProperty> proxyProperty = std::make_shared<FloatMaterialProperty>(propertyName);
      PushMaterialProperty(proxyProperty);

      mDynamicProperties.emplace_back(std::move(dynamicProperty));
   }

   std::shared_ptr<MaterialProperty> DynamicMaterial::TryGetAnyMaterialPropertyByName(const std::string& propertyName) const
   {
      std::shared_ptr<MaterialProperty> result = nullptr;

      for (const auto& dynamicProperty : mDynamicProperties) {
         auto property = dynamicProperty->TryGetInternalMaterialPropertyByName(propertyName);
         if (property)
         {
            result = property;
            break;
         }
      }

      if (!result)
      {
         result = GetMaterialPropertyByName(propertyName);
      }

      return result;
   }

   std::shared_ptr<DynamicFloatMaterialProperty> DynamicMaterial::TryGetDynamicPropertyByName(const std::string& propertyName) const
   {
      auto propertyIt = std::find_if(mDynamicProperties.begin(), mDynamicProperties.end(),
         [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });
      if (propertyIt != mDynamicProperties.end()) {
         return *propertyIt;
      }

      return nullptr;
   }
}