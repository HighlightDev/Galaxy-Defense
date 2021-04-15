#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicFloatMaterialProperty.h"

namespace Game {
   class Scene;
}

namespace Graphics
{
   class DynamicMaterial 
      : public IMaterial
      , public ITickable
   {
   protected:

      using DynanamicPropertySP_t = std::shared_ptr<DynamicFloatMaterialProperty>;

      std::vector<DynanamicPropertySP_t> mDynamicProperties;
      std::vector<std::shared_ptr<MaterialProperty>> mDirtyProperties;

      std::weak_ptr<Game::Scene> mScene;

   public:

      DynamicMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~DynamicMaterial();

      virtual eMaterialType GetMaterialType() const override;

      virtual void Tick(const float deltaTime);

      void SetScene(std::weak_ptr<Game::Scene> scene);

      void SyncDataWithRenderThread();

      void PushDynamicProperty(std::shared_ptr<DynamicFloatMaterialProperty> dynamicProperty);

      std::shared_ptr<DynamicFloatMaterialProperty> TryGetDynamicPropertyByName(const std::string& propertyName) const;

      std::shared_ptr<MaterialProperty> TryGetAnyMaterialPropertyByName(const std::string& propertyName) const;
      
   };

}

