#include "BeamComponentBase.h"

#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/ElectricBeamComponentLuaProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/ElectricBeamSceneProxy.h"

#include <utility>

using namespace Graphics::Renderer;
using namespace EngineCore::Scripts;

namespace EngineCore {

BeamComponentBase::BeamComponentBase(
    const std::string& gameObjectName,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& beamMeshPoolParams,
    const bool isEnabled,
    const bool isVisible)
    : PrimitiveComponent(gameObjectName, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), isEnabled, isVisible)
    , mBeamMeshPoolParams(beamMeshPoolParams)
    , mAnimationFrames()
    , mStartWorldPosition(glm::vec3(0.0f))
    , mEndWorldPosition(glm::vec3(10.0f, 0.0f, 0.0f))
    , mBeamThickness(2.0f)
    , mBeamCount(3)
    , mJitterAmount(0.2f)
    , mUpdateFrequency(0.05f)
    , mRadialSegments(8)
    , mLengthSegments(20)
    , mAnimationSpeed(2.0f)
    , mRenderData(renderData)
{
    SetCanBloomBeApplied(true);
}

BeamComponentBase::~BeamComponentBase() = default;

std::shared_ptr<PrimitiveSceneProxy> BeamComponentBase::CreateSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::ElectricBeamSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> BeamComponentBase::ReplicateLuaProxy()
{
    return std::make_shared<ElectricBeamComponentLuaProxy>(std::static_pointer_cast<BeamComponentBase>(shared_from_this()));
}

const MeshRenderData& BeamComponentBase::GetRenderData() const
{
    return mRenderData;
}

void BeamComponentBase::Tick(const float deltaTime, const float playSpeed)
{
    Component::Tick(deltaTime, playSpeed);

    // No per-tick jitter regeneration: the animation is pre-baked into a frame set the render thread
    // cycles through on its own. Re-bake (+ resend) only when flagged.
    if (mAreFramesDirty) {
        GenerateAnimationFrames();
        SyncAnimationFrames();
        mAreFramesDirty = false;
    }
}

void BeamComponentBase::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    // Placement must be synced here and not in Tick: components tick BEFORE the owning actor's Tick body
    // sets the endpoints, so a Tick-time sync always ships the previous frame's matrix. UnpausableTick runs
    // after the whole Scene::Tick pass, and calling SyncTransformIfDirty before Base::UnpausableTick puts
    // the placement job in the render-thread queue ahead of the enable job — a pooled proxy re-enabled this
    // frame can never render with the stale matrix of its previous use (old "electro chain flicker" bug).
    SyncTransformIfDirty();
    Base::UnpausableTick(deltaTimeSec, playSpeed);
}

void BeamComponentBase::OnRegistered()
{
    LogInfo("BeamComponentBase::OnRegistered: ", GetEngineObjectName());
    mAreFramesDirty = true;
}

void BeamComponentBase::OnUnregistered()
{
    LogInfo("BeamComponentBase::OnUnregistered: ", GetEngineObjectName());
}

void BeamComponentBase::SetStartWorldPosition(const glm::vec3& point)
{
    if (mStartWorldPosition != point) {
        mStartWorldPosition = point;
        OnEndpointsChanged();
    }
}

void BeamComponentBase::SetEndWorldPosition(const glm::vec3& point)
{
    if (mEndWorldPosition != point) {
        mEndWorldPosition = point;
        OnEndpointsChanged();
    }
}

void BeamComponentBase::SetBeamThickness(float thickness)
{
    mBeamThickness = thickness;
    mAreFramesDirty = true; // thickness drives the cross-section radius
}

void BeamComponentBase::SetBeamCount(const int32_t count)
{
    const int32_t clampedCount = count < c_minBeamsCount ? c_minBeamsCount : (count > c_maxBeamsCount ? c_maxBeamsCount : count);

    mBeamCount = clampedCount;
    mAreFramesDirty = true;
}

void BeamComponentBase::SetJitterAmount(const float amount)
{
    mJitterAmount = amount;
    mAreFramesDirty = true;
}

void BeamComponentBase::SetUpdateFrequency(const float frequency)
{
    mUpdateFrequency = frequency;
    mAreFramesDirty = true; // changes the frame-advance cadence shipped to the proxy
}

void BeamComponentBase::SetRadialSegments(const int32_t radialSegments)
{
    mRadialSegments = radialSegments < 3 ? 3 : radialSegments;
    mAreFramesDirty = true;
}

void BeamComponentBase::SetAnimationSpeed(const float speed)
{
    const float clampedSpeed = speed < 0.0f ? 0.0f : speed;
    mAnimationSpeed = clampedSpeed;
    mAreFramesDirty = true;
    LogInfo("BeamComponentBase::SetAnimationSpeed: ", clampedSpeed);
}

glm::vec3 BeamComponentBase::GetStartPoint() const
{
    return mStartWorldPosition;
}

glm::vec3 BeamComponentBase::GetEndPoint() const
{
    return mEndWorldPosition;
}

float BeamComponentBase::GetBeamThickness() const
{
    return mBeamThickness;
}

int32_t BeamComponentBase::GetBeamCount() const
{
    return mBeamCount;
}

float BeamComponentBase::GetJitterAmount() const
{
    return mJitterAmount;
}

float BeamComponentBase::GetUpdateFrequency() const
{
    return mUpdateFrequency;
}

float BeamComponentBase::GetAnimationSpeed() const
{
    return mAnimationSpeed;
}

int32_t BeamComponentBase::GetRadialSegments() const
{
    return mRadialSegments;
}

int32_t BeamComponentBase::GetLengthSegments() const
{
    return mLengthSegments;
}

RuntimeGeneratedMeshPoolParameters BeamComponentBase::GetRuntimeGeneratedMeshPoolParameters() const
{
    return mBeamMeshPoolParams;
}

void BeamComponentBase::GenerateAnimationFrames()
{
    mAnimationFrames.clear();

    if (!CanBakeGeometry()) {
        return; // e.g. degenerate endpoints — proxy keeps its previous frames / renders nothing
    }

    mAnimationFrames.resize(c_animationFramesCount);

    const float frameStep = (mUpdateFrequency > 0.0f) ? mUpdateFrequency : 0.05f;

    for (int32_t frame = 0; frame < c_animationFramesCount; ++frame) {
        const float frameTime = static_cast<float>(frame) * frameStep;

        auto& beamsForFrame = mAnimationFrames[frame];
        beamsForFrame.reserve(mBeamCount);

        for (int32_t beam = 0; beam < mBeamCount; ++beam) {
            std::vector<BeamVertex> vertices;
            std::vector<uint32_t> indices;
            BakeBeamMesh(frameTime + beam * 0.5f, vertices, indices); // per-beam animation offset, as before
            beamsForFrame.emplace_back(std::move(vertices), std::move(indices));
        }
    }

    LogInfo("BeamComponentBase::GenerateAnimationFrames: baked ", c_animationFramesCount, " frames x ", mBeamCount, " beams");
}

void BeamComponentBase::SyncAnimationFrames()
{
    static constexpr uint64_t functionId = Hash64_CT("BeamComponentBase::SyncAnimationFrames");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [sceneProxyId = mSceneProxyId, beamCount = mBeamCount, frameStep = mUpdateFrequency, frames = mAnimationFrames](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& beamProxySp = std::static_pointer_cast<Graphics::Proxy::ElectricBeamSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        beamProxySp->SetAnimationFrames(frames, beamCount, frameStep);
                    }
                }
            });
    }
}

} // namespace EngineCore
