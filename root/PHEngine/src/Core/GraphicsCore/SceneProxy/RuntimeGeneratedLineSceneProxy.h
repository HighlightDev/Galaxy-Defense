#pragma once
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "StaticMeshSceneProxy.h"

#include <glm/vec3.hpp>

using namespace Resources;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class RuntimeGeneratedLineSceneProxy : public StaticMeshSceneProxy {
    using Base = StaticMeshSceneProxy;
    using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;

protected:
    RuntimeGeneratedMeshPoolParameters mRtMeshPoolParams;

    glm::vec3 mLineBeginWorldSpacePosition;

    glm::vec3 mLineEndWorldSpacePosition;

    float mLineWidth;

    bool bUpdateLineGeometry{true};

    uint32_t mVerticesCountToRender{4};

public:
    RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent* component);

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    void SetLineBeginWorldSpacePosition(const glm::vec3& position);

    void SetLineEndWorldSpacePosition(const glm::vec3& position);

    void SetLineWidth(const float lineWidth);

    bool IsFrustumCullTestNeeded() const override;

    RenderInfo GetRenderInfo() const override;

protected:
    virtual void UpdateGeometry(const glm::mat4& viewMatrix);
};

} // namespace Proxy
} // namespace Graphics
