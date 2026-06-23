#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/InstancedStaticMeshMaterialDataProvider.h"
#include "PrimitiveComponent.h"

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore {
struct InstancedMeshComponentData;

class InstancedStaticMeshComponent : public PrimitiveComponent, public InstancedStaticMeshMaterialDataProvider {
protected:
    MeshRenderData m_renderData;

public:
    InstancedStaticMeshComponent(
        const std::shared_ptr<InstancedMeshComponentData>& meshComponentData, const MeshRenderData renderData);

    ~InstancedStaticMeshComponent() override;

    void OnPostRegistered() override;

    eComponentType GetComponentType() const override;

    void SetIsEnabled(const bool bEnabled) override;

    void SetIsVisible(bool isVisible) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    inline const MeshRenderData& GetRenderData() const
    {
        return m_renderData;
    }

    std::shared_ptr<IMaterial> GetMaterial() const;

    BoundingBox3D GetTransformedBoundingBox() const override;

    // override of InstancedStaticMeshMaterialDataProvider methods

    std::string GetBatchKey() const override;

    std::weak_ptr<::EngineCore::Scene> GetSceneWp() const override;

    int32_t GetInstanceObjectId() const override;

    int32_t GetInstanceProxyId() const override;
};

} // namespace EngineCore
