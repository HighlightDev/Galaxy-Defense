#include "InstancedStaticMeshComponent.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/InstancedStaticMeshComponentLuaProxy.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchHolder.h"
#include "Core/GraphicsCore/SceneProxy/InstancedStaticMeshSceneProxy.h"

#include <algorithm>
#include <memory>

using namespace Graphics;
using namespace EngineCore::Scripts;

namespace EngineCore {

InstancedStaticMeshComponent::InstancedStaticMeshComponent(
    const std::shared_ptr<InstancedMeshComponentData>& meshComponentData, const MeshRenderData renderData)
    : PrimitiveComponent(
        meshComponentData->EngineObjectName,
        meshComponentData->m_translation,
        meshComponentData->m_eulerRotationDegrees,
        meshComponentData->m_scale)
    , m_renderData(renderData)
{
}

InstancedStaticMeshComponent::~InstancedStaticMeshComponent()
{
}

void InstancedStaticMeshComponent::OnPostRegistered()
{
    PrimitiveComponent::OnPostRegistered();

    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto thisSp = std::static_pointer_cast<InstancedStaticMeshComponent>(shared_from_this());
        const auto& batchHolderSp = sceneSp->GetInstancedGeometryBatchHolder();
        const auto& batchKey = GetBatchKey();
        if (batchHolderSp->CheckIfBatchExists(batchKey)) {
            const auto& batchSp = batchHolderSp->GetBatch(batchKey);
            ext_assert(
                batchSp->GetInstancesCount() <= EngineConstants::c_maxInstancesPerInstanceBatch,
                "Exceeded max instances per batch, allowed: " + std::to_string(EngineConstants::c_maxInstancesPerInstanceBatch));
            batchSp->AddInstancedMeshComponent(thisSp);
        } else {
            const auto newBatchSp = std::make_shared<InstancedGeometryBatch>(batchKey);
            newBatchSp->AddInstancedMeshComponent(thisSp);
            batchHolderSp->AddInstancedGeometryBatch(newBatchSp);
        }
    }
}

void InstancedStaticMeshComponent::UnpausableTick(const float deltaTimeSec)
{
    PrimitiveComponent::UnpausableTick(deltaTimeSec);
    InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider();
}

void InstancedStaticMeshComponent::SetIsEnabled(const bool bEnabled)
{
    PrimitiveComponent::SetIsEnabled(bEnabled);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(mIsVisible && mIsEnabled);
    }
}

void InstancedStaticMeshComponent::SetIsVisible(bool isVisible)
{
    PrimitiveComponent::SetIsVisible(isVisible);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(mIsVisible && mIsEnabled);
    }
}

std::shared_ptr<IMaterial> InstancedStaticMeshComponent::GetMaterial() const
{
    std::shared_ptr<IMaterial> materialResult = nullptr;
    if (const auto& sceneSP = m_sceneWP.lock()) {
        materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
    }
    ext_assert(materialResult != nullptr, "InstancedStaticMeshComponent::GetMaterial: materialResult is null");
    return materialResult;
}

std::shared_ptr<PrimitiveSceneProxy> InstancedStaticMeshComponent::CreateSceneProxy() const
{
    return std::make_shared<InstancedStaticMeshSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> InstancedStaticMeshComponent::ReplicateLuaProxy()
{
    return std::make_shared<InstancedStaticMeshComponentLuaProxy>(
        std::static_pointer_cast<InstancedStaticMeshComponent>(shared_from_this()));
}

eComponentType InstancedStaticMeshComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

BoundingBox3D InstancedStaticMeshComponent::GetTransformedBoundingBox() const
{
    return {}; // todo: for now frustum cull test is not applyed to instanced geometry, so return wrong value anyway
}

std::string InstancedStaticMeshComponent::GetBatchKey() const
{
    return m_renderData.mModelPath + "_" + m_renderData.mMaterialProxy->MaterialName + "_deferred_"
        + std::to_string(m_renderData.mIsDeferredShaded);
}

std::weak_ptr<::EngineCore::Scene> InstancedStaticMeshComponent::GetSceneWp() const
{
    return m_sceneWP;
}

int32_t InstancedStaticMeshComponent::GetInstanceObjectId() const
{
    return mObjectId;
}

int32_t InstancedStaticMeshComponent::GetInstanceProxyId() const
{
    return GetSceneProxyId();
}
} // namespace EngineCore
