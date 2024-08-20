#include "DynamicMaterial.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/iVec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec3MaterialProperty.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace EngineCore;
using namespace Graphics::Renderer;

namespace Graphics
{
   DynamicMaterial::DynamicMaterial(const std::string &materialName, const std::string &materialShaderName)
       : IMaterial(materialName, materialShaderName),
         mIsEnabled(true)
   {
   }

   DynamicMaterial::~DynamicMaterial()
   {
   }

   void DynamicMaterial::CleanUp()
   {
      IMaterial::CleanUp();

      mDirtyProperties.clear();
      mDynamicProperties.clear();
   }

   void DynamicMaterial::SetScene(std::weak_ptr<Scene> scene)
   {
      mScene = scene;
   }

   IMaterial::eMaterialType DynamicMaterial::GetMaterialType() const
   {
      return IMaterial::eMaterialType::DYNAMIC;
   }

   void DynamicMaterial::SetIsEnabled(const bool bIsEnabled)
   {
      if (mIsEnabled != bIsEnabled)
      {
         mIsEnabled = bIsEnabled;
      }
   }

   bool DynamicMaterial::IsEnabled() const
   {
      return mIsEnabled;
   }

   void DynamicMaterial::SyncDataWithRenderThread()
   {
      if (const auto &sceneSp = mScene.lock())
      {
         if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            sceneRendererSp->MaterialPropertiesUpdated_OnRenderThread(MaterialProxyId, std::move(mDirtyProperties));
         }
      }
   }

   void DynamicMaterial::Tick(const float deltaTime)
   {
      for (auto dynProp : mDynamicProperties)
      {
         const auto staticProperty = GetMaterialPropertyByName(dynProp->GetPropertyName());
         assert(staticProperty);
         dynProp->UpdateStaticPropertyWithDynamicValue(staticProperty);
         mDirtyProperties.push_back(staticProperty);
      }

      if (mDirtyProperties.size())
      {
         SyncDataWithRenderThread();
         mDirtyProperties.clear();
      }
   }

   void DynamicMaterial::PushDynamicProperty(const std::shared_ptr<DynamicMaterialProperty> &dynamicProperty)
   {
      const auto &propertyName = dynamicProperty->GetPropertyName();

      const bool alreadyExistsWithSuchName = std::any_of(mDynamicProperties.begin(), mDynamicProperties.end(),
                                                         [&](const auto &dynamicProperty)
                                                         { return propertyName == dynamicProperty->GetPropertyName(); });
      assert(!alreadyExistsWithSuchName);

      // Add static version of dynamic property
      std::shared_ptr<MaterialProperty> staticProperty;
      if (dynamicProperty->GetPropertyType() == DynamicMaterialProperty::eDynamicMaterialPropertyType::FloatProperty)
      {
         staticProperty = std::make_shared<FloatMaterialProperty>(propertyName);
      }
      else if (dynamicProperty->GetPropertyType() == DynamicMaterialProperty::eDynamicMaterialPropertyType::iVec2Property)
      {
         staticProperty = std::make_shared<iVec2MaterialProperty>(propertyName);
      }
      else if (dynamicProperty->GetPropertyType() == DynamicMaterialProperty::eDynamicMaterialPropertyType::Vec2Property)
      {
         staticProperty = std::make_shared<Vec2MaterialProperty>(propertyName);
      }
      else if (dynamicProperty->GetPropertyType() == DynamicMaterialProperty::eDynamicMaterialPropertyType::Vec3Property)
      {
         staticProperty = std::make_shared<Vec3MaterialProperty>(propertyName);
      }

      assert(staticProperty);
      PushMaterialProperty(staticProperty);

      mDynamicProperties.emplace_back(dynamicProperty);
   }

   std::shared_ptr<MaterialProperty> DynamicMaterial::TryGetAnyMaterialPropertyByName(const std::string &propertyName) const
   {
      std::shared_ptr<MaterialProperty> result = nullptr;

      for (const auto &dynamicProperty : mDynamicProperties)
      {
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

   std::shared_ptr<DynamicMaterialProperty> DynamicMaterial::TryGetDynamicPropertyByName(const std::string &propertyName) const
   {
      auto propertyIt = std::find_if(mDynamicProperties.begin(), mDynamicProperties.end(),
                                     [&](const auto &dynamicProperty)
                                     { return propertyName == dynamicProperty->GetPropertyName(); });
      if (propertyIt != mDynamicProperties.end())
      {
         return *propertyIt;
      }

      return nullptr;
   }
}