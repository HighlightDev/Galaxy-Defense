#include "SkeletalMeshComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/SkeletalMeshComponentLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaWrapper.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/IoCore/FolderManager.h"

#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace EngineCore::Scripts;

namespace EngineCore {
SkeletalMeshComponent::SkeletalMeshComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData, const MeshRenderData& renderData)
    : PrimitiveComponent(
        meshComponentData->EngineObjectName,
        meshComponentData->m_translation,
        meshComponentData->m_eulerRotationDegrees,
        meshComponentData->m_scale,
        meshComponentData->mIsEnabled,
        meshComponentData->mIsVisible)
    , m_renderData(renderData)
    , mLuaInstance(std::make_unique<LuaWrapper>())
    , mUpdateDataResetTimeCounter(0.0f)
    , mUpdateDataResetTime(0.1f)
    , mTimeIncreaseMultiply(1.0f)
    , SrcAnimationTime(std::make_shared<EngineObjectProperty<float>>(0.0f, "SrcAnimTime"))
    , DstAnimationTime(std::make_shared<EngineObjectProperty<float>>(0.0f, "DstAnimTime"))
    , SrcAnimationName(std::make_shared<EngineObjectProperty<std::string>>("", "SrcAnimName"))
    , DstAnimationName(std::make_shared<EngineObjectProperty<std::string>>("", "DstAnimName"))
    , TransitionValue(std::make_shared<EngineObjectProperty<float>>(0.0f, "AnimTransitionValue"))
    , bTransitionEnabled(std::make_shared<EngineObjectProperty<bool>>(false, "bAnimTransitionEnabled"))
{
    /* Meta table */
    AddEngineProperty(SrcAnimationTime);
    AddEngineProperty(DstAnimationTime);
    AddEngineProperty(SrcAnimationName);
    AddEngineProperty(DstAnimationName);
    AddEngineProperty(TransitionValue);
    AddEngineProperty(bTransitionEnabled);
    /* Meta table */
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
}

void SkeletalMeshComponent::OnSceneOwnerInitialized()
{
    PrimitiveComponent::OnSceneOwnerInitialized();
}

void SkeletalMeshComponent::SetIsEnabled(const bool bEnabled)
{
    PrimitiveComponent::SetIsEnabled(bEnabled);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(bEnabled);
    }
}

void SkeletalMeshComponent::SetIsVisible(bool isVisible)
{
    PrimitiveComponent::SetIsVisible(isVisible);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(isVisible);
    }
}

void SkeletalMeshComponent::SetTimeIncreaseMultiply(const float timeMultiply)
{
    mTimeIncreaseMultiply = timeMultiply;
}

std::shared_ptr<IMaterial> SkeletalMeshComponent::GetMaterial() const
{
    // get from scene corresponding to material proxy material instance
    std::shared_ptr<IMaterial> materialResult = nullptr;
    if (const auto& sceneSP = m_sceneWP.lock()) {
        materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
    }
    ext_assert(materialResult != nullptr, "SkeletalMeshComponent::GetMaterial: materialResult is null");
    return materialResult;
}

eComponentType SkeletalMeshComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void SkeletalMeshComponent::Tick(const float deltaTimeSec)
{
    SrcAnimationTime->SetValue(SrcAnimationTime->GetValue() + (deltaTimeSec * mTimeIncreaseMultiply));
    mUpdateDataResetTimeCounter += deltaTimeSec;
    const bool bUpdateData = mUpdateDataResetTimeCounter >= mUpdateDataResetTime;
    mUpdateDataResetTimeCounter = fmod(mUpdateDataResetTimeCounter, mUpdateDataResetTime);

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst) && (bUpdateData || bIsRenderDataDirty)) {
        SyncDataWithRenderThread();
        bIsRenderDataDirty = false;
    }
}

void SkeletalMeshComponent::SyncDataWithRenderThread()
{
    static const uint64_t functionId = Hash("SkeletalMeshComponent::SyncDataWithRenderThread");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                GetObjectId(),
                functionId,
                [sceneRenderer,
                 sceneProxyId = mSceneProxyId,
                 isTransition = bTransitionEnabled->GetValue(),
                 transitionValue = TransitionValue->GetValue(),
                 srcAnimationTime = SrcAnimationTime->GetValue(),
                 dstAnimationTime = DstAnimationTime->GetValue(),
                 srcAnimation = SrcAnimationName->GetValue(),
                 dstAnimation = DstAnimationName->GetValue()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    if (const auto& primitiveProxySp = std::static_pointer_cast<SkeletalMeshSceneProxy>(
                            sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        primitiveProxySp->UpdateAnimationData(
                            isTransition, transitionValue, srcAnimationTime, dstAnimationTime, srcAnimation, dstAnimation);
                    }
                });
        }
    }
}

std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
{
    return std::make_shared<SkeletalMeshSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> SkeletalMeshComponent::ReplicateLuaProxy()
{
    return std::make_shared<SkeletalMeshComponentLuaProxy>(std::static_pointer_cast<SkeletalMeshComponent>(shared_from_this()));
}

void SkeletalMeshComponent::SetMeshModelPath(const std::string& modelPath)
{
    if (!modelPath.empty() && m_renderData.mModelPath != modelPath) {
        m_renderData.mModelPath = modelPath;
        if (const auto& sceneSp = m_sceneWP.lock()) {
            if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                sceneRenderer->UpdateMeshModelPath_OnRenderThread(
                    GetSceneProxyId(), ePrimitiveProxyType::SKELETAL_MESH_PROXY, modelPath);
            }
        }
    }
}
} // namespace EngineCore