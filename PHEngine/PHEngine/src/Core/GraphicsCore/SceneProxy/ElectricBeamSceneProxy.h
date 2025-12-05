#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "PrimitiveSceneProxy.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics::Mesh {
class Skin;
}

namespace Graphics {
namespace Proxy {

class ElectricBeamSceneProxy : public PrimitiveSceneProxy {
    using Base = PrimitiveSceneProxy;
    using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;
    using PlanarReflectionShaderType
        = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;

protected:
    std::vector<RuntimeGeneratedMeshPoolParameters> mBeamMeshPoolParams;
    std::vector<std::shared_ptr<Skin>> mBeamSkins;

    glm::vec3 mBeamColor;
    int mBeamCount;
    BeamRenderMode mRenderMode;
    int mRadialSegments;
    int mLengthSegments;

    bool bUpdateBeamGeometry{true};

    std::vector<std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>> mMeshData;

public:
    explicit ElectricBeamSceneProxy(const ElectricBeamComponent* component);

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    void SetBeamColor(const glm::vec3& color);
    void SetBeamCount(int count);
    void SetRenderMode(BeamRenderMode mode);
    void SetMeshData(const std::vector<std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>>& meshData);

    bool IsFrustumCullTestNeeded() const override;
    RenderInfo GetRenderInfo() const override;
    bool IsDeferred() const override;

protected:
    void UpdateBeamGeometry(int beamIndex);
    void CreateBeamSkins();
    void DestroyBeamSkins();

    std::shared_ptr<ShaderType> GetShader() const;
};

} // namespace Proxy
} // namespace Graphics
