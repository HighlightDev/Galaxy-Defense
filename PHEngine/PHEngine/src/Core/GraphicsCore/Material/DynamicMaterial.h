#pragma once
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicFloatMaterialProperty.h"
#include "IMaterial.h"

namespace EngineCore {
class Scene;
}

namespace Graphics {
class DynamicMaterial : public IMaterial, public ITickable {
    std::vector<std::shared_ptr<MaterialProperty>> mDirtyProperties;

    bool mIsEnabled;

protected:
    std::vector<std::shared_ptr<DynamicMaterialProperty>> mDynamicProperties;

    std::weak_ptr<EngineCore::Scene> mScene;

public:
    DynamicMaterial(const std::string& materialName, const std::string& materialShaderName);

    ~DynamicMaterial() override;

    void CleanUp() override;

    eMaterialType GetMaterialType() const override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override { };

    void SetIsEnabled(const bool bIsEnabled) override;

    bool IsEnabled() const override;

    void SetScene(std::weak_ptr<EngineCore::Scene> scene);

    void SyncDataWithRenderThread();

    void PushDynamicProperty(const std::shared_ptr<DynamicMaterialProperty>& dynamicProperty);

    std::shared_ptr<DynamicMaterialProperty> TryGetDynamicPropertyByName(const std::string& propertyName) const;

    std::shared_ptr<MaterialProperty> TryGetAnyMaterialPropertyByName(const std::string& propertyName) const;
};

} // namespace Graphics
