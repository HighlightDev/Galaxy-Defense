#pragma once
#include "Core/GameCore/Components/PrimitiveComponents/CubemapComponent.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "PrimitiveSceneProxy.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {

class CubemapSceneProxy : public PrimitiveSceneProxy {

    std::shared_ptr<CubemapShader> m_shaderCubemap;

    TextureAtlasSpaceRequest m_textureObtainer;

protected:
    using Base = PrimitiveSceneProxy;

public:
    CubemapSceneProxy(const CubemapComponent* component);

    ~CubemapSceneProxy() override;

    void CleanUp() override;

    std::shared_ptr<IShader> GetShader() const;

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    virtual bool IsDeferred() const;

    eMeshFacing GetMeshFrontFace() const override;

    RenderInfo GetRenderInfo() const override;
};

} // namespace Proxy
} // namespace Graphics
