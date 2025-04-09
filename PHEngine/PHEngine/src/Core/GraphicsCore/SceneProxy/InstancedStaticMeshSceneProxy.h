#pragma once
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "PrimitiveSceneProxy.h"

using namespace Graphics;
using namespace Graphics::Data;
using namespace EngineCore;

namespace Graphics::GeometryBatching {
class InstancedGeometryBatchProxy;
}

namespace Graphics {
namespace Proxy {

class InstancedStaticMeshSceneProxy : public PrimitiveSceneProxy,
                                      public std::enable_shared_from_this<InstancedStaticMeshSceneProxy>

{
    using Base = PrimitiveSceneProxy;

    std::shared_ptr<Graphics::GeometryBatching::InstancedGeometryBatchProxy> mBatchProxy;

protected:
    MeshRenderData m_renderData;

public:
    InstancedStaticMeshSceneProxy(const InstancedStaticMeshComponent* component);

    ~InstancedStaticMeshSceneProxy() override;

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ::Graphics::ActiveBindedState& activeBindedState) override;

    void RenderPlanarReflection(
        const glm::vec4& plane,
        const glm::mat4& mirrorMatrix,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ::Graphics::ActiveBindedState& activeBindedState) override;

    bool IsDeferred() const override;

    bool IsFrustumCullTestNeeded() const override;

    ePrimitiveProxyType GetPrimitiveProxyType() const override;

    MeshRenderData GetRenderData() const;

    std::string GetBatchKey() const;

    RenderInfo GetRenderInfo() const override;
};

} // namespace Proxy
} // namespace Graphics
