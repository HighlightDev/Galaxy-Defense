#pragma once

#include <atomic>
#include <string>

#include "MaterialInstanceDataProvider.h"

namespace Resources
{
    class InstancedStaticMeshMaterialDataProvider : public MaterialInstanceDataProvider
    {
    protected:
        void UpdateInstancedDataProvider(); // todo: for now

        virtual std::string GetBatchKey() const = 0;

    };
}
