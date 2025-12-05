#include "StaticMeshComponent.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"

#include <algorithm>
#include <memory>

using namespace Graphics;

namespace EngineCore {

StaticMeshComponent::StaticMeshComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData, const MeshRenderData renderData)
    : PrimitiveComponent(
        meshComponentData->EngineObjectName,
        meshComponentData->m_translation,
        meshComponentData->m_eulerRotationDegrees,
        meshComponentData->m_scale)
    , m_renderData(renderData)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
}

void StaticMeshComponent::Tick(const float deltaTimeSec)
{
    Base::Tick(deltaTimeSec);
}

void StaticMeshComponent::SetIsEnabled(const bool bEnabled)
{
    PrimitiveComponent::SetIsEnabled(bEnabled);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(bEnabled);
    }
}

void StaticMeshComponent::SetIsVisible(bool isVisible)
{
    PrimitiveComponent::SetIsVisible(isVisible);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(isVisible);
    }
}

std::shared_ptr<IMaterial> StaticMeshComponent::GetMaterial() const
{
    std::shared_ptr<IMaterial> materialResult = nullptr;
    if (const auto& sceneSP = m_sceneWP.lock()) {
        materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
    }
    ext_assert(materialResult != nullptr, "StaticMeshComponent::GetMaterial: materialResult is null");
    return materialResult;
}

std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::CreateSceneProxy() const
{
    return std::make_shared<StaticMeshSceneProxy>(this);
}

eComponentType StaticMeshComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void StaticMeshComponent::SetMeshModelPath(const std::string& modelPath)
{
    if (!modelPath.empty() && m_renderData.mModelPath != modelPath) {
        m_renderData.mModelPath = modelPath;
        if (const auto& sceneSp = m_sceneWP.lock()) {
            if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                sceneRenderer->UpdateMeshModelPath_OnRenderThread(
                    GetSceneProxyId(), ePrimitiveProxyType::STATIC_MESH_PROXY, modelPath);
            }
        }
    }
}

} // namespace EngineCore
