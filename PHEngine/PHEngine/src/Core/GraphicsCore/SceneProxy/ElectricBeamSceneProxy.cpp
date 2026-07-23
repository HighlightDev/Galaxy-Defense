#include "ElectricBeamSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <string>

using namespace Resources;
using namespace Graphics;
using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Renderer;

namespace Graphics {
namespace Proxy {

ElectricBeamSceneProxy::ElectricBeamSceneProxy(const BeamComponentBase* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mBeamMeshPoolParams(component->GetRuntimeGeneratedMeshPoolParameters())
    , mBeamCount(component->GetBeamCount())
    , mRadialSegments(component->GetRadialSegments())
    , mLengthSegments(component->GetLengthSegments())
{
}

void ElectricBeamSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("ElectricBeamSceneProxy::PostConstructorInitialize");

    ShaderParams shaderParams("ElectricBeam_BaseShader");
    shaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("runtimeGeneratedMeshVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes("forwardNoLitFS.glsl"));

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
        "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    ShaderParams planarReflectionParams("PlanarReflectionShader");
    planarReflectionParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("planarReflectionVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes("forwardNoLitFS.glsl"));

    m_planarReflectionShader = CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(
        "StaticMeshVertexFactory_CapturePlanarReflectionShader_" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);

    // Skins are allocated lazily once the pre-baked frame set arrives (SetAnimationFrames), since the
    // frame count is only known then.
}

void ElectricBeamSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (mBeamSkins.empty()) {
        return; // frame set not uploaded yet
    }

    AdvanceFrame();

    const auto& shader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }

    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    // Static beams: mBeamWorldMatrix is identity, so this is just m_worldMatrix (world-space geometry).
    // Dynamic beams: m_worldMatrix is identity and mBeamWorldMatrix places the canonical unit beam.
    shader->GetVertexFactoryShader()->SetMatrices(m_worldMatrix * mBeamWorldMatrix, viewMatrix, projectionMatrix);

    // Render the currently selected pre-baked frame.
    if (mCurrentFrame >= 0 && mCurrentFrame < static_cast<int>(mBeamSkins.size()) && mBeamSkins[mCurrentFrame]) {
        mBeamSkins[mCurrentFrame]->GetBuffer()->RenderVAO(GL_TRIANGLES);
    }
}

bool ElectricBeamSceneProxy::IsDeferred() const
{
    return false;
}

void ElectricBeamSceneProxy::SetAnimationFrames(const std::vector<BeamFrame>& frames, int beamCount, float frameStepSeconds)
{
    mBeamCount = beamCount > 0 ? beamCount : mBeamCount;
    mFrameStepSeconds = frameStepSeconds > 0.0f ? frameStepSeconds : 0.05f;

    const int frameCount = static_cast<int>(frames.size());
    if (frameCount == 0) {
        DestroyBeamSkins();
        return;
    }

    // Topology is identical across frames, so size each GPU buffer from a representative frame rather
    // than from (potentially stale) radial/length segment counts.
    size_t verticesPerFrame = 0, indicesPerFrame = 0;
    for (const auto& beamMesh : frames.front()) {
        verticesPerFrame += std::get<0>(beamMesh).size();
        indicesPerFrame += std::get<1>(beamMesh).size();
    }

    // A moving beam re-bakes (and resends) its frames every tick. Only (re)allocate the GPU buffers when
    // the count or per-frame capacity actually changes — otherwise reuse them and just re-upload, so the
    // common per-tick path is a BufferSubData, not buffer churn.
    const bool needRealloc = static_cast<int>(mBeamSkins.size()) != frameCount
        || mBeamMeshPoolParams.mMaxVerticesCount != verticesPerFrame || mBeamMeshPoolParams.mMaxIndicesCount != indicesPerFrame;
    if (needRealloc) {
        AllocateBeamSkins(frameCount, verticesPerFrame, indicesPerFrame);
        mCurrentFrame = 0;
        mFrameAccumulatorSeconds = 0.0f;
        mHasLastAdvanceTime = false;
    }

    for (int frame = 0; frame < frameCount; ++frame) {
        UploadFrame(frame, frames[frame]);
    }
}

void ElectricBeamSceneProxy::SetBeamWorldMatrix(const glm::mat4& beamWorldMatrix)
{
    mBeamWorldMatrix = beamWorldMatrix;
}

bool ElectricBeamSceneProxy::IsFrustumCullTestNeeded() const
{
    return false; // Electric beams are typically thin and might be culled incorrectly
}

std::shared_ptr<ElectricBeamSceneProxy::ShaderType> ElectricBeamSceneProxy::GetShader() const
{
    return std::static_pointer_cast<ElectricBeamSceneProxy::ShaderType>(m_shader);
}

RenderInfo ElectricBeamSceneProxy::GetRenderInfo() const
{
    RenderInfo renderInfo;
    renderInfo.mShaderName = GetShader()->GetShaderName();
    return renderInfo;
}

void ElectricBeamSceneProxy::UploadFrame(int frameIndex, const BeamFrame& frame)
{
    if (frameIndex < 0 || frameIndex >= static_cast<int>(mBeamSkins.size()) || !mBeamSkins[frameIndex]) {
        return;
    }

    // Single interleaved vertex stream: [pos.xyz, normal.xyz, tex.uv] per vertex — matches the layout the
    // allocation policy set up on the CompositeVertexBufferObject.
    constexpr size_t c_floatsPerVertex = 3 + 3 + 2;
    std::vector<float> interleavedData;
    std::vector<uint32_t> totalIndices;
    interleavedData.reserve(mBeamMeshPoolParams.mMaxVerticesCount * c_floatsPerVertex);
    totalIndices.reserve(mBeamMeshPoolParams.mMaxIndicesCount);
    const uint32_t verticesPerBeam = mBeamCount > 0 ? mBeamMeshPoolParams.mMaxVerticesCount / mBeamCount : 0;

    const int beamsInFrame = std::min(static_cast<int>(frame.size()), mBeamCount);
    for (int i = 0; i < beamsInFrame; ++i) {
        const auto& vertices = std::get<0>(frame[i]);
        const auto& indices = std::get<1>(frame[i]);

        for (const auto& vertex : vertices) {
            interleavedData.push_back(vertex.Position.x);
            interleavedData.push_back(vertex.Position.y);
            interleavedData.push_back(vertex.Position.z);
            interleavedData.push_back(vertex.Normal.x);
            interleavedData.push_back(vertex.Normal.y);
            interleavedData.push_back(vertex.Normal.z);
            interleavedData.push_back(vertex.TexCoord.x);
            interleavedData.push_back(vertex.TexCoord.y);
        }

        for (const auto& index : indices) {
            totalIndices.push_back(index + (i * verticesPerBeam));
        }
    }

    auto& buffer = mBeamSkins[frameIndex]->GetBuffer();
    const auto& interleavedVBO = buffer->GetVboByAttribArrayIndexName("VertexPosition"); // the single composite VBO
    const auto& ibo = buffer->GetIBO();

    interleavedVBO->BufferSubData(0, interleavedData.size() * sizeof(float), interleavedData.data());
    interleavedVBO->UnbindBuffer();
    ibo->BufferSubData(0, totalIndices.size() * sizeof(uint32_t), totalIndices.data());
    ibo->UnbindBuffer();
}

void ElectricBeamSceneProxy::AllocateBeamSkins(int frameCount, size_t verticesPerFrame, size_t indicesPerFrame)
{
    DestroyBeamSkins();

    const auto vertexAttributes = GetShader()->GetVertexAttributes();
    const std::string baseName = mBeamMeshPoolParams.mComponentName;

    mBeamSkins.reserve(frameCount);
    for (int frame = 0; frame < frameCount; ++frame) {
        // One distinct pool key per frame (the pool dedups by {name, vtxCount, idxCount}), so each frame
        // gets its own GPU buffer rather than aliasing a single shared one.
        RuntimeGeneratedMeshPoolParameters frameParams(baseName + "_frame" + std::to_string(frame));
        frameParams.mVertexAttributes = vertexAttributes;
        frameParams.mMaxVerticesCount = verticesPerFrame;
        frameParams.mMaxIndicesCount = indicesPerFrame;
        frameParams.mUseInterleavedBuffer = true; // all attributes in one interleaved CompositeVertexBufferObject
        mBeamSkins.push_back(RuntimeGeneratedMeshPool::GetInstance()->GetOrAllocateResource(frameParams));
    }

    // Keep the shared params reflecting the per-frame capacity used by UploadFrame's reserves/offsets.
    mBeamMeshPoolParams.mVertexAttributes = vertexAttributes;
    mBeamMeshPoolParams.mMaxVerticesCount = verticesPerFrame;
    mBeamMeshPoolParams.mMaxIndicesCount = indicesPerFrame;
}

void ElectricBeamSceneProxy::DestroyBeamSkins()
{
    for (auto& skin : mBeamSkins) {
        if (skin) {
            RuntimeGeneratedMeshPool::GetInstance()->TryToFreeMemory(skin);
        }
    }
    mBeamSkins.clear();
}

void ElectricBeamSceneProxy::AdvanceFrame()
{
    const int frameCount = static_cast<int>(mBeamSkins.size());
    if (frameCount <= 1 || mFrameStepSeconds <= 0.0f) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (!mHasLastAdvanceTime) {
        mLastAdvanceTime = now;
        mHasLastAdvanceTime = true;
        return;
    }

    const float deltaSeconds = std::chrono::duration<float>(now - mLastAdvanceTime).count();
    mLastAdvanceTime = now;
    mFrameAccumulatorSeconds += deltaSeconds;

    // Catch up if the render thread stalled, but cap the work to one full loop.
    int advance = static_cast<int>(mFrameAccumulatorSeconds / mFrameStepSeconds);
    if (advance > 0) {
        mFrameAccumulatorSeconds -= advance * mFrameStepSeconds;
        mCurrentFrame = (mCurrentFrame + advance) % frameCount;
    }
}

void ElectricBeamSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();

    DestroyBeamSkins();
}

} // namespace Proxy
} // namespace Graphics
