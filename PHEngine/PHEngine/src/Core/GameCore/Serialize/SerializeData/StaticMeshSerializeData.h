#pragma once

#include "ComponentSerializeData.h"

namespace Game
{
   struct StaticMeshSerializeData
      : public ComponentSerializeData
   {
      std::string modelName;
      glm::vec3 translation;
      glm::vec3 rotation;
      glm::vec3 scale;
      std::string luaScriptName;
      // todo: material
   };
}
