#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace EnginePhysics
{
   class DebugPhysicsRenderData
   {
      std::vector<std::pair<glm::vec3, glm::vec3>> mDebugLines;

   public:

      void AddLine(const glm::vec3& from, const glm::vec3& to) {

         mDebugLines.emplace_back(std::make_pair(from, to));
      }

      std::vector<std::pair<glm::vec3, glm::vec3>>& GetDebugLines() {
         return mDebugLines;
      }

      void ClearLines() {
         mDebugLines.clear();
      }

      DebugPhysicsRenderData()
         : mDebugLines() 
      {
      }
      
   };
}
