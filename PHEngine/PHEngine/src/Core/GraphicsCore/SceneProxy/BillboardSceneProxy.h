#pragma once
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"
#include "PrimitiveSceneProxy.h"

#include <functional>

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace Graphics {
namespace Proxy {
class BillboardSceneProxy : public PrimitiveSceneProxy {
    using Base = PrimitiveSceneProxy;

protected:
    using Shader_t = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, BillboardShader>;

    BillboardRenderData mRenderData;

    float mBillboardExtent;

    bool mApplyScreenAspectRatio;

    float mRotationRadians;

    bool mIsFlipped{false};

    std::function<glm::mat4(const glm::mat4&)> mViewMatrixTransformer;

    std::function<glm::mat4(const glm::mat4&)> mProjectionMatrixTransformer;

public:
    BillboardSceneProxy(const BillboardComponent* component);

    ~BillboardSceneProxy() override;

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    bool IsDeferred() const override;

    bool IsFrustumCullTestNeeded() const override;

    eMeshFacing GetMeshFrontFace() const override;

    void SetBillboardExtent(const float extent);

    RenderInfo GetRenderInfo() const override;

    void SetApplyScreenAspectRatio(const bool applyScreenAspectRatio);

    void SetRotationRadians(const float rotationRadians);

    void SetIsFlipped(const bool isFlipped);

protected:
    std::shared_ptr<Shader_t> GetShader() const;
};

} // namespace Proxy
} // namespace Graphics
