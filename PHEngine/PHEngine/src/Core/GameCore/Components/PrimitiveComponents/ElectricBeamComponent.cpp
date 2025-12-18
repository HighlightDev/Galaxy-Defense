#include "ElectricBeamComponent.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/ElectricBeamSceneProxy.h"

#include <random>

using namespace Graphics::Renderer;

namespace EngineCore {

ElectricBeamComponent::ElectricBeamComponent(
    const std::string& gameObjectName,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& beamMeshPoolParams)
    : PrimitiveComponent(gameObjectName, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f))
    , mBeamMeshPoolParams(beamMeshPoolParams)
    , mBeamMeshes()
    , mStartWorldPosition(glm::vec3(0.0f))
    , mEndWorldPosition(glm::vec3(10.0f, 0.0f, 0.0f))
    , mBeamThickness(2.0f)
    , mBeamCount(3)
    , mJitterAmount(0.2f)
    , mUpdateFrequency(0.05f)
    , mRadialSegments(8)
    , mLengthSegments(20)
    , mAnimationTime(0.0f)
    , mAnimationSpeed(2.0f)
    , mTimeSinceLastUpdate(0.0f)
    , mRenderData(renderData)
{
    SetCanBloomBeApplied(true);
}

ElectricBeamComponent::~ElectricBeamComponent()
{
    DestroyBeamMeshes();
}

std::shared_ptr<PrimitiveSceneProxy> ElectricBeamComponent::CreateSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::ElectricBeamSceneProxy>(this);
}

const MeshRenderData& ElectricBeamComponent::GetRenderData() const
{
    return mRenderData;
}

void ElectricBeamComponent::Tick(const float deltaTime)
{
    Component::Tick(deltaTime);

    mAnimationTime += deltaTime;
    mTimeSinceLastUpdate += deltaTime;

    if (not EngineMath::FloatsNearEqual(mUpdateFrequency, 0.0f) && mTimeSinceLastUpdate >= mUpdateFrequency) {
        mIsRenderDataDirty = true;
        mTimeSinceLastUpdate = 0.0f;
    }

    if (mIsRenderDataDirty) {
        RegenerateBeams();
        SyncRenderData();
        mIsRenderDataDirty = false;
    }
}

void ElectricBeamComponent::OnRegistered()
{
    LogInfo("ElectricBeamComponent::OnRegistered: ", GetEngineObjectName());
    CreateBeamMeshes();
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::OnUnregistered()
{
    LogInfo("ElectricBeamComponent::OnUnregistered: ", GetEngineObjectName());
    DestroyBeamMeshes();
}

void ElectricBeamComponent::SetStartWorldPosition(const glm::vec3& point)
{
    mStartWorldPosition = point;
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetEndWorldPosition(const glm::vec3& point)
{
    mEndWorldPosition = point;
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetBeamThickness(float thickness)
{
    mBeamThickness = thickness;
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetBeamCount(const int32_t count)
{
    const int32_t clampedCount = count < c_minBeamsCount ? c_minBeamsCount : (count > c_maxBeamsCount ? c_maxBeamsCount : count);

    mBeamCount = clampedCount;
    mIsRenderDataDirty = true;

    DestroyBeamMeshes();
    CreateBeamMeshes();
}

void ElectricBeamComponent::SetJitterAmount(const float amount)
{
    mJitterAmount = amount;
}

void ElectricBeamComponent::SetUpdateFrequency(const float frequency)
{
    mUpdateFrequency = frequency;
}

glm::vec3 ElectricBeamComponent::GetStartPoint() const
{
    return mStartWorldPosition;
}

glm::vec3 ElectricBeamComponent::GetEndPoint() const
{
    return mEndWorldPosition;
}

float ElectricBeamComponent::GetBeamThickness() const
{
    return mBeamThickness;
}

int32_t ElectricBeamComponent::GetBeamCount() const
{
    return mBeamCount;
}

float ElectricBeamComponent::GetJitterAmount() const
{
    return mJitterAmount;
}

float ElectricBeamComponent::GetUpdateFrequency() const
{
    return mUpdateFrequency;
}

void ElectricBeamComponent::RegenerateBeams()
{
    const float totalLength = glm::length(mEndWorldPosition - mStartWorldPosition);
    if (EngineMath::FloatsNearEqual(totalLength, 0.0f)) {
        return;
    }

    // todo: removed this feature due to missing reallocation of beam mesh buffers on the render thread
    // mLengthSegments = static_cast<int32_t>(totalLength / 1.5f); // Example: one segment per 1.5 units of length

    for (int32_t i = 0; i < static_cast<int32_t>(mBeamMeshes.size()); ++i) {
        UpdateBeamMesh(i);
    }
}

glm::vec3 ElectricBeamComponent::GetJitteredPoint(const glm::vec3& basePoint) const
{
    const float jitter1 = Random::Float() * 2.0f - 1.0f;
    const float jitter2 = Random::Float() * 2.0f - 1.0f;
    const float jitter3 = Random::Float() * 2.0f - 1.0f;

    return basePoint + glm::vec3(jitter1 * mJitterAmount, jitter2 * mJitterAmount, jitter3 * mJitterAmount);
}

void ElectricBeamComponent::SetRadialSegments(const int32_t radialSegments)
{
    mRadialSegments = radialSegments < 3 ? 3 : radialSegments;
    DestroyBeamMeshes();
    CreateBeamMeshes();
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetAnimationSpeed(const float speed)
{
    const float clampedSpeed = speed < 0.0f ? 0.0f : speed;
    mAnimationSpeed = clampedSpeed;
    LogInfo("ElectricBeamComponent::SetAnimationSpeed: ", clampedSpeed);
}

float ElectricBeamComponent::GetAnimationSpeed() const
{
    return mAnimationSpeed;
}

int32_t ElectricBeamComponent::GetRadialSegments() const
{
    return mRadialSegments;
}

int32_t ElectricBeamComponent::GetLengthSegments() const
{
    return mLengthSegments;
}

RuntimeGeneratedMeshPoolParameters ElectricBeamComponent::GetRuntimeGeneratedMeshPoolParameters() const
{
    return mBeamMeshPoolParams;
}

void ElectricBeamComponent::CreateBeamMeshes()
{
    mBeamMeshes.clear();
    mBeamMeshes.reserve(mBeamCount);

    for (int32_t i = 0; i < mBeamCount; ++i) {
        mBeamMeshes.emplace_back(std::vector<BeamVertex>(), std::vector<uint32_t>());
    }

    LogInfo("ElectricBeamComponent::CreateBeamMeshes: Created ", mBeamCount, " beam meshes");
}

void ElectricBeamComponent::DestroyBeamMeshes()
{
    mBeamMeshes.clear();
    LogInfo("ElectricBeamComponent::DestroyBeamMeshes: Destroyed all beam meshes");
}

void ElectricBeamComponent::UpdateBeamMesh(const int32_t beamIndex)
{
    if (beamIndex < 0 || beamIndex >= static_cast<int32_t>(mBeamMeshes.size())) {
        return;
    }

    const glm::vec3 startPoint = mStartWorldPosition;
    const glm::vec3 endPoint = mEndWorldPosition;
    const float radius = mBeamThickness * 0.1f; // Convert thickness to radius

    std::vector<BeamVertex> vertices;
    std::vector<uint32_t> indices;

    // Electric beam with animated jittered segments
    glm::vec3 jitteredStart = GetJitteredPoint(startPoint);
    glm::vec3 jitteredEnd = GetJitteredPoint(endPoint);
    // Use animated version with continuous time parameter
    ProceduralBeamGeometry::GenerateAnimatedElectricBeamGeometry(
        jitteredStart,
        jitteredEnd,
        radius,
        mRadialSegments,
        mLengthSegments,
        mJitterAmount,
        mAnimationTime + beamIndex * 0.5f, // Offset animation time per beam
        mAnimationSpeed,
        vertices,
        indices);

    mBeamMeshes[beamIndex] = std::make_tuple(vertices, indices);

    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SyncRenderData()
{
    static constexpr uint64_t functionId = Hash64_CT("ElectricBeamComponent::SyncRenderData");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [sceneProxyId = mSceneProxyId, beamCount = mBeamCount, animationSpeed = mAnimationSpeed, beamMeshes = mBeamMeshes](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& beamProxySp = std::static_pointer_cast<Graphics::Proxy::ElectricBeamSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        beamProxySp->SetMeshData(beamMeshes);
                    }
                }
            });
    }
}

} // namespace EngineCore
