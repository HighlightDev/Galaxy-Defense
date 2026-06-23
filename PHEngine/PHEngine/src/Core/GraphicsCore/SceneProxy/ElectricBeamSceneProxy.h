#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/BeamComponentBase.h"
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "PrimitiveSceneProxy.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <chrono>
#include <memory>
#include <tuple>
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

public:
    using BeamMesh = std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>;
    using BeamFrame = std::vector<BeamMesh>; // one mesh per beam, for a single animation frame

protected:
    RuntimeGeneratedMeshPoolParameters mBeamMeshPoolParams;

    // One GPU buffer per pre-baked animation frame. The proxy cycles through them on its own; no new
    // geometry is streamed from the game thread once these are uploaded.
    std::vector<std::shared_ptr<Skin>> mBeamSkins;

    int mBeamCount;
    int mRadialSegments;
    int mLengthSegments;

    // Beam placement: set per tick by DynamicBeamComponent to map its canonical unit beam onto the live
    // [start,end] segment. Identity until the first update.
    glm::mat4 mBeamWorldMatrix{glm::mat4(1.0f)};

    // Frame cycling state (driven by the render-thread wall clock).
    int mCurrentFrame{0};
    float mFrameStepSeconds{0.05f};
    float mFrameAccumulatorSeconds{0.0f};
    std::chrono::steady_clock::time_point mLastAdvanceTime;
    bool mHasLastAdvanceTime{false};

public:
    explicit ElectricBeamSceneProxy(const BeamComponentBase* component);

    void PostConstructorInitialize() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    // Receives the full pre-baked frame set (game thread → render thread, once / on param change).
    // Allocates one GPU buffer per frame and uploads the flattened geometry.
    void SetAnimationFrames(const std::vector<BeamFrame>& frames, int beamCount, float frameStepSeconds);

    // Updates only the beam placement (cheap; sent every tick by DynamicBeamComponent for moving beams).
    void SetBeamWorldMatrix(const glm::mat4& beamWorldMatrix);

    bool IsFrustumCullTestNeeded() const override;
    RenderInfo GetRenderInfo() const override;
    bool IsDeferred() const override;

    void CleanUp() override;

protected:
    void AllocateBeamSkins(int frameCount, size_t verticesPerFrame, size_t indicesPerFrame);
    void UploadFrame(int frameIndex, const BeamFrame& frame);
    void DestroyBeamSkins();
    void AdvanceFrame();

    std::shared_ptr<ShaderType> GetShader() const;
};

} // namespace Proxy
} // namespace Graphics
