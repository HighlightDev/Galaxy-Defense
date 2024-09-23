#include "InstancedStaticMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GraphicsCore/SceneProxy/InstancedStaticMeshSceneProxy.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchHolder.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"

#include <memory>
#include <algorithm>

using namespace Graphics;

namespace EngineCore
{

    InstancedStaticMeshComponent::InstancedStaticMeshComponent(const std::shared_ptr<InstancedMeshComponentData> &meshComponentData,
                                                               const MeshRenderData renderData)
        : PrimitiveComponent(meshComponentData->EngineObjectName,
                             meshComponentData->m_translation,
                             meshComponentData->m_eulerRotationDegrees,
                             meshComponentData->m_scale),
          m_renderData(renderData)
    {
    }

    InstancedStaticMeshComponent::~InstancedStaticMeshComponent()
    {
    }

    void InstancedStaticMeshComponent::OnPostInitialized()
    {
        PrimitiveComponent::OnPostInitialized();

        if (const auto &sceneSp = m_sceneWP.lock())
        {
            const auto thisSp = std::static_pointer_cast<InstancedStaticMeshComponent>(shared_from_this());
            const auto &batchHolderSp = sceneSp->GetInstancedGeometryBatchHolder();
            const auto &batchKey = GetBatchKey();
            if (batchHolderSp->CheckIfBatchExists(batchKey))
            {
               const auto& batchSp = batchHolderSp->GetBatch(batchKey);
               batchSp->AddInstancedMeshComponent(thisSp);
            }
            else
            {
                const auto newBatchSp = std::make_shared<InstancedGeometryBatch>(batchKey);
                newBatchSp->AddInstancedMeshComponent(thisSp);
                batchHolderSp->AddInstancedGeometryBatch(newBatchSp);
            }
        }
    }

    void InstancedStaticMeshComponent::UnpausableTick(const float deltaTime)
    {
        PrimitiveComponent::UnpausableTick(deltaTime);
        InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider();
    }

    void InstancedStaticMeshComponent::SetIsEnabled(const bool bEnabled)
    {
        PrimitiveComponent::SetIsEnabled(bEnabled);

        const auto &material = GetMaterial();
        if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
        {
            material->SetIsEnabled(mIsVisible && mIsEnabled);
        }
    }

    void InstancedStaticMeshComponent::SetIsVisible(bool isVisible)
    {
        PrimitiveComponent::SetIsVisible(isVisible);

        const auto &material = GetMaterial();
        if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
        {
            material->SetIsEnabled(mIsVisible && mIsEnabled);
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

    std::string InstancedStaticMeshComponent::GetBatchKey() const
    {
        return m_renderData.mModelPath + "_" + m_renderData.mMaterialProxy->MaterialName;
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
}
