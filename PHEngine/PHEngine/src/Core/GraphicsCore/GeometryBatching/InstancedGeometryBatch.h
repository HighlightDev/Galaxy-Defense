#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>
#include <string>
#include <vector>

namespace EngineCore {
class InstancedStaticMeshComponent;

class InstancedGeometryBatch : public ITickable {
    std::vector<std::weak_ptr<InstancedStaticMeshComponent>> mInstancedStaticMeshComponents;

    std::string mBatchKey;

    std::vector<int32_t /*proxy id*/> mCachedValidInstances;

public:
    explicit InstancedGeometryBatch(const std::string& batchKey);

    void AddInstancedMeshComponent(const std::shared_ptr<InstancedStaticMeshComponent>& componentSp);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    std::vector<int32_t> GetValidInstances() const;

    std::string GetBatchKey() const;

    bool IsValidInstance(const int32_t proxyId) const;

    int32_t GetRenderInstanceId(const int32_t proxyId) const;

    int32_t GetInstancesCount() const;
};
} // namespace EngineCore
