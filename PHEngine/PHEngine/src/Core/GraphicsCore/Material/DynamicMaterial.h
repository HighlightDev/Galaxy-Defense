#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicFloatMaterialProperty.h"

namespace EngineCore
{
   class Scene;
}

namespace Graphics
{
   class DynamicMaterial
       : public IMaterial,
         public ITickable
   {
      std::vector<std::shared_ptr<MaterialProperty>> mDirtyProperties;

      bool mIsEnabled;

   protected:
      std::vector<std::shared_ptr<DynamicFloatMaterialProperty>> mDynamicProperties;

      std::weak_ptr<EngineCore::Scene> mScene;

   public:
      DynamicMaterial(const std::string &materialName, const std::string &materialShaderName);

      virtual ~DynamicMaterial();

      virtual eMaterialType GetMaterialType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void UnpausableTick(const float deltaTime) override {};

      virtual void SetIsEnabled(const bool bIsEnabled) override;

      void SetScene(std::weak_ptr<EngineCore::Scene> scene);

      void SyncDataWithRenderThread();

      void PushDynamicProperty(std::shared_ptr<DynamicFloatMaterialProperty> dynamicProperty);

      std::shared_ptr<DynamicFloatMaterialProperty> TryGetDynamicPropertyByName(const std::string &propertyName) const;

      std::shared_ptr<MaterialProperty> TryGetAnyMaterialPropertyByName(const std::string &propertyName) const;
   };

}
