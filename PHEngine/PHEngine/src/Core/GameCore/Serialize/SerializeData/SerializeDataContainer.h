#pragma once
#include "SerializeData.h"

#include <cereal/types/vector.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace EngineCore {
struct SerializeDataContainer {
    SerializeAllocatedResources Resources;

    std::vector<std::shared_ptr<SerializeDataPlayerController>> ActorControllerData;

    std::vector<SerializeDataActor> Actors;

    std::vector<std::shared_ptr<SerializeDataCamera>> Cameras;

    template<typename Archive>
    void serialize(Archive& archive)
    {
        archive(Resources, Actors, ActorControllerData, Cameras);
    }
};
} // namespace EngineCore
