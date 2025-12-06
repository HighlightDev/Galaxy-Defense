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

ElectricBeamComponent::ElectricBeamComponent(const std::string& gameObjectName, const MeshRenderData& renderData)
    : PrimitiveComponent(gameObjectName, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f))
    , mBeamMeshes()
    , mStartPoint(glm::vec3(0.0f))
    , mEndPoint(glm::vec3(10.0f, 0.0f, 0.0f))
    , mBeamColor(glm::vec3(0.3f, 0.5f, 1.0f))
    , mBeamThickness(2.0f)
    , mBeamCount(3)
    , mJitterAmount(0.2f)
    , mUpdateFrequency(0.05f)
    , mRenderMode(BeamRenderMode::ProceduralElectric)
    , mRadialSegments(8)
    , mLengthSegments(10)
    , mAnimationTime(0.0f)
    , mAnimationSpeed(2.0f)
    , mTimeSinceLastUpdate(0.0f)
    , mRenderData(renderData)
{
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

    // Update animation time continuously for smooth procedural animation
    mAnimationTime += deltaTime;

    mTimeSinceLastUpdate += deltaTime;

    if (not EngineMath::FloatsNearEqual(mUpdateFrequency, 0.0f) && mTimeSinceLastUpdate >= mUpdateFrequency) {
        RegenerateBeams();
        mTimeSinceLastUpdate = 0.0f;
    }

    if (mIsRenderDataDirty) {
        SyncRenderData();
        mIsRenderDataDirty = false;
    }
}

void ElectricBeamComponent::OnRegistered()
{
    LogInfo("ElectricBeamComponent::OnRegistered: ", GetEngineObjectName());

    CreateBeamMeshes();

    RegenerateBeams();
}

void ElectricBeamComponent::OnUnregistered()
{
    LogInfo("ElectricBeamComponent::OnUnregistered: ", GetEngineObjectName());

    DestroyBeamMeshes();
}

void ElectricBeamComponent::SetStartPoint(const glm::vec3& point)
{
    mStartPoint = point;
    mIsRenderDataDirty = true;
    RegenerateBeams();
}

void ElectricBeamComponent::SetEndPoint(const glm::vec3& point)
{
    mEndPoint = point;
    mIsRenderDataDirty = true;
    RegenerateBeams();
}

void ElectricBeamComponent::SetBeamColor(const glm::vec3& color)
{
    mBeamColor = color;
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetBeamThickness(float thickness)
{
    mBeamThickness = thickness;
    mIsRenderDataDirty = true;
}

void ElectricBeamComponent::SetBeamCount(int count)
{
    if (count < 1)
        count = 1;
    if (count > 20)
        count = 20; // Reasonable limit

    mBeamCount = count;
    mIsRenderDataDirty = true;

    DestroyBeamMeshes();
    CreateBeamMeshes();

    RegenerateBeams();
}

void ElectricBeamComponent::SetJitterAmount(float amount)
{
    mJitterAmount = amount;
}

void ElectricBeamComponent::SetUpdateFrequency(float frequency)
{
    mUpdateFrequency = frequency;
}

glm::vec3 ElectricBeamComponent::GetStartPoint() const
{
    return mStartPoint;
}

glm::vec3 ElectricBeamComponent::GetEndPoint() const
{
    return mEndPoint;
}

glm::vec3 ElectricBeamComponent::GetBeamColor() const
{
    return mBeamColor;
}

float ElectricBeamComponent::GetBeamThickness() const
{
    return mBeamThickness;
}

int ElectricBeamComponent::GetBeamCount() const
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
    for (int i = 0; i < static_cast<int>(mBeamMeshes.size()); ++i) {
        UpdateBeamMesh(i);
    }
}

glm::vec3 ElectricBeamComponent::GetJitteredPoint(const glm::vec3& basePoint, float jitterScale) const
{
    const float jitter1 = Random::Float() * 2.0f - 1.0f;
    const float jitter2 = Random::Float() * 2.0f - 1.0f;
    const float jitter3 = Random::Float() * 2.0f - 1.0f;

    const float jitter = mJitterAmount * jitterScale;

    return basePoint + glm::vec3(jitter1 * jitter, jitter2 * jitter, jitter3 * jitter);
}

void ElectricBeamComponent::SetRenderMode(BeamRenderMode mode)
{
    if (mRenderMode == mode) {
        return;
    }

    DestroyBeamMeshes();

    mRenderMode = mode;

    CreateBeamMeshes();

    RegenerateBeams();

    LogInfo("ElectricBeamComponent::SetRenderMode: Changed to ", static_cast<int>(mode));
}

void ElectricBeamComponent::SetGeometrySegments(int radialSegments, int lengthSegments)
{
    if (radialSegments < 3)
        radialSegments = 3;
    if (lengthSegments < 2)
        lengthSegments = 2;

    mRadialSegments = radialSegments;

    mLengthSegments = lengthSegments;
    // Recreate meshes with new segment count
    DestroyBeamMeshes();
    CreateBeamMeshes();
    RegenerateBeams();
}

BeamRenderMode ElectricBeamComponent::GetRenderMode() const
{
    return mRenderMode;
}

void ElectricBeamComponent::SetAnimationSpeed(float speed)
{
    if (speed < 0.0f)
        speed = 0.0f;
    mAnimationSpeed = speed;
    LogInfo("ElectricBeamComponent::SetAnimationSpeed: ", speed);
}

float ElectricBeamComponent::GetAnimationSpeed() const
{
    return mAnimationSpeed;
}

int ElectricBeamComponent::GetRadialSegments() const
{
    return mRadialSegments;
}

int ElectricBeamComponent::GetLengthSegments() const
{
    return mLengthSegments;
}

void ElectricBeamComponent::CreateBeamMeshes()
{
    mBeamMeshes.clear();
    mBeamMeshes.reserve(mBeamCount);

    for (int i = 0; i < mBeamCount; ++i) {
        mBeamMeshes.emplace_back(std::vector<BeamVertex>(), std::vector<uint32_t>());
    }

    LogInfo("ElectricBeamComponent::CreateBeamMeshes: Created ", mBeamCount, " beam meshes");
}

void ElectricBeamComponent::DestroyBeamMeshes()
{
    mBeamMeshes.clear();
    LogInfo("ElectricBeamComponent::DestroyBeamMeshes: Destroyed all beam meshes");
}

void ElectricBeamComponent::UpdateBeamMesh(int beamIndex)
{
    if (beamIndex < 0 || beamIndex >= static_cast<int>(mBeamMeshes.size())) {
        return;
    }

    const glm::vec3 startPoint = mStartPoint;
    const glm::vec3 endPoint = mEndPoint;
    const float radius = mBeamThickness * 0.1f; // Convert thickness to radius

    std::vector<BeamVertex> vertices;
    std::vector<uint32_t> indices;

    if (mRenderMode == BeamRenderMode::ProceduralMesh) {
        // Simple cylindrical beam with animated jitter
        glm::vec3 jitteredStart = GetJitteredPoint(startPoint, 0.3f);
        glm::vec3 jitteredEnd = GetJitteredPoint(endPoint, 0.3f);

        ProceduralBeamGeometry::GenerateBeamGeometry(jitteredStart, jitteredEnd, radius, mRadialSegments, vertices, indices);
    } else if (mRenderMode == BeamRenderMode::ProceduralElectric) {
        // Electric beam with animated jittered segments
        glm::vec3 jitteredStart = GetJitteredPoint(startPoint, 0.2f);
        glm::vec3 jitteredEnd = GetJitteredPoint(endPoint, 0.2f);

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
    }

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
            [sceneProxyId = mSceneProxyId,
             beamColor = mBeamColor,
             beamCount = mBeamCount,
             animationSpeed = mAnimationSpeed,
             beamMeshes = mBeamMeshes](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& beamProxySp = std::static_pointer_cast<Graphics::Proxy::ElectricBeamSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        beamProxySp->SetBeamColor(beamColor);
                        beamProxySp->SetBeamCount(beamCount);
                        beamProxySp->SetMeshData(beamMeshes);
                    }
                }
            });
    }
}

} // namespace EngineCore
