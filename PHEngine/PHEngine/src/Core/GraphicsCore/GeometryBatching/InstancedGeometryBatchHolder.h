#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>
#include <string>
#include <vector>

namespace EngineCore {
class Scene;
class InstancedGeometryBatch;

class InstancedGeometryBatchHolder : public ITickable {
    std::weak_ptr<Scene> mSceneWp;

    std::vector<std::shared_ptr<InstancedGeometryBatch>> mInstancedGeometryBatches;

public:
    explicit InstancedGeometryBatchHolder();

    void SetScene(const std::weak_ptr<Scene>& sceneWp);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void AddInstancedGeometryBatch(const std::shared_ptr<InstancedGeometryBatch>& batch);

    void CleanUp();

    bool CheckIfBatchExists(std::string_view batchKey) const;

    std::shared_ptr<InstancedGeometryBatch> GetBatch(std::string_view batchKey) const;
};
} // namespace EngineCore
