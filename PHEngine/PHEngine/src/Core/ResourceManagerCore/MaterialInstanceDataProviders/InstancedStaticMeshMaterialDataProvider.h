#pragma once

#include <atomic>

#include "MaterialInstanceDataProvider.h"

namespace Resources
{
    class InstancedStaticMeshMaterialDataProvider : public IMaterialInstanceDataProvider
    {
        std::atomic_bool mIsBeingUpdatedDataOnRenderThread{false};

    protected:
        void UpdateData(); // todo: for now

        virtual std::string GetBatchKey() const = 0;
    };
}
