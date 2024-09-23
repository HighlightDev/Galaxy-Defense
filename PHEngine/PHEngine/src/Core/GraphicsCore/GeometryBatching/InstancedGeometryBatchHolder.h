#pragma once

#include <memory>
#include <vector>

#include "Core/GameCore/ITickable.h"

namespace EngineCore
{
    class Scene;
    class InstancedGeometryBatch;

    class InstancedGeometryBatchHolder
        : public ITickable
    {
        std::weak_ptr<Scene> mSceneWp;

        std::vector<std::shared_ptr<InstancedGeometryBatch>> mInstancedGeometryBatches;

    public:
        explicit InstancedGeometryBatchHolder();

        void SetScene(const std::weak_ptr<Scene> &sceneWp);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddInstancedGeometryBatch(const std::shared_ptr<InstancedGeometryBatch> &batch);

        void CleanUp();

        bool CheckIfBatchExists(const std::string &batchKey) const;

        std::shared_ptr<InstancedGeometryBatch> GetBatch(const std::string &batchKey) const;
    };
}
