#pragma once

#include <memory>
#include <string>
#include <vector>

namespace EngineCore {
class InstancedStaticMeshComponent;

class InstancedGeometryBatch {
    std::vector<std::weak_ptr<InstancedStaticMeshComponent>> mInstancedStaticMeshComponents;

    std::string mBatchKey;

    std::vector<int32_t /*proxy id*/> mCachedValidInstances;

    bool mIsInstanceValidityDirty;

public:
    explicit InstancedGeometryBatch(const std::string& batchKey);

    void AddInstancedMeshComponent(const std::shared_ptr<InstancedStaticMeshComponent>& componentSp);

    bool UpdateBatchValidityState();

    const std::vector<int32_t>& GetValidInstances() const;

    const std::string& GetBatchKey() const;

    bool IsValidInstance(const int32_t proxyId) const;

    int32_t GetRenderInstanceId(const int32_t proxyId) const;

    int32_t GetInstancesCount() const;

    void SetInstanceValidityDirty();
};
} // namespace EngineCore
