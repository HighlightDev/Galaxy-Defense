#pragma once
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "PrimitiveSceneProxy.h"

using namespace Graphics::Data;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {

class StaticMeshSceneProxy : public PrimitiveSceneProxy {
    using Base = PrimitiveSceneProxy;
    using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;
    using PlanarReflectionShaderType
        = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;
    using OutlineShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;

protected:
    MeshRenderData m_renderData;

    std::shared_ptr<MaterialProxy> mOutlineMaterialProxy;

protected:
    std::shared_ptr<ShaderType> GetShader() const;

    std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

public:
    StaticMeshSceneProxy(const StaticMeshComponent* component);

    ~StaticMeshSceneProxy() override;

    void CleanUp() override;

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    void RenderPlanarReflection(
        const glm::vec4& plane,
        const glm::mat4& mirrorMatrix,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    void RenderOutline(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    virtual void RenderOutlineStencil(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    bool IsDeferred() const override;

    eMeshFacing GetMeshFrontFace() const override;

    ePrimitiveProxyType GetPrimitiveProxyType() const override;

    RenderInfo GetRenderInfo() const override;
};

} // namespace Proxy
} // namespace Graphics
