#include "InstancedGeometryBatchHolder.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchRenderer.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "InstancedGeometryBatch.h"

using namespace Renderer;
using namespace Graphics::GeometryBatching;

namespace EngineCore {
InstancedGeometryBatchHolder::InstancedGeometryBatchHolder()
{
}

void InstancedGeometryBatchHolder::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void InstancedGeometryBatchHolder::Tick(const float deltaTime)
{
    if (!mInstancedGeometryBatches.size())
        return;

    if (const auto& sceneSp = mSceneWp.lock()) {
        auto& threadMngr = sceneSp->GetInterThreadCommunicationManager();
        if (const auto& sceneRendererSp = threadMngr.GetSceneRendererWP().lock()) {
            std::unordered_map<std::string /*batch key*/, std::vector<int32_t> /*proxy ids with correct render order*/> batchData;
            batchData.reserve(mInstancedGeometryBatches.size());
            for (const auto& batchSp : mInstancedGeometryBatches) {
                batchSp->Tick(deltaTime);
                batchData[batchSp->GetBatchKey()] = batchSp->GetValidInstances();
            }

            static constexpr auto functionId = Hash64_CT("InstancedGeometryBatchHolder::SyncBatchData");
            threadMngr.ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                0,
                functionId,
                [sceneRendererSp, data = std::move(batchData)]( std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    const auto& geometryBatcherRenderer = sceneRendererSp->GetInstancedGeometryBatchRenderer();
                    geometryBatcherRenderer->UpdateBatchInstancesData(data);
                });
        }
    }
}

void InstancedGeometryBatchHolder::UnpausableTick(const float deltaTime)
{
}

void InstancedGeometryBatchHolder::AddInstancedGeometryBatch(const std::shared_ptr<InstancedGeometryBatch>& batch)
{
    const auto isNewBatch = std::none_of(
        mInstancedGeometryBatches.cbegin(),
        mInstancedGeometryBatches.cend(),
        [newBatchKey = batch->GetBatchKey()](const auto& batchSp) { return newBatchKey == batchSp->GetBatchKey(); });

    assert(isNewBatch);
    mInstancedGeometryBatches.emplace_back(batch);
}

void InstancedGeometryBatchHolder::CleanUp()
{
    mInstancedGeometryBatches.clear();
}

bool InstancedGeometryBatchHolder::CheckIfBatchExists(const std::string& batchKey) const
{
    const auto bBatchExists
        = std::any_of(mInstancedGeometryBatches.cbegin(), mInstancedGeometryBatches.cend(), [batchKey](const auto& batchSp) {
              return batchKey == batchSp->GetBatchKey();
          });
    return bBatchExists;
}

std::shared_ptr<InstancedGeometryBatch> InstancedGeometryBatchHolder::GetBatch(const std::string& batchKey) const
{
    const auto foundIt
        = std::find_if(mInstancedGeometryBatches.cbegin(), mInstancedGeometryBatches.cend(), [batchKey](const auto& batchSp) {
              return batchKey == batchSp->GetBatchKey();
          });
    return foundIt != mInstancedGeometryBatches.cend() ? *foundIt : nullptr;
}
} // namespace EngineCore
