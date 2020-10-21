#pragma once
#include <vector>
#include <cereal/types/vector.hpp>

#include "SerializeData.h"
#include <glm/vec3.hpp>

namespace Game
{
   struct SerializeDataContainer
   {
      std::vector<SerializeDataActor> Actors;

      template <typename Archive>
      void serialize(Archive& archive)
      {
         archive(Actors);
      }
   };
}
