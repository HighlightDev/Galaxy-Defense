#pragma once

#include "MaterialInstanceDataProvider.h"

#include <atomic>
#include <string>

namespace Resources {
class InstancedStaticMeshMaterialDataProvider : public MaterialInstanceDataProvider {
protected:
    void UpdateInstancedDataProvider();

    virtual std::string GetBatchKey() const = 0;
};
} // namespace Resources
