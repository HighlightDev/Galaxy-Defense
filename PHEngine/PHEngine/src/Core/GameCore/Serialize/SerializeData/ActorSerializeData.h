#pragma once
#include <string>
#include <glm/vec3.hpp>

namespace Game
{
   struct ActorSerializeData
   {
      std::string ActorName;
      glm::vec3 RootCompTranslation;
      glm::vec3 RootCompRotation;
      glm::vec3 RootCompScale;
   };
}
