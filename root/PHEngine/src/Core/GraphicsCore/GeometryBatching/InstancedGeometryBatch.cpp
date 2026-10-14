#include "InstancedGeometryBatch.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"

namespace EngineCore {
InstancedGeometryBatch::InstancedGeometryBatch(const std::string& batchKey)
    : mBatchKey(batchKey)
    , mIsInstanceValidityDirty(false)
{
}

void InstancedGeometryBatch::AddInstancedMeshComponent(const std::shared_ptr<InstancedStaticMeshComponent>& componentSp)
{
    const auto isNewComp = std::none_of(
        mInstancedStaticMeshComponents.cbegin(),
        mInstancedStaticMeshComponents.cend(),
        [id = componentSp->GetObjectId()](const auto& componentWp) {
            if (const auto& compSp = componentWp.lock()) {
                return compSp->GetObjectId() == id;
            }
            return false;
        });
    ext_assert(isNewComp, "InstancedGeometryBatch::AddInstancedMeshComponent: Component already exists in batch");

    mInstancedStaticMeshComponents.emplace_back(componentSp);
    mIsInstanceValidityDirty = true;
}

bool InstancedGeometryBatch::UpdateBatchValidityState()
{
    if (not mIsInstanceValidityDirty) {
        return false;
    }

    mCachedValidInstances.clear();
    mCachedValidInstances.reserve(mInstancedStaticMeshComponents.size());

    for (const auto& compWp : mInstancedStaticMeshComponents) {
        if (const auto& compSp = compWp.lock()) {
            if (compSp->IsEnabled() && compSp->IsVisible()) {
                mCachedValidInstances.emplace_back(compSp->GetSceneProxyId());
            }
        }
    }
    mIsInstanceValidityDirty = false;
    return true;
}

const std::vector<int32_t>& InstancedGeometryBatch::GetValidInstances() const
{
    return mCachedValidInstances;
}

const std::string& InstancedGeometryBatch::GetBatchKey() const
{
    return mBatchKey;
}

bool InstancedGeometryBatch::IsValidInstance(const int32_t proxyId) const
{
    return std::find(mCachedValidInstances.cbegin(), mCachedValidInstances.cend(), proxyId) != mCachedValidInstances.cend();
}

int32_t InstancedGeometryBatch::GetRenderInstanceId(const int32_t proxyId) const
{
    const auto foundIt = std::find(mCachedValidInstances.cbegin(), mCachedValidInstances.cend(), proxyId);
    if (foundIt != mCachedValidInstances.cend()) {
        return std::distance(mCachedValidInstances.cbegin(), foundIt);
    }
    return -1;
}

int32_t InstancedGeometryBatch::GetInstancesCount() const
{
    return static_cast<int32_t>(mInstancedStaticMeshComponents.size());
}

void InstancedGeometryBatch::SetInstanceValidityDirty()
{
    mIsInstanceValidityDirty = true;
}

} // namespace EngineCore
