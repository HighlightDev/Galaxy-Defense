#pragma once
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>

namespace MeshLoader
{
   struct MeshNode
   {
      std::string Name;
      std::vector<MeshNode*> Children;
      glm::mat4 NodeTransformation;

      ~MeshNode()
      {
         for (size_t i = 0; i < Children.size(); ++i)
         {
            delete Children[i];
         }
      }
   };

}
