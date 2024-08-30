#include "InstancedStaticMeshMaterialDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatcher.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"
#include "Core/CommonCore/Assertion.h"

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Graphics::GeometryBatching;

namespace Resources
{
    void InstancedStaticMeshMaterialDataProvider::UpdateData()
    {
        static const uint64_t functionId = Hash("InstancedStaticMeshMaterialDataProvider::UpdateData");
        if (!mIsBeingUpdatedDataOnRenderThread.load(std::memory_order::memory_order_seq_cst))
        {
            if (const auto &sceneSp = GetSceneWp().lock())
            {
                if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    mIsBeingUpdatedDataOnRenderThread.store(true, std::memory_order::memory_order_seq_cst);
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetInstanceObjectId(), functionId, [this, sceneRendererSp]()
                                                                                        {
                    const auto &instancedGeometryBatcherSp = sceneRendererSp->GetInstancedGeometryBatcher();
                    const auto &batchSp = instancedGeometryBatcherSp->GetBatch(GetBatchKey());
                    mIsInstanceActive = batchSp->IsProxyActive(GetInstanceProxyId());
                    mInstanceId = batchSp->GetInstanceId(GetInstanceProxyId());
                    mIsBeingUpdatedDataOnRenderThread.store(false, std::memory_order::memory_order_seq_cst); });
                }
            }
        }
    }

    void InstancedStaticMeshMaterialDataProvider::BindEngineObjectToPropertyName(const std::shared_ptr<EngineObject> &engineObject,
                                                                                 const std::string &propertyName)
    {
        assert(!mStoredPropertiesInEngineObjectsMap.count(propertyName));
        mStoredPropertiesInEngineObjectsMap.emplace(propertyName, engineObject);
    }

    std::shared_ptr<EngineObject> InstancedStaticMeshMaterialDataProvider::GetEngineObjectByPropertyName(const std::string &propertyName) const
    {
        assert(mStoredPropertiesInEngineObjectsMap.count(propertyName));
        return mStoredPropertiesInEngineObjectsMap.at(propertyName).lock();
    }

    int32_t InstancedStaticMeshMaterialDataProvider::GetInstanceId() const
    {
        return mInstanceId;
    }

    bool InstancedStaticMeshMaterialDataProvider::IsInstanceActive() const
    {
        return mIsInstanceActive;
    }
}
