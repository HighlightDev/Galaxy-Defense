#pragma once
#include <vector>
#include <cereal/types/vector.hpp>
#include <glm/vec3.hpp>

#include "SerializeData.h"

namespace Game
{
   struct SerializeDataContainer
   {
      SerializeAllocatedResources Resources;

      std::vector<SerializeDataActor> Actors;

      std::unique_ptr<SerializeDataPlayerController> PlayerControllerData;

      std::vector<std::shared_ptr<SerializeDataCamera>> Cameras;

      template <typename Archive>
      void serialize(Archive& archive)
      {
         archive(Resources, Actors, PlayerControllerData, Cameras);
      }
   };
}
