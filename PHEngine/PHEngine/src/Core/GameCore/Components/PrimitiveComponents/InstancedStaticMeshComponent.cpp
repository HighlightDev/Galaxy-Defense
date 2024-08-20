#include "InstancedStaticMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GraphicsCore/SceneProxy/InstancedStaticMeshSceneProxy.h"

#include <memory>
#include <algorithm>

using namespace Graphics;

namespace EngineCore
{

    InstancedStaticMeshComponent::InstancedStaticMeshComponent(const std::shared_ptr<InstancedMeshComponentData> &meshComponentData,
                                                               const MeshRenderData renderData)
        : PrimitiveComponent(meshComponentData->EngineObjectName,
                             {},
                             {},
                             glm::vec3(1.0f)),
          m_renderData(renderData)
    {
    }

    InstancedStaticMeshComponent::~InstancedStaticMeshComponent()
    {
    }

    void InstancedStaticMeshComponent::Tick(const float deltaTime)
    {
        Base::Tick(deltaTime);
    }

    void InstancedStaticMeshComponent::SetIsEnabled(const bool bEnabled)
    {
        PrimitiveComponent::SetIsEnabled(bEnabled);

        const auto &material = GetMaterial();
        if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
        {
            material->SetIsEnabled(bEnabled);
        }
    }

    void InstancedStaticMeshComponent::SetIsVisible(bool isVisible)
    {
        PrimitiveComponent::SetIsVisible(isVisible);

        const auto &material = GetMaterial();
        if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
        {
            material->SetIsEnabled(isVisible);
        }
    }

    std::shared_ptr<IMaterial> InstancedStaticMeshComponent::GetMaterial() const
    {
        std::shared_ptr<IMaterial> materialResult = nullptr;
        if (const auto &sceneSP = m_sceneWP.lock())
        {
            materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
        }
        assert(materialResult != nullptr);
        return materialResult;
    }

    void InstancedStaticMeshComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    std::shared_ptr<PrimitiveSceneProxy> InstancedStaticMeshComponent::CreateSceneProxy() const
    {
        return std::make_shared<InstancedStaticMeshSceneProxy>(this);
    }

    eComponentType InstancedStaticMeshComponent::GetComponentType() const
    {
        return PRIMITIVE_COMPONENT;
    }

    BoundingBox3D InstancedStaticMeshComponent::GetTransformedBoundingBox() const
    {
        return {}; // todo: for now frustum cull test is not applyed to instanced geometry, so return wrong value anyway
    }

}
