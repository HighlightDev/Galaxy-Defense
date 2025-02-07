#pragma once

#include "SerializeData/SerializeDataContainer.h"

#include <cereal/archives/binary.hpp>

namespace EngineCore {
struct ISerializable {
    virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;
};
} // namespace EngineCore